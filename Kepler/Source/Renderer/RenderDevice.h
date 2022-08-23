#pragma once
#include "Core/Types.h"
#include "RenderTypes.h"
#include "Core/Malloc.h"

#include "Elements/SwapChain.h"
#include "Elements/VertexBuffer.h"
#include "Elements/CommandList.h"

#include <memory>
#include "Elements/IndexBuffer.h"

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
		virtual TRef<TParamBuffer> CreateParamBuffer(TRef<TPipelineParamPack> Params) = 0;

		virtual bool RT_FlushPendingDeleteResources() = 0;

	protected:
		TRef<TCommandListImmediate> ImmediateCommandList{};
	};
}