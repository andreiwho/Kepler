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

namespace ke
{
	class TRenderDevice : public IntrusiveRefCounted
	{
	public:
		virtual ~TRenderDevice() = default;

		static TRef<TRenderDevice> CreateRenderDevice(ERenderAPI overrideApi = ERenderAPI::Default);
		virtual TRef<TSwapChain> CreateSwapChainForWindow(class TWindow* pWindow) = 0;

		inline TRef<GraphicsCommandListImmediate> GetImmediateCommandList() const { return m_ImmediateCommandList; }
		virtual TRef<TVertexBuffer> CreateVertexBuffer(EBufferAccessFlags access, TRef<AsyncDataBlob> pData) = 0;
		virtual TRef<TIndexBuffer> CreateIndexBuffer(EBufferAccessFlags access, TRef<AsyncDataBlob> pData) = 0;
		virtual TRef<TParamBuffer> CreateParamBuffer(TRef<TPipelineParamMapping> params) = 0;
		virtual TRef<TTransferBuffer> CreateTransferBuffer(usize size, TRef<AsyncDataBlob> pInitialData) = 0;
		virtual TRef<TImage2D> CreateImage2D(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mips = 1, u32 layers = 1) = 0;
		virtual TRef<RenderTarget2D> CreateRenderTarget2D(TRef<TImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual TRef<DepthStencilTarget2D> CreateDepthStencilTarget2D(TRef<TImage2D> pImage, u32 mip = 0, u32 layer = 0, bool bReadOnly = false) = 0;
		virtual TRef<TTextureSampler2D> CreateTextureSampler2D(TRef<TImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual bool RT_FlushPendingDeleteResources() = 0;

	protected:
		TRef<GraphicsCommandListImmediate> m_ImmediateCommandList{};
	};
}