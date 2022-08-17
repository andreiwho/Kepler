#include "CommandListImmediateD3D11.h"
#include "SwapChainD3D11.h"
#include "Renderer/RenderGlobals.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "Core/Log.h"

namespace Kepler
{
	TCommandListImmediateD3D11* TCommandListImmediateD3D11::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	TCommandListImmediateD3D11::TCommandListImmediateD3D11(ID3D11DeviceContext4* InContext) : Context(InContext)
	{
		CHECK(!Instance);
		Instance = this;

		CHECK(IsRenderThread());
		Context->AddRef();
	}

	//////////////////////////////////////////////////////////////////////////
	TCommandListImmediateD3D11::~TCommandListImmediateD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
		Context->Release();
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::StartDrawingToSwapChainImage(TSwapChain* SwapChain)
	{
		CHECK(IsRenderThread());
		CHECK(SwapChain && Context);
		TSwapChainD3D11* MySwapChain = static_cast<TSwapChainD3D11*>(SwapChain);
		ID3D11RenderTargetView* ppRTV[] = { CHECKED(MySwapChain->GetRenderTargetView()) };
		Context->OMSetRenderTargets(ARRAYSIZE(ppRTV), ppRTV, nullptr);
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::ClearSwapChainImage(TSwapChain* SwapChain, float ClearColor[4])
	{
		CHECK(IsRenderThread());
		CHECK(SwapChain && Context);
		TSwapChainD3D11* MySwapChain = static_cast<TSwapChainD3D11*>(SwapChain);
		Context->ClearRenderTargetView(MySwapChain->GetRenderTargetView(), ClearColor);
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::Draw(u32 VertexCount, u32 BaseVertexIndex)
	{
		CHECK(IsRenderThread());
		if (VALIDATEDMSG(HasPipelineStateSetup(), "Pipeline state not setup. Cannot draw."))
		{
			Context->Draw(VertexCount, BaseVertexIndex);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset)
	{
		CHECK(IsRenderThread());
		if (TRef<TVertexBufferD3D11> MyBuffer = RefCast<TVertexBufferD3D11>(VertexBuffer))
		{
			if (ID3D11Buffer* Buffer = MyBuffer->GetBuffer())
			{
				UINT Stride = (UINT)MyBuffer->GetStride();
				UINT BindOffset = (UINT)Offset;
				Context->IASetVertexBuffers(StartSlot, 1, &Buffer, &Stride, &BindOffset);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::BindVertexBuffers(const TDynArray<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const TDynArray<u32>& Offsets)
	{
		CHECK(IsRenderThread());
		const bool bOffsetsHasEntries = Offsets.GetLength() > 0;

		if (bOffsetsHasEntries)
		{
			CHECKMSG(VertexBuffers.GetLength() == Offsets.GetLength(),
				"Error in BindVertexBuffers. If you specify at least one offset than the number of offsets in Offsets must match with the number of vertex buffers");
		}

		if (VertexBuffers.GetLength() > 0)
		{
			TDynArray<ID3D11Buffer*> ppBuffers;
			TDynArray<u32> pStrides;
			TDynArray<u32> pOffsets;

			ppBuffers.Reserve(VertexBuffers.GetLength());
			pStrides.Reserve(VertexBuffers.GetLength());
			pOffsets.Reserve(VertexBuffers.GetLength());

			usize Index = 0;
			for (const auto& Buffer : VertexBuffers)
			{
				if (Buffer)
				{
					if (TRef<TVertexBufferD3D11> MyBuffer = RefCast<TVertexBufferD3D11>(Buffer))
					{
						ppBuffers.EmplaceBack(MyBuffer->GetBuffer());
						pStrides.EmplaceBack((u32)MyBuffer->GetStride());
						pOffsets.EmplaceBack(bOffsetsHasEntries ? (u32)Offsets[Index] : (u32)0);
					}
				}
				Index++;
			}

			CHECK((pStrides.GetLength() == ppBuffers.GetLength()) && (pOffsets.GetLength() == ppBuffers.GetLength()));
			Context->IASetVertexBuffers(StartSlot, (UINT)ppBuffers.GetLength(), ppBuffers.GetData(), pStrides.GetData(), pOffsets.GetData());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset)
	{
		CHECK(IsRenderThread());

		// Let us be a little forgiving here
		if (VALIDATED(HasPipelineStateSetup()))
		{
			Context->DrawIndexed(IndexCount, BaseIndexOffset, (INT)BaseVertexOffset);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset)
	{
		CHECK(IsRenderThread());
		if (auto MyBuffer = RefCast<TIndexBufferD3D11>(IndexBuffer))
		{
			if (ID3D11Buffer* pBuffer = MyBuffer->GetBuffer())
			{
				UINT Stride = MyBuffer->GetStride();
				Context->IASetIndexBuffer(pBuffer,
					[Stride]()
					{
						const usize UShort = sizeof(u16);
						const usize UInt = sizeof(u32);
						switch (Stride)
						{
						case UShort:
							return DXGI_FORMAT_R16_UINT;
						case UInt:
							return DXGI_FORMAT_R32_UINT;
						default:
							CHECKMSG(false, "Unknown index format");
						}
						return DXGI_FORMAT_UNKNOWN;
					}(), Offset);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
}