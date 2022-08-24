#include "CommandListImmediateD3D11.h"
#include "SwapChainD3D11.h"
#include "Renderer/RenderGlobals.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "Core/Log.h"
#include "HLSLShaderD3D11.h"
#include "GraphicsPipelineHandleD3D11.h"
#include "ParamBufferD3D11.h"

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
	void TCommandListImmediateD3D11::BindShader(TRef<TShader> Shader)
	{
		CHECK(IsRenderThread());
		if (!Shader)
		{
			return;
		}

		TRef<TShaderHandleD3D11> MyShader = RefCast<TShaderHandleD3D11>(Shader->GetHandle());
		// TODO: Make handling for HLSL Class Instances (or should we?)
		if (MyShader)
		{
			if (MyShader->VertexShader && BoundVertexShader != MyShader->VertexShader)
			{
				BoundVertexShader = MyShader->VertexShader;
				Context->VSSetShader(BoundVertexShader, nullptr, 0);
			}

			if (MyShader->PixelShader && BoundPixelShader != MyShader->PixelShader)
			{
				BoundPixelShader = MyShader->PixelShader;
				Context->PSSetShader(BoundPixelShader, nullptr, 0);
			}

			if (MyShader->ComputeShader && BoundComputeShader != MyShader->ComputeShader)
			{
				BoundComputeShader = MyShader->ComputeShader;
				Context->CSSetShader(BoundComputeShader, nullptr, 0);
			}
		}
	}

	void TCommandListImmediateD3D11::BindPipeline(TRef<TGraphicsPipeline> Pipeline)
	{
		CHECK(IsRenderThread());
		BindShader(Pipeline->GetShader());
		TRef<TGraphicsPipelineHandleD3D11> Handle = RefCast<TGraphicsPipelineHandleD3D11>(Pipeline->GetHandle());
		if (Handle && BoundGraphicsPipeline != Pipeline.Raw())
		{
			Context->IASetPrimitiveTopology(Handle->GetPrimitiveTopology());
			Context->IASetInputLayout(Handle->GetInputLayout());
			// TODO: Deal with stencil
			Context->OMSetDepthStencilState(Handle->GetDepthStencilState(), D3D11_STENCIL_OP_KEEP);
			Context->RSSetState(Handle->GetRasterState());
			bHasAttachedPipeline = true;
			BoundGraphicsPipeline = Pipeline.Raw();
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
	void TCommandListImmediateD3D11::SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth)
	{
		CHECK(IsRenderThread());

		D3D11_VIEWPORT Viewport{};
		Viewport.TopLeftX = X;
		Viewport.TopLeftY = Y;
		Viewport.Width = Width;
		Viewport.Height = Height;
		Viewport.MinDepth = MinDepth;
		Viewport.MaxDepth = MaxDepth;
		
		Context->RSSetViewports(1, &Viewport);
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::SetScissor(float X, float Y, float Width, float Height)
	{
		D3D11_RECT Rect{};
		Rect.left = (LONG)X;
		Rect.right = (LONG)X + (LONG)Width;
		Rect.top = (LONG)Y;
		Rect.bottom = (LONG)Y + (LONG)Height;
		
		Context->RSSetScissorRects(1, &Rect);
	}

	//////////////////////////////////////////////////////////////////////////
	void* TCommandListImmediateD3D11::MapBuffer(TRef<TBuffer> Buffer)
	{
		CHECK(IsRenderThread());

		D3D11_MAPPED_SUBRESOURCE Subresource;
		HRCHECK(Context->Map((ID3D11Resource*)Buffer->GetNativeHandle(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource));
		return Subresource.pData;
	}


	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::UnmapBuffer(TRef<TBuffer> Buffer)
	{
		CHECK(IsRenderThread());
		Context->Unmap((ID3D11Resource*)Buffer->GetNativeHandle(), 0);
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::BindParamBuffers(TRef<TParamBuffer> ParamBuffer, u32 Slot)
	{
		CHECK(IsRenderThread());
		CHECK(ParamBuffer);

		const EShaderStageFlags Stages = ParamBuffer->GetShaderStages();
		if (Stages == 0)
		{
			return;
		}

		auto MyBuffer = RefCast<TParamBufferD3D11>(ParamBuffer);
		if (MyBuffer)
		{
			ID3D11Buffer* BindBuffers[] = { (ID3D11Buffer*)MyBuffer->GetNativeHandle() };
			if (Stages & EShaderStageFlags::Vertex)
			{
				Context->VSSetConstantBuffers(Slot, 1, BindBuffers);
			}

			if (Stages & EShaderStageFlags::Pixel)
			{
				Context->PSSetConstantBuffers(Slot, 1, BindBuffers);
			}

			if (Stages & EShaderStageFlags::Compute)
			{
				Context->CSSetConstantBuffers(Slot, 1, BindBuffers);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TCommandListImmediateD3D11::BindParamBuffers(TDynArray<TRef<TParamBuffer>> ParamBuffers, u32 Slot)
	{
		CHECK(IsRenderThread());
		bool bAllocatedVSBuffers = false;
		bool bAllocatedPSBuffers = false;
		bool bAllocatedCSBuffers = false;
		TDynArray<ID3D11Buffer*> Buffers;
		const usize BufferCount = ParamBuffers.GetLength();
		Buffers.Resize(BufferCount * 3);

		const usize VSOffset = BufferCount * 0;
		const usize PSOffset = BufferCount * 1;
		const usize CSOffset = BufferCount * 2;
		
		usize Index = 0;
		for (const auto& Buffer : ParamBuffers)
		{
			const EShaderStageFlags Stages = Buffer->GetShaderStages();
			if (Stages == 0)
			{
				continue;
			}

			auto MyBuffer = RefCast<TParamBufferD3D11>(Buffer);
			if (MyBuffer)
			{
				if (Stages & EShaderStageFlags::Vertex)
				{
					if (!bAllocatedVSBuffers)
					{
						bAllocatedVSBuffers = true;
					}
					Buffers[VSOffset + Index] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}

				if (Stages & EShaderStageFlags::Pixel)
				{
					if (!bAllocatedPSBuffers)
					{
						bAllocatedPSBuffers = true;
					}
					Buffers[PSOffset + Index] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}

				if (Stages & EShaderStageFlags::Compute)
				{
					if (!bAllocatedCSBuffers)
					{
						bAllocatedCSBuffers = true;
					}
					Buffers[CSOffset + Index] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}
			}
			Index++;
		}

		if (bAllocatedVSBuffers)
		{
			Context->VSSetConstantBuffers(Slot, (UINT)BufferCount, Buffers.GetData() + VSOffset);
		}
		if (bAllocatedPSBuffers)
		{
			Context->PSSetConstantBuffers(Slot, (UINT)BufferCount, Buffers.GetData() + PSOffset);
		}
		if (bAllocatedCSBuffers)
		{
			Context->CSSetConstantBuffers(Slot, (UINT)BufferCount, Buffers.GetData() + CSOffset);
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