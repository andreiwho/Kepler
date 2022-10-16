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

		static RefPtr<TRenderDevice> CreateRenderDevice(ERenderAPI overrideApi = ERenderAPI::Default);
		virtual RefPtr<TSwapChain> CreateSwapChainForWindow(class TWindow* pWindow) = 0;

		inline RefPtr<GraphicsCommandListImmediate> GetImmediateCommandList() const { return m_ImmediateCommandList; }
		virtual RefPtr<TVertexBuffer> CreateVertexBuffer(EBufferAccessFlags access, RefPtr<AsyncDataBlob> pData) = 0;
		virtual RefPtr<DynamicVertexBuffer> CreateDynamicVertexBuffer(EBufferAccessFlags access, usize size, usize stride) = 0;
		virtual RefPtr<TIndexBuffer> CreateIndexBuffer(EBufferAccessFlags access, RefPtr<AsyncDataBlob> pData) = 0;
		virtual RefPtr<DynamicIndexBuffer> CreateDynamicIndexBuffer(EBufferAccessFlags access, usize size, usize stride) = 0;
		virtual RefPtr<TParamBuffer> CreateParamBuffer(RefPtr<TPipelineParamMapping> params) = 0;
		virtual RefPtr<TTransferBuffer> CreateTransferBuffer(usize size, RefPtr<AsyncDataBlob> pInitialData) = 0;
		virtual RefPtr<TImage2D> CreateImage2D(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mips = 1, u32 layers = 1) = 0;
		virtual RefPtr<RenderTarget2D> CreateRenderTarget2D(RefPtr<TImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual RefPtr<DepthStencilTarget2D> CreateDepthStencilTarget2D(RefPtr<TImage2D> pImage, u32 mip = 0, u32 layer = 0, bool bReadOnly = false) = 0;
		virtual RefPtr<TTextureSampler2D> CreateTextureSampler2D(RefPtr<TImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual bool RT_FlushPendingDeleteResources() = 0;


	protected:
		RefPtr<GraphicsCommandListImmediate> m_ImmediateCommandList{};
	};
}