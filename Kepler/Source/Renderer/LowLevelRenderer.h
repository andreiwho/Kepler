#pragma once
#include "RenderThread.h"
#include "RenderDevice.h"
#include "Elements/SwapChain.h"
#include "TargetRegistry.h"

#include <vector>
#include "Pipelines/GraphicsPipeline.h"
#include "World/StaticMesh.h"

namespace Kepler
{
	class TLowLevelRenderer : public TRefCounted
	{
		static TLowLevelRenderer* Instance;

	public:
		TLowLevelRenderer();
		~TLowLevelRenderer();

		void InitRenderStateForWindow(class TWindow* InWindow);
		void PresentAll();
		void DestroyRenderStateForWindow(class TWindow* InWindow);
		void OnWindowResized(class TWindow* InWindow);

		inline u8 GetFrameIndex() const
		{
			return SwapChainFrame.load(std::memory_order_relaxed);
		}

		static TLowLevelRenderer* Get() { return Instance; }
	public:
		inline TRef<TRenderDevice> GetRenderDevice() const { return RenderDevice; }

		inline TRef<TSwapChain> GetSwapChain(u32 Index) const
		{
			if (SwapChains.GetLength() > Index)
			{
				return SwapChains[Index];
			}
			return nullptr;
		}

		static constexpr u32 SwapChainFrameCount = 3;
		static constexpr u32 FramesInFlight = 3;

		void InitScreenQuad();

		// Screen quad
		struct TScreenQuad
		{
			TRef<TGraphicsPipeline> Pipeline{};
			TRef<TVertexBuffer> VertexBuffer{};
			TRef<TIndexBuffer> IndexBuffer{};
			TRef<TPipelineSamplerPack> Samplers{};
		} ScreenQuad{};

	private:
		TRef<TSwapChain> FindAssociatedSwapChain(class TWindow* InWindow) const;

	private:
		TRenderThread RenderThread{};
		TSharedPtr<TShaderCache> ShaderCache{};
		TSharedPtr<TGraphicsPipelineCache> PipelineCache{};
		TSharedPtr<TTargetRegistry> TargetRegistry{};

		TRef<TRenderDevice> RenderDevice{};
		TDynArray<TRef<TSwapChain>> SwapChains;
		TAtomic<u64> FrameCounter = 0;
		TAtomic<u8> SwapChainFrame = 0;
		const u64 FlushPendingDeleteResourcesInterval = 16;
	};
}