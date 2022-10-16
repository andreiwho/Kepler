#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"
#include "TargetRegistry.h"

#include <vector>
#include "Pipelines/GraphicsPipeline.h"
#include "World/StaticMesh.h"
#include "Subrenderer/Subrenderer.h"

namespace ke
{
	enum class ESubrendererOrder
	{
		Background,
		Overlay
	};

	class LowLevelRenderer : public IntrusiveRefCounted
	{
		static LowLevelRenderer* Instance;

	public:
		LowLevelRenderer();
		~LowLevelRenderer();

		void InitRenderStateForWindow(class TWindow* pWindow);
		void PresentAll();
		void DestroyRenderStateForWindow(class TWindow* pWindow);
		void OnWindowResized(class TWindow* pWindow);

		inline u8 GetFrameIndex() const
		{
			return m_SwapChainFrame.load(std::memory_order_relaxed);
		}

		inline u8 GetNextFrameIndex() const
		{
			return m_NextFrameIndex;
		}

		static LowLevelRenderer* Get() { return Instance; }
	public:
		inline RefPtr<TRenderDevice> GetRenderDevice() const { return m_RenderDevice; }

		inline RefPtr<TSwapChain> GetSwapChain(u32 idx) const
		{
			if (m_SwapChains.GetLength() > idx)
			{
				return m_SwapChains[idx];
			}
			return nullptr;
		}

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
		Array<SharedPtr<ISubrenderer>>& GetSubrenderers()
		{
			if constexpr (TOrder == ESubrendererOrder::Overlay)
			{
				return m_OverlaySubrenderers;
			}
			return m_BackgroundSubrenderers;
		}

		static constexpr u32 m_SwapChainFrameCount = 3;
		static constexpr u32 m_FramesInFlight = 3;

		void InitScreenQuad();

		template<ESubrendererOrder TOrder>
		void RenderSubrenderers(RefPtr<GraphicsCommandListImmediate> pImmCtx)
		{
			for (auto& pSr : GetSubrenderers<TOrder>())
			{
				pSr->Render(pImmCtx);
			}
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

		// Screen quad
		struct TScreenQuad
		{
			RefPtr<TGraphicsPipeline> Pipeline{};
			RefPtr<TVertexBuffer> VertexBuffer{};
			RefPtr<TIndexBuffer> IndexBuffer{};
			RefPtr<TPipelineSamplerPack> Samplers{};
		} m_ScreenQuad{};

	private:
		RefPtr<TSwapChain> FindAssociatedSwapChain(class TWindow* pWindow) const;

	private:
		TRenderThread m_RenderThread{};
		SharedPtr<TShaderCache> m_ShaderCache{};
		SharedPtr<TGraphicsPipelineCache> m_PipelineCache{};
		SharedPtr<TTargetRegistry> m_TargetRegistry{};
		Array<SharedPtr<ISubrenderer>> m_BackgroundSubrenderers;
		Array<SharedPtr<ISubrenderer>> m_OverlaySubrenderers;

		RefPtr<TRenderDevice> m_RenderDevice{};
		Array<RefPtr<TSwapChain>> m_SwapChains;
		TAtomic<u64> m_FrameCounter = 0;
		TAtomic<u8> m_SwapChainFrame = 0;
		u8 m_NextFrameIndex = 1;
		const u64 m_FlushPendingDeleteResourcesInterval = 16;
	};
}