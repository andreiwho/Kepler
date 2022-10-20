#include "WorldRenderer.h"
#include "../RenderGlobals.h"
#include "World/Game/Components/MaterialComponent.h"
#include "Renderer/TargetRegistry.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Components/TransformComponent.h"
#include "../HLSLShaderCompiler.h"
#include "../Pipelines/GraphicsPipeline.h"
#include "World/Game/Components/Light/AmbientLightComponent.h"
#include "World/Game/Components/Light/DirectionalLightComponent.h"
#include "../Subrenderer/Subrenderer2D.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogWorldRenderer, All);

	//////////////////////////////////////////////////////////////////////////
	WorldRenderer::WorldRenderer()
		: m_LLR(LowLevelRenderer::Get())
	{
		if (!bInitialized)
		{
			CHECK(IsRenderThread());
			RefPtr<PipelineParamMapping> RS_CameraParams = PipelineParamMapping::New();
			RS_CameraParams->AddParam("ViewProjection", OFFSET_PARAM_ARGS(RS_CameraBufferStruct, ViewProjection), EShaderStageFlags::Vertex, EShaderInputType::Matrix4x4);
			RS_CameraBuffer = IParamBuffer::New(RS_CameraParams);

			RefPtr<PipelineParamMapping> RS_LightParams = PipelineParamMapping::New();
			RS_LightParams->AddParam("Ambient", OFFSET_PARAM_ARGS(RS_LightBufferStruct, Ambient), EShaderStageFlags::Pixel, EShaderInputType::Float4);
			RS_LightParams->AddParam("DirectionalLightDirection", OFFSET_PARAM_ARGS(RS_LightBufferStruct, DirectionalLightDirection), EShaderStageFlags::Vertex, EShaderInputType::Float4);
			RS_LightParams->AddParam("DirectionalLightColor", OFFSET_PARAM_ARGS(RS_LightBufferStruct, DirectionalLightColor), EShaderStageFlags::Pixel, EShaderInputType::Float4);
			RS_LightParams->AddParam("DirectionalLightIntensity", OFFSET_PARAM_ARGS(RS_LightBufferStruct, DirectionalLightIntensity), EShaderStageFlags::Pixel, EShaderInputType::Float);
			RS_LightBuffer = IParamBuffer::New(RS_LightParams);

			// Setup pipeline
			auto prePassShader = THLSLShaderCompiler::CreateShaderCompiler()
				->CompileShader("EngineShaders://DefaultPrePass.hlsl", EShaderStageFlags::Vertex);

			GraphicsPipelineConfig config{};
			config.VertexInput.Topology = EPrimitiveTopology::TriangleList;
			config.VertexInput.VertexLayout = prePassShader->GetReflection()->VertexLayout;
			config.ParamMapping = prePassShader->GetReflection()->ParamMapping;
			config.DepthStencil.bDepthEnable = true;
			config.DepthStencil.DepthAccess = EDepthBufferAccess::Write;
			config.Rasterizer.bRasterDisabled = true;

			PrePassPipeline = MakeRef(ke::New<IGraphicsPipeline>(prePassShader, config));
			bInitialized = true;
		}

	}

	//////////////////////////////////////////////////////////////////////////
	WorldRenderer::~WorldRenderer()
	{
		if (!bInitialized)
		{
			return;
		}
	}

	void WorldRenderer::InitFrame(RefPtr<GameWorld> pWorld)
	{
		m_CurrentWorld = pWorld;
	}

	//////////////////////////////////////////////////////////////////////////
	void WorldRenderer::Render(TViewport2D vpSize)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		m_CurrentViewport = vpSize;

		// Upload material data to the GPU
		RefPtr<ICommandListImmediate> pImmCtx = m_LLR->GetRenderDevice()->GetImmediateCommandList();

		RT_UpdateMaterialComponents(pImmCtx);

		// Prepare for rendering
		pImmCtx->BeginDebugEvent("ClearRenderState");
		{
			pImmCtx->ClearSamplers();
		}
		pImmCtx->EndDebugEvent();

		pImmCtx->SetViewport(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height, 0.0f, 1.0f);
		pImmCtx->SetScissor(0, 0, (float)m_CurrentViewport.Width, (float)m_CurrentViewport.Height);

		// Bind static camera
		RS_CameraBuffer->RT_UploadToGPU(pImmCtx);
		pImmCtx->BindParamBuffers(RS_CameraBuffer, RS_Camera);

		RS_LightBuffer->RT_UploadToGPU(pImmCtx);
		pImmCtx->BindParamBuffers(RS_LightBuffer, RS_Light);

		// Collect renderable objects
		// ...
		PrePass(pImmCtx);

		// Render background subrenderers
		RenderSubrenderers<ESubrendererOrder::Background>(pImmCtx);
		// Draw meshes
		MeshPass(pImmCtx);
		// Render overlay subrenderers
		RenderSubrenderers<ESubrendererOrder::Overlay>(pImmCtx);

		// Tonemapping
		FlushPass(pImmCtx);
	}

	void WorldRenderer::UpdateRendererMainThread(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		UpdateLightingData_MainThread();

		auto camera = m_CurrentWorld->GetMainCamera();
		if (m_CurrentWorld->IsValidEntity(camera) && m_CurrentWorld->IsCamera(camera))
		{
			auto& cameraComp = m_CurrentWorld->GetComponent<CameraComponent>(camera);
			if (RenderTargetRegistry::Get()->RenderTargetGroupExists(cameraComp.GetRenderTargetName()))
			{
				if (auto pTarget = RenderTargetRegistry::Get()->GetRenderTargetGroup(cameraComp.GetRenderTargetName()))
				{
					auto& mathCamera = m_CurrentWorld->GetComponent<CameraComponent>(camera).GetCamera();
					mathCamera.SetFrustumWidth(pTarget->GetRenderTargetAtArrayLayer(0)->GetWidth());
					mathCamera.SetFrustumHeight(pTarget->GetRenderTargetAtArrayLayer(0)->GetHeight());

					// Write it here, though there may be a better place for it
					const auto viewProj = glm::transpose(mathCamera.GenerateViewProjectionMatrix());
					RS_CameraBuffer->Write("ViewProjection", &viewProj);
				}
				else
				{
					KEPLER_WARNING(LogWorldRenderer, "Requested render target '{}' for camera does not exist", cameraComp.GetRenderTargetName());
				}
			}
		}
		UpdateSubrenderersMainThread(deltaTime);
	}

	void WorldRenderer::UpdateLightingData_MainThread()
	{
		KEPLER_PROFILE_SCOPE();
		float3 Ambient{ 0.0f, 0.0f, 0.0f };
		m_CurrentWorld->GetComponentView<AmbientLightComponent>().each(
			[&](auto, AmbientLightComponent& AL)
			{
				Ambient += AL.GetColor();
			});

		RS_LightBuffer->Write("Ambient", &Ambient);

		m_CurrentWorld->GetComponentView<DirectionalLightComponent, TTransformComponent>().each(
			[&, this](auto, DirectionalLightComponent& DLC, TTransformComponent& TC)
			{
				auto dir = TC.GetTransform().RotationToEuler();
				auto color = DLC.GetColor();
				auto intensity = DLC.GetIntensity();
				RS_LightBuffer->Write("DirectionalLightDirection", &dir);
				RS_LightBuffer->Write("DirectionalLightColor", &color);
				RS_LightBuffer->Write("DirectionalLightIntensity", &intensity);
			});
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<WorldRenderer> WorldRenderer::New()
	{
		KEPLER_PROFILE_SCOPE();
		return MakeRef(ke::New<WorldRenderer>());
	}

	//////////////////////////////////////////////////////////////////////////
	void WorldRenderer::RT_UpdateMaterialComponents(RefPtr<ICommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("RT_UpdateMaterialComponents");

		m_CurrentWorld->GetComponentView<TMaterialComponent>().each(
			[this, pImmCtx](auto, TMaterialComponent& component)
			{
				component.GetMaterial()->RT_Update(pImmCtx);
			});
		pImmCtx->EndDebugEvent();
	}

	//////////////////////////////////////////////////////////////////////////
	void WorldRenderer::CollectRenderableViews()
	{
		KEPLER_PROFILE_SCOPE();
	}

	void WorldRenderer::PrePass(RefPtr<ICommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("PrePass");

		auto pDepthTarget = RenderTargetRegistry::Get()->GetDepthTarget("PrePassDepth",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::D24_UNORM_S8_UINT,
			false);

		pImmCtx->StartDrawingToRenderTargets(nullptr, pDepthTarget);
		pImmCtx->ClearDepthTarget(pDepthTarget, true);

		pImmCtx->BindPipeline(PrePassPipeline);
		m_CurrentWorld->GetComponentView<TMaterialComponent, TStaticMeshComponent>().each(
			[pImmCtx](auto, TMaterialComponent& MT, TStaticMeshComponent& SM)
			{
				if (MT.UsesPrepass())
				{
					pImmCtx->BindParamBuffers(MT.GetMaterial()->GetParamBuffer(), RS_User);
					for (const auto& Section : SM.GetStaticMesh()->GetSections())
					{
						pImmCtx->BindVertexBuffers(Section.VertexBuffer, 0, 0);
						pImmCtx->BindIndexBuffer(Section.IndexBuffer, 0);
						pImmCtx->DrawIndexed(Section.IndexBuffer->GetCount(), 0, 0);
					}
				}
			}
		);
		pImmCtx->EndDebugEvent();
	}

	namespace
	{
		struct TDrawCall
		{
			RefPtr<IParamBuffer> ParamBuffer;
			RefPtr<IGraphicsPipeline> Pipeline;
			RefPtr<PipelineSamplerPack> Samplers;
			RefPtr<TStaticMesh> StaticMesh;
		};
	}

	//////////////////////////////////////////////////////////////////////////
	void WorldRenderer::MeshPass(RefPtr<ICommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCtx->BeginDebugEvent("MeshPass");
		const u32 frameIdx = m_LLR->GetFrameIndex();

		// Note: Now it is a simple forward renderer, but it needs to become deferred...
		// Configure mesh pass render target
		auto renderTargetGroup = RenderTargetRegistry::Get()->GetRenderTargetGroup(
			"MeshPassTarget",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			LowLevelRenderer::m_SwapChainFrameCount);
		RefPtr<IRenderTarget2D> pCurTarget = renderTargetGroup->GetRenderTargetAtArrayLayer(frameIdx);

		auto pDepthTarget = RenderTargetRegistry::Get()->GetDepthTarget("PrePassDepth");

		// Configure render target which will contain entity ids
		auto pIdTargetGroup = RenderTargetRegistry::Get()->GetRenderTargetGroup(
			"IdTarget",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R32_SINT,
			1,
			true);
		auto pIdTarget = pIdTargetGroup->GetRenderTargetAtArrayLayer(0);

		// Start drawing other state
		pImmCtx->StartDrawingToRenderTargets({ pCurTarget, pIdTarget }, pDepthTarget);
		pImmCtx->ClearRenderTarget(pCurTarget, float4(0.1f, 0.1f, 0.1f, 1.0f));
		pImmCtx->ClearRenderTarget(pIdTarget, float4(-1.0f));

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
	void WorldRenderer::FlushPass(RefPtr<ICommandListImmediate> pImmCtx)
	{
		KEPLER_PROFILE_SCOPE();
		// For now just flush the mesh pass image
		pImmCtx->BeginDebugEvent("Screen Quad Pass");
		// Main swap chain
#ifdef ENABLE_EDITOR
		auto pTargetGroup = RenderTargetRegistry::Get()->GetRenderTargetGroup(
			"EditorViewport",
			m_CurrentViewport.Width,
			m_CurrentViewport.Height,
			EFormat::R8G8B8A8_UNORM,
			LowLevelRenderer::m_SwapChainFrameCount);

		const u32 frameIndex = m_LLR->GetFrameIndex();
		RefPtr<IRenderTarget2D> pCurRenderTarget = pTargetGroup->GetRenderTargetAtArrayLayer(frameIndex);
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
		auto pTarget = RenderTargetRegistry::Get()->GetRenderTargetGroup("MeshPassTarget");
		CHECK(pTarget);
		auto pSampler = pTarget->GetTextureSamplerAtArrayLayer(m_LLR->GetFrameIndex());

		m_LLR->m_ScreenQuad.Samplers->Write("RenderTarget", pSampler);
		// and
		pImmCtx->BindSamplers(m_LLR->m_ScreenQuad.Samplers);
		pImmCtx->DrawIndexed(m_LLR->m_ScreenQuad.IndexBuffer->GetCount(), 0, 0);
		pImmCtx->EndDebugEvent();
	}
}