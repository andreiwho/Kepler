#include "WorldRenderer.h"
#include "../RenderGlobals.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/TargetRegistry.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Components/TransformComponent.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogWorldRenderer);

	//////////////////////////////////////////////////////////////////////////
	TWorldRenderer::TWorldRenderer(TRef<TGameWorld> WorldToRender, TSharedPtr<TLowLevelRenderer> InLLR)
		: CurrentWorld(WorldToRender)
		, LLR(InLLR)
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TWorldRenderer::~TWorldRenderer()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::Render(TViewport2D ViewportSize)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		CurrentViewport = ViewportSize;

		// Upload material data to the GPU
		TRef<TCommandListImmediate> pImmCtx = LLR->GetRenderDevice()->GetImmediateCommandList();

		RT_UpdateMaterialComponents(pImmCtx);

		// Prepare for rendering
		pImmCtx->BeginDebugEvent("ClearRenderState");
		{
			pImmCtx->ClearSamplers();
		}
		pImmCtx->EndDebugEvent();

		// Collect renderable objects
		// ...

		// Draw meshes
		MeshPass(pImmCtx);

		// Tonemapping
		FlushPass(pImmCtx);
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TWorldRenderer> TWorldRenderer::New(TRef<TGameWorld> WorldToRender, TSharedPtr<TLowLevelRenderer> InLLR)
	{
		KEPLER_PROFILE_SCOPE();
		return MakeRef(Kepler::New<TWorldRenderer>(WorldToRender, InLLR));
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::RT_UpdateMaterialComponents(TRef<TCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("RT_UpdateMaterialComponents");
		auto Camera = CurrentWorld->GetMainCamera();

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// TODO: Change camera sizes prematurely.
		// Need to define camera to render target relationships and use those to control camera frustums
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if (CurrentWorld->IsValidEntity(Camera) && CurrentWorld->IsCamera(Camera))
		{
			auto& CameraComponent = CurrentWorld->GetComponent<TCameraComponent>(Camera);
			if (TTargetRegistry::Get()->RenderTargetGroupExists(CameraComponent.GetRenderTargetName()))
			{
				if (auto RenderTarget = TTargetRegistry::Get()->GetRenderTargetGroup(CameraComponent.GetRenderTargetName()))
				{
					auto& MathCamera = CurrentWorld->GetComponent<TCameraComponent>(Camera).GetCamera();
					MathCamera.SetFrustumWidth(RenderTarget->GetRenderTargetAtArrayLayer(0)->GetWidth());
					MathCamera.SetFrustumHeight(RenderTarget->GetRenderTargetAtArrayLayer(0)->GetHeight());
				}
				else
				{
					KEPLER_WARNING(LogWorldRenderer, "Requested render target '{}' for camera does not exist", CameraComponent.GetRenderTargetName());
				}
			}
		}

		CurrentWorld->GetComponentView<TMaterialComponent>().each(
			[this, pImmCtx](auto, TMaterialComponent& Component)
			{
				Component.GetMaterial()->RT_Update(pImmCtx);
			});
		pImmCtx->EndDebugEvent();
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::CollectRenderableViews()
	{
		KEPLER_PROFILE_SCOPE();
	}

	namespace
	{
		struct TDrawCall
		{
			TRef<TParamBuffer> ParamBuffer;
			TRef<TGraphicsPipeline> Pipeline;
			TRef<TPipelineSamplerPack> Samplers;
			TRef<TStaticMesh> StaticMesh;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::MeshPass(TRef<TCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("MeshPass");
		const u32 FrameIndex = LLR->GetFrameIndex();

		// Note: Now it is a simple forward renderer, but it needs to become deferred...
		// Configure mesh pass render target
		auto RenderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"MeshPassTarget",
			CurrentViewport.Width,
			CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			TLowLevelRenderer::SwapChainFrameCount);
		TRef<TRenderTarget2D> CurrentRenderTarget = RenderTargetGroup->GetRenderTargetAtArrayLayer(FrameIndex);

		auto DepthTarget = TTargetRegistry::Get()->GetDepthTarget("MeshPassDepth",
			CurrentViewport.Width,
			CurrentViewport.Height,
			EFormat::D24_UNORM_S8_UINT,
			false);

		// Configure render target which will contain entity ids
		auto IdTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"IdTarget",
			CurrentViewport.Width,
			CurrentViewport.Height,
			EFormat::R32_SINT,
			1,
			true);
		auto IdTarget = IdTargetGroup->GetRenderTargetAtArrayLayer(0);

		pImmCtx->StartDrawingToRenderTargets({ CurrentRenderTarget, IdTarget }, DepthTarget);
		pImmCtx->ClearRenderTarget(CurrentRenderTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->ClearRenderTarget(IdTarget, float4(-1.0f));
		pImmCtx->ClearDepthTarget(DepthTarget, true);
		pImmCtx->SetViewport(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height);

		// For now just draw all the meshes with no pipeline sorting
		CurrentWorld->GetComponentView<TMaterialComponent, TStaticMeshComponent>().each(
			[pImmCtx](auto, TMaterialComponent& MT, TStaticMeshComponent& SM)
			{
				pImmCtx->BindParamBuffers(MT.GetMaterial()->GetParamBuffer(), 0);
				pImmCtx->BindPipeline(MT.GetMaterial()->GetPipeline());
				pImmCtx->BindSamplers(MT.GetMaterial()->GetSamplers());
				for (const auto& Section : SM.GetStaticMesh()->GetSections())
				{
					pImmCtx->BindVertexBuffers(Section.VertexBuffer, 0, 0);
					pImmCtx->BindIndexBuffer(Section.IndexBuffer, 0);
					pImmCtx->DrawIndexed(Section.IndexBuffer->GetCount(), 0, 0);
				}
			}
		);
		pImmCtx->EndDebugEvent();
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::FlushPass(TRef<TCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		// For now just flush the mesh pass image
		pImmCtx->BeginDebugEvent("Screen Quad Pass");
		// Main swap chain
#ifdef ENABLE_EDITOR
		auto RenderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"EditorViewport",
			CurrentViewport.Width,
			CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			TLowLevelRenderer::SwapChainFrameCount);

		const u32 FrameIndex = LLR->GetFrameIndex();
		TRef<TRenderTarget2D> CurrentRenderTarget = RenderTargetGroup->GetRenderTargetAtArrayLayer(FrameIndex);
		pImmCtx->StartDrawingToRenderTargets(CurrentRenderTarget, nullptr);
		pImmCtx->ClearRenderTarget(CurrentRenderTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->SetViewport(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height);
#else
		auto SwapChain = LLR->GetSwapChain(0);
		pImmCtx->StartDrawingToSwapChainImage(SwapChain);
		pImmCtx->ClearSwapChainImage(SwapChain, { 0.1f, 0.1f, 0.1f, 1.0f });
#endif
		pImmCtx->BindVertexBuffers(LLR->ScreenQuad.VertexBuffer, 0, 0);
		pImmCtx->BindIndexBuffer(LLR->ScreenQuad.IndexBuffer, 0);
		pImmCtx->BindPipeline(LLR->ScreenQuad.Pipeline);

		//Write quad sampler
		auto RenderTarget = TTargetRegistry::Get()->GetRenderTargetGroup("MeshPassTarget");
		CHECK(RenderTarget);
		auto SamplerToDraw = RenderTarget->GetTextureSamplerAtArrayLayer(LLR->GetFrameIndex());

		LLR->ScreenQuad.Samplers->Write("RenderTarget", SamplerToDraw);
		// and
		pImmCtx->BindSamplers(LLR->ScreenQuad.Samplers);
		pImmCtx->DrawIndexed(LLR->ScreenQuad.IndexBuffer->GetCount(), 0, 0);
		pImmCtx->EndDebugEvent();
	}
}