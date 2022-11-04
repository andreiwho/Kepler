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
		virtual RefPtr<ISwapChain> CreateSwapChainForWindow(class TWindow* pWindow) = 0;

		inline RefPtr<ICommandListImmediate> GetImmediateCommandList() const { return m_ImmediateCommandList; }
		virtual RefPtr<IVertexBuffer> CreateVertexBuffer(EBufferAccessFlags access, RefPtr<IAsyncDataBlob> pData) = 0;
		virtual RefPtr<IVertexBufferDynamic> CreateDynamicVertexBuffer(EBufferAccessFlags access, usize size, usize stride) = 0;
		virtual RefPtr<IIndexBuffer> CreateIndexBuffer(EBufferAccessFlags access, RefPtr<IAsyncDataBlob> pData) = 0;
		virtual RefPtr<IIndexBufferDynamic> CreateDynamicIndexBuffer(EBufferAccessFlags access, usize size, usize stride) = 0;
		virtual RefPtr<IParamBuffer> CreateParamBuffer(RefPtr<PipelineParamMapping> params) = 0;
		virtual RefPtr<ITransferBuffer> CreateTransferBuffer(usize size, RefPtr<IAsyncDataBlob> pInitialData) = 0;
		virtual RefPtr<IImage2D> CreateImage2D(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mips = 1, u32 layers = 1) = 0;
		virtual RefPtr<IRenderTarget2D> CreateRenderTarget2D(RefPtr<IImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual RefPtr<IDepthStencilTarget2D> CreateDepthStencilTarget2D(RefPtr<IImage2D> pImage, u32 mip = 0, u32 layer = 0, bool bReadOnly = false) = 0;
		virtual RefPtr<ITextureSampler2D> CreateTextureSampler2D(RefPtr<IImage2D> pImage, u32 mip = 0, u32 layer = 0) = 0;
		virtual bool RT_FlushPendingDeleteResources() = 0;


	protected:
		RefPtr<ICommandListImmediate> m_ImmediateCommandList{};
	};
}