#pragma once
#include "Core/Core.h"
#include "Core/Types.h"
#include "Renderer/Elements/CommandList.h"
#include "World/Game/GameWorld.h"
#include "../LowLevelRenderer.h"
#include "../Subrenderer/Subrenderer.h"
#include "WorldRenderer.gen.h"

namespace ke
{
	struct TViewport2D
	{
		u32 X = 0, Y = 0;
		u32 Width = 0, Height = 0;
	};

	enum class ESubrendererOrder
	{
		Background,
		Overlay
	};

	reflected enum class EReservedSlots
	{
		RS_Renderer = 0,
		RS_Camera = 1,
		RS_Light = 2,
		RS_User = 3,
	};

	reflected class WorldRenderer : public IntrusiveRefCounted
	{
		reflected_body();
		static WorldRenderer* Instance;
	public:
		static WorldRenderer* Get() { return Instance; }

		reflected EFormat MeshPassBufferFormat = EFormat::R11G11B10_FLOAT;
		reflected EFormat PrePassDepthBufferFormat = EFormat::D24_UNORM_S8_UINT;
		
		reflected kmeta(editspeed=0.01f, clampmin = 0.1f) 
		float Gamma = 1.0f;
		
		reflected kmeta(editspeed=0.01f, clampmin = 0.1f) 
		float Exposure = 1.0f;

		WorldRenderer();
		~WorldRenderer();
		
		void InitFrame(RefPtr<GameWorld> pWorld);
		void Render(TViewport2D ViewportSize);
		void UpdateRendererMainThread(float deltaTime);
		void UpdateLightingData_MainThread();
		void UpdateRendererData_MainThread();

		static RefPtr<WorldRenderer> New();

		template<typename T, ESubrendererOrder TOrder, typename ... Args>
		inline SharedPtr<T> PushSubrenderer(Args&&... args)
		{
			static_assert(std::is_base_of_v<ISubrenderer, T>);
			auto pSubrenderer = Await(TRenderThread::Submit([&] { return MakeShared<T>(std::forward<Args>(args)...); }));

			switch (TOrder)
			{
			case ESubrendererOrder::Background:
				m_BackgroundSubrenderers.AppendBack(pSubrenderer);
				break;
			case ESubrendererOrder::Overlay:
				m_OverlaySubrenderers.AppendBack(pSubrenderer);
				break;
			default:
				CRASH();
				break;
			}
			return pSubrenderer;
		}

		template<ESubrendererOrder TOrder>
		void RenderSubrenderers(RefPtr<ICommandListImmediate> pImmCtx)
		{
			for (auto& pSr : GetSubrenderers<TOrder>())
			{
				pSr->Render(pImmCtx);
			}
		}

		template<ESubrendererOrder TOrder>
		Array<SharedPtr<ISubrenderer>>& GetSubrenderers()
		{
			if constexpr (TOrder == ESubrendererOrder::Overlay)
			{
				return m_OverlaySubrenderers;
			}
			return m_BackgroundSubrenderers;
		}

		void UpdateSubrenderersMainThread(float deltaTime)
		{
			for (auto& pSr : GetSubrenderers<ESubrendererOrder::Background>()) { pSr->UpdateRendererMainThread(deltaTime); }
			for (auto& pSr : GetSubrenderers<ESubrendererOrder::Overlay>()) { pSr->UpdateRendererMainThread(deltaTime); }
		}

		void ClearSubrenderersState()
		{
			for (auto& pSr : GetSubrenderers<ESubrendererOrder::Background>()) { pSr->ClearState(); }
			for (auto& pSr : GetSubrenderers<ESubrendererOrder::Overlay>()) { pSr->ClearState(); }
		}


	private:
		void RT_UpdateMaterialComponents(RefPtr<ICommandListImmediate> pImmCtx);
		void CollectRenderableViews();
		void PrePass(RefPtr<ICommandListImmediate> pImmCtx);
		void MeshPass(RefPtr<ICommandListImmediate> pImmCtx);
		void TonemappingAndFlushPass(RefPtr<ICommandListImmediate> pImmCtx);

	private:
		TViewport2D m_CurrentViewport{};
		RefPtr<GameWorld> m_CurrentWorld;
		LowLevelRenderer* m_LLR;
		Array<SharedPtr<ISubrenderer>> m_BackgroundSubrenderers;
		Array<SharedPtr<ISubrenderer>> m_OverlaySubrenderers;

		bool bInitialized = false;
		RefPtr<IParamBuffer> RS_CameraBuffer;
		RefPtr<IParamBuffer> RS_LightBuffer;
		RefPtr<IParamBuffer> RS_RendererSetupBuffer;

		RefPtr<IGraphicsPipeline> PrePassPipeline;

		struct RS_RendererSetupStruct
		{
			float Gamma{2.2f};
			float Exposure{ 1.0f };
		};

		struct RS_CameraBufferStruct
		{
			matrix4x4 ViewProjection;
		};

		struct RS_LightBufferStruct
		{
			float4 Ambient;
			float4 DirectionalLightDirection;
			float4 DirectionalLightColor;
			float DirectionalLightIntensity;
		};
	};
}