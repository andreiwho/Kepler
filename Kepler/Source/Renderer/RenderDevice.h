#pragma once
#include "Core/Types.h"
#include "RenderTypes.h"
#include "Core/Malloc.h"

#include "Elements/SwapChain.h"
#include "Elements/VertexBuffer.h"
#include "Elements/CommandList.h"

#include <memory>
#include "Elements/IndexBuffer.h"
#include "Elements/Image.h"
#include "Elements/RenderTarget.h"
#include "Elements/Texture.h"

namespace Kepler
{
	class TRenderDevice : public TRefCounted
	{
	public:
		virtual ~TRenderDevice() = default;

		static TRef<TRenderDevice> CreateRenderDevice(ERenderAPI OverrideAPI = ERenderAPI::Default);
		virtual TRef<TSwapChain> CreateSwapChainForWindow(class TWindow* Window) = 0;

		inline TRef<TCommandListImmediate> GetImmediateCommandList() const { return ImmediateCommandList; }
		virtual TRef<TVertexBuffer> CreateVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data) = 0;
		virtual TRef<TIndexBuffer> CreateIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data) = 0;
		virtual TRef<TParamBuffer> CreateParamBuffer(TRef<TPipelineParamMapping> Params) = 0;
		virtual TRef<TTransferBuffer> CreateTransferBuffer(usize Size, TRef<TDataBlob> InitialData) = 0;
		virtual TRef<TImage1D> CreateImage1D(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 MipLevels = 1, u32 InArraySize = 1) = 0;
		virtual TRef<TImage2D> CreateImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 MipLevels = 1, u32 InArraySize = 1) = 0;
		virtual TRef<TImage3D> CreateImage3D(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 MipLevels = 1, u32 InArraySize = 1) = 0;
		virtual TRef<TRenderTarget2D> CreateRenderTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0) = 0;
		virtual TRef<TDepthStencilTarget2D> CreateDepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0) = 0;
		virtual TRef<TTextureSampler2D> CreateTextureSampler2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0) = 0;
		virtual bool RT_FlushPendingDeleteResources() = 0;

	protected:
		TRef<TCommandListImmediate> ImmediateCommandList{};
	};
}