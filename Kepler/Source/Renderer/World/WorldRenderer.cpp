#include "WorldRenderer.h"
#include "../RenderGlobals.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/TargetRegistry.h"
#include "World/Game/Components/StaticMeshComponent.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TWorldRenderer::TWorldRenderer(TRef<TGameWorld> WorldToRender, TSharedPtr<TLowLevelRenderer> InLLR)
		:	CurrentWorld(WorldToRender)
		,	LLR(InLLR)
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

		pImmCtx->StartDrawingToRenderTargets(CurrentRenderTarget, DepthTarget);
		pImmCtx->ClearRenderTarget(CurrentRenderTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->ClearDepthTarget(DepthTarget, false);
		pImmCtx->SetViewport(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)CurrentViewport.Width, (float)CurrentViewport.Height);

		// For now just draw all the meshes with no pipeline sorting
		CurrentWorld->GetComponentView<TMaterialComponent, TStaticMeshComponent>().each(
			[pImmCtx](auto, TMaterialComponent& MT, TStaticMeshComponent& SM)
			{
				pImmCtx->BindParamBuffers(MT.GetMaterial()->GetParamBuffer(), 0);
				pImmCtx->BindVertexBuffers(SM.GetStaticMesh()->GetVertexBuffer(), 0, 0);
				pImmCtx->BindIndexBuffer(SM.GetStaticMesh()->GetIndexBuffer(), 0);
				pImmCtx->BindPipeline(MT.GetMaterial()->GetPipeline());
				pImmCtx->BindSamplers(MT.GetMaterial()->GetSamplers());
				pImmCtx->DrawIndexed(SM.GetStaticMesh()->GetIndexCount(), 0, 0);
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
		auto SwapChain = LLR->GetSwapChain(0);
		pImmCtx->StartDrawingToSwapChainImage(SwapChain);
		pImmCtx->ClearSwapChainImage(SwapChain, { 0.1f, 0.1f, 0.1f, 1.0f });
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