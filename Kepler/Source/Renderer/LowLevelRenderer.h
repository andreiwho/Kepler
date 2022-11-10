#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"
#include "TargetRegistry.h"

#include <vector>
#include "Pipelines/GraphicsPipeline.h"
#include "World/StaticMesh.h"
#include "Subrenderer/Subrenderer.h"
#include "LowLevelRenderer.gen.h"

namespace ke
{
	reflected class LowLevelRenderer : public IntrusiveRefCounted
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

		reflected bool bEnableVSync = false;

	public:
		inline RefPtr<TRenderDevice> GetRenderDevice() const { return m_RenderDevice; }

		inline RefPtr<ISwapChain> GetSwapChain(u32 idx) const
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
			RefPtr<IGraphicsPipeline> Pipeline{};
			RefPtr<IVertexBuffer> VertexBuffer{};
			RefPtr<IIndexBuffer> IndexBuffer{};
			RefPtr<PipelineSamplerPack> Samplers{};
		} m_ScreenQuad{};

	private:
		RefPtr<ISwapChain> FindAssociatedSwapChain(class TWindow* pWindow) const;

	private:
		TRenderThread m_RenderThread{};
		SharedPtr<TShaderCache> m_ShaderCache{};
		SharedPtr<GraphicsPipelineCache> m_PipelineCache{};
		SharedPtr<RenderTargetRegistry> m_TargetRegistry{};

		RefPtr<TRenderDevice> m_RenderDevice{};
		Array<RefPtr<ISwapChain>> m_SwapChains;
		TAtomic<u64> m_FrameCounter = 0;
		TAtomic<u8> m_SwapChainFrame = 0;
		u8 m_NextFrameIndex = 1;
		const u64 m_FlushPendingDeleteResourcesInterval = 16;
	};
}