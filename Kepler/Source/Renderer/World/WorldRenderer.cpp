#include "WorldRenderer.h"
#include "../RenderGlobals.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/TargetRegistry.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Components/TransformComponent.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogWorldRenderer);

	//////////////////////////////////////////////////////////////////////////
	TWorldRenderer::TWorldRenderer(TRef<TGameWorld> pWorld)
		: m_CurrentWorld(pWorld)
		, m_LLR(TLowLevelRenderer::Get())
	{
		if (!StaticState.bInitialized)
		{
			StaticState.Init();
		}
	}

	void TWorldRenderer::TStaticState::Init()
	{
		CHECK(IsRenderThread());
		TRef<TPipelineParamMapping> RS_CameraParams = TPipelineParamMapping::New();
		RS_CameraParams->AddParam("ViewProjection", offsetof(RS_CameraBufferStruct, ViewProjection), sizeof(RS_CameraBufferStruct::ViewProjection), EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);
		RS_CameraBuffer = TParamBuffer::New(RS_CameraParams);
		bInitialized = true;
	}

	void TWorldRenderer::TStaticState::Clear()
	{
		if (!bInitialized)
		{
			return;
		}

		RS_CameraBuffer = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	TWorldRenderer::~TWorldRenderer()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::Render(TViewport2D vpSize)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		m_CurrentViewport = vpSize;

		// Upload material data to the GPU
		TRef<GraphicsCommandListImmediate> pImmCtx = m_LLR->GetRenderDevice()->GetImmediateCommandList();

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
	TRef<TWorldRenderer> TWorldRenderer::New(TRef<TGameWorld> pWorld)
	{
		KEPLER_PROFILE_SCOPE();
		return MakeRef(ke::New<TWorldRenderer>(pWorld));
	}

	void TWorldRenderer::ClearStaticState()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void TWorldRenderer::RT_UpdateMaterialComponents(TRef<GraphicsCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("RT_UpdateMaterialComponents");
		auto camera = m_CurrentWorld->GetMainCamera();

		if (m_CurrentWorld->IsValidEntity(camera) && m_CurrentWorld->IsCamera(camera))
		{
			auto& cameraComp = m_CurrentWorld->GetComponent<CameraComponent>(camera);
			if (TTargetRegistry::Get()->RenderTargetGroupExists(cameraComp.GetRenderTargetName()))
			{
				if (auto pTarget = TTargetRegistry::Get()->GetRenderTargetGroup(cameraComp.GetRenderTargetName()))
				{
					auto& mathCamera = m_CurrentWorld->GetComponent<CameraComponent>(camera).GetCamera();
					mathCamera.SetFrustumWidth(pTarget->GetRenderTargetAtArrayLayer(0)->GetWidth());
					mathCamera.SetFrustumHeight(pTarget->GetRenderTargetAtArrayLayer(0)->GetHeight());

					// Write it here, though there may be a better place for it
					const auto viewProj = glm::transpose(mathCamera.GenerateViewProjectionMatrix());
					StaticState.RS_CameraBuffer->Write("ViewProjection", &viewProj);
				}
				else
				{
					KEPLER_WARNING(LogWorldRenderer, "Requested render target '{}' for camera does not exist", cameraComp.GetRenderTargetName());
				}
			}
		}

		m_CurrentWorld->GetComponentView<TMaterialComponent>().each(
			[this, pImmCtx](auto, TMaterialComponent& component)
			{
				component.GetMaterial()->RT_Update(pImmCtx);
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
	void TWorldRenderer::MeshPass(TRef<GraphicsCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("MeshPass");
		const u32 frameIdx = m_LLR->GetFrameIndex();

		// Note: Now it is a simple forward renderer, but it needs to become deferred...
		// Configure mesh pass render target
		auto renderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"MeshPassTarget",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			TLowLevelRenderer::m_SwapChainFrameCount);
		TRef<RenderTarget2D> pCurTarget = renderTargetGroup->GetRenderTargetAtArrayLayer(frameIdx);

		auto pDepthTarget = TTargetRegistry::Get()->GetDepthTarget("MeshPassDepth",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::D24_UNORM_S8_UINT,
			false);

		// Configure render target which will contain entity ids
		auto pIdTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"IdTarget",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R32_SINT,
			1,
			true);
		auto pIdTarget = pIdTargetGroup->GetRenderTargetAtArrayLayer(0);
		
		// Bind static camera
		StaticState.RS_CameraBuffer->RT_UploadToGPU(pImmCtx);
		pImmCtx->BindParamBuffers(StaticState.RS_CameraBuffer, RS_Camera);

		// Start drawing other state
		pImmCtx->StartDrawingToRenderTargets({ pCurTarget, pIdTarget }, pDepthTarget);
		pImmCtx->ClearRenderTarget(pCurTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->ClearRenderTarget(pIdTarget, float4(-1.0f));
		pImmCtx->ClearDepthTarget(pDepthTarget, true);
		pImmCtx->SetViewport(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height);

		// For now just draw all the meshes with no pipeline sorting
		m_CurrentWorld->GetComponentView<TMaterialComponent, TStaticMeshComponent>().each(
			[pImmCtx](auto, TMaterialComponent& MT, TStaticMeshComponent& SM)
			{
				pImmCtx->BindParamBuffers(MT.GetMaterial()->GetParamBuffer(), RS_User);
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
	void TWorldRenderer::FlushPass(TRef<GraphicsCommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		// For now just flush the mesh pass image
		pImmCtx->BeginDebugEvent("Screen Quad Pass");
		// Main swap chain
#ifdef ENABLE_EDITOR
		auto pTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup(
			"EditorViewport",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			TLowLevelRenderer::m_SwapChainFrameCount);

		const u32 frameIndex = m_LLR->GetFrameIndex();
		TRef<RenderTarget2D> pCurRenderTarget = pTargetGroup->GetRenderTargetAtArrayLayer(frameIndex);
		pImmCtx->StartDrawingToRenderTargets(pCurRenderTarget, nullptr);
		pImmCtx->ClearRenderTarget(pCurRenderTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->SetViewport(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height);
#else
		auto SwapChain = m_LLR->GetSwapChain(0);
		pImmCtx->StartDrawingToSwapChainImage(SwapChain);
		pImmCtx->ClearSwapChainImage(SwapChain, { 0.1f, 0.1f, 0.1f, 1.0f });
#endif
		pImmCtx->BindVertexBuffers(m_LLR->m_ScreenQuad.VertexBuffer, 0, 0);
		pImmCtx->BindIndexBuffer(m_LLR->m_ScreenQuad.IndexBuffer, 0);
		pImmCtx->BindPipeline(m_LLR->m_ScreenQuad.Pipeline);

		//Write quad sampler
		auto pTarget = TTargetRegistry::Get()->GetRenderTargetGroup("MeshPassTarget");
		CHECK(pTarget);
		auto pSampler = pTarget->GetTextureSamplerAtArrayLayer(m_LLR->GetFrameIndex());

		m_LLR->m_ScreenQuad.Samplers->Write("RenderTarget", pSampler);
		// and
		pImmCtx->BindSamplers(m_LLR->m_ScreenQuad.Samplers);
		pImmCtx->DrawIndexed(m_LLR->m_ScreenQuad.IndexBuffer->GetCount(), 0, 0);
		pImmCtx->EndDebugEvent();
	}

	TWorldRenderer::TStaticState TWorldRenderer::StaticState;
}