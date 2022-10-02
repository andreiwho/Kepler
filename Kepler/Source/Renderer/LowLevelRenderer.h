#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"
#include "TargetRegistry.h"

#include <vector>
#include "Pipelines/GraphicsPipeline.h"
#include "World/StaticMesh.h"

namespace ke
{
	class TLowLevelRenderer : public TRefCounted
	{
		static TLowLevelRenderer* Instance;

	public:
		TLowLevelRenderer();
		~TLowLevelRenderer();

		void InitRenderStateForWindow(class TWindow* pWindow);
		void PresentAll();
		void DestroyRenderStateForWindow(class TWindow* pWindow);
		void OnWindowResized(class TWindow* pWindow);

		inline u8 GetFrameIndex() const
		{
			return m_SwapChainFrame.load(std::memory_order_relaxed);
		}

		static TLowLevelRenderer* Get() { return Instance; }
	public:
		inline TRef<TRenderDevice> GetRenderDevice() const { return m_RenderDevice; }

		inline TRef<TSwapChain> GetSwapChain(u32 idx) const
		{
			if (m_SwapChains.GetLength() > idx)
			{
				return m_SwapChains[idx];
			}
			return nullptr;
		}

		static constexpr u32 m_SwapChainFrameCount = 3;
		static constexpr u32 m_FramesInFlight = 3;

		void InitScreenQuad();

		// Screen quad
		struct TScreenQuad
		{
			TRef<TGraphicsPipeline> Pipeline{};
			TRef<TVertexBuffer> VertexBuffer{};
			TRef<TIndexBuffer> IndexBuffer{};
			TRef<TPipelineSamplerPack> Samplers{};
		} m_ScreenQuad{};

	private:
		TRef<TSwapChain> FindAssociatedSwapChain(class TWindow* pWindow) const;

	private:
		TRenderThread m_RenderThread{};
		TSharedPtr<TShaderCache> m_ShaderCache{};
		TSharedPtr<TGraphicsPipelineCache> m_PipelineCache{};
		TSharedPtr<TTargetRegistry> m_TargetRegistry{};

		TRef<TRenderDevice> m_RenderDevice{};
		TDynArray<TRef<TSwapChain>> m_SwapChains;
		TAtomic<u64> m_FrameCounter = 0;
		TAtomic<u8> m_SwapChainFrame = 0;
		const u64 m_FlushPendingDeleteResourcesInterval = 16;
	};
}