#include "CommandListImmediateD3D11.h"
#include "SwapChainD3D11.h"
#include "Renderer/RenderGlobals.h"
#include "VertexBufferD3D11.h"
#include "IndexBufferD3D11.h"
#include "Core/Log.h"
#include "HLSLShaderD3D11.h"
#include "GraphicsPipelineHandleD3D11.h"
#include "ParamBufferD3D11.h"
#include "RenderTargetD3D11.h"
#include "ImageD3D11.h"
#include "TextureD3D11.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogImmediateContext, All);

	GraphicsCommandListImmediateD3D11* GraphicsCommandListImmediateD3D11::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	GraphicsCommandListImmediateD3D11::GraphicsCommandListImmediateD3D11(ID3D11DeviceContext4* InContext) : Context(InContext)
	{
		CHECK(!Instance);
		Instance = this;

		CHECK(IsRenderThread());
		Context->AddRef();
	}

	//////////////////////////////////////////////////////////////////////////
	GraphicsCommandListImmediateD3D11::~GraphicsCommandListImmediateD3D11()
	{
		CHECK_NOTHROW(IsRenderThread());
		if (AnnotationInterface)
		{
			AnnotationInterface->Release();
		}
		Context->Release();
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::StartDrawingToSwapChainImage(TRef<TSwapChain> pSwapChain, TRef<DepthStencilTarget2D> pDepthStencil)
	{
		CHECK(IsRenderThread());
		CHECK(pSwapChain && Context);
		TRef<TSwapChainD3D11> MySwapChain = RefCast<TSwapChainD3D11>(pSwapChain);
		ID3D11RenderTargetView* ppRTV[] = { CHECKED(MySwapChain->GetRenderTargetView()) };
		ID3D11DepthStencilView* pDsv = nullptr;
		if (pDepthStencil)
		{
			if (auto MyDSV = RefCast<DepthStencilTarget2D_D3D11>(pDepthStencil))
			{
				pDsv = MyDSV->GetView();
			}
		}

		Context->OMSetRenderTargets(ARRAYSIZE(ppRTV), ppRTV, pDsv);
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::ClearSwapChainImage(TRef<TSwapChain> SwapChain, float4 ClearColor)
	{
		CHECK(IsRenderThread());
		CHECK(SwapChain && Context);
		TRef<TSwapChainD3D11> MySwapChain = RefCast<TSwapChainD3D11>(SwapChain);
		const float NewColor[4] = { ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a };
		Context->ClearRenderTargetView(MySwapChain->GetRenderTargetView(), NewColor);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::Draw(u32 VertexCount, u32 BaseVertexIndex)
	{
		CHECK(IsRenderThread());
		if (VALIDATEDMSG(HasPipelineStateSetup(), "Pipeline state not setup. Cannot draw."))
		{
			Context->Draw(VertexCount, BaseVertexIndex);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindVertexBuffers(TRef<TVertexBuffer> VertexBuffer, u32 StartSlot, u32 Offset)
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
	void GraphicsCommandListImmediateD3D11::BindVertexBuffers(const Array<TRef<TVertexBuffer>>& VertexBuffers, u32 StartSlot, const Array<u32>& Offsets)
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
			Array<ID3D11Buffer*> ppBuffers;
			Array<u32> pStrides;
			Array<u32> pOffsets;

			ppBuffers.Reserve(VertexBuffers.GetLength());
			pStrides.Reserve(VertexBuffers.GetLength());
			pOffsets.Reserve(VertexBuffers.GetLength());

			usize idx = 0;
			for (const auto& Buffer : VertexBuffers)
			{
				if (Buffer)
				{
					if (TRef<TVertexBufferD3D11> MyBuffer = RefCast<TVertexBufferD3D11>(Buffer))
					{
						ppBuffers.EmplaceBack(MyBuffer->GetBuffer());
						pStrides.EmplaceBack((u32)MyBuffer->GetStride());
						pOffsets.EmplaceBack(bOffsetsHasEntries ? (u32)Offsets[idx] : (u32)0);
					}
				}
				idx++;
			}

			CHECK((pStrides.GetLength() == ppBuffers.GetLength()) && (pOffsets.GetLength() == ppBuffers.GetLength()));
			Context->IASetVertexBuffers(StartSlot, (UINT)ppBuffers.GetLength(), ppBuffers.GetData(), pStrides.GetData(), pOffsets.GetData());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindShader(TRef<TShader> Shader)
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
			if (BoundVertexShader != MyShader->VertexShader)
			{
				BoundVertexShader = MyShader->VertexShader;
				Context->VSSetShader(BoundVertexShader, nullptr, 0);
			}

			if (BoundPixelShader != MyShader->PixelShader)
			{
				BoundPixelShader = MyShader->PixelShader;
				Context->PSSetShader(BoundPixelShader, nullptr, 0);
			}

			if (BoundComputeShader != MyShader->ComputeShader)
			{
				BoundComputeShader = MyShader->ComputeShader;
				Context->CSSetShader(BoundComputeShader, nullptr, 0);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindSamplers(TRef<TPipelineSamplerPack> Samplers, u32 Slot)
	{
		CHECK(IsRenderThread());

		Array<ID3D11SamplerState*> ppSamplers;
		Array<ID3D11ShaderResourceView*> ppShaderResources;

		if (auto SamplerCount = Samplers->GetSamplers().GetLength())
		{
			ppSamplers.Resize(SamplerCount);
			ppShaderResources.Resize(SamplerCount);
			for (usize idx = 0; idx < SamplerCount; ++idx)
			{
				if (auto Sampler = RefCast<TTextureSampler2D_D3D11>(Samplers->GetSamplers()[idx]))
				{
					ppSamplers[idx] = Sampler->GetSampler();
					ppShaderResources[idx] = Sampler->GetView();
				}
			}

			Context->PSSetShaderResources(Slot, ppShaderResources.GetLength(), ppShaderResources.GetData());
			Context->PSSetSamplers(Slot, ppSamplers.GetLength(), ppSamplers.GetData());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::ClearSamplers(u32 Slot)
	{
		CHECK(IsRenderThread());
		static constexpr UINT ResourceCount = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
		std::array<void*, ResourceCount> Nulls{};
		for (auto& Null : Nulls) { Null = nullptr; }

		// This is a little hack to do this fast and without allocations
		Context->PSSetShaderResources(Slot, ResourceCount, (ID3D11ShaderResourceView**)Nulls.data());
		Context->PSSetSamplers(Slot, ResourceCount, (ID3D11SamplerState**)Nulls.data());

	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindPipeline(TRef<TGraphicsPipeline> Pipeline)
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
			m_bHasAttachedPipeline = true;
			BoundGraphicsPipeline = Pipeline.Raw();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::DrawIndexed(u32 IndexCount, u32 BaseIndexOffset, u32 BaseVertexOffset)
	{
		CHECK(IsRenderThread());

		// Let us be a little forgiving here
		if (VALIDATED(HasPipelineStateSetup()))
		{
			Context->DrawIndexed(IndexCount, BaseIndexOffset, (INT)BaseVertexOffset);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::SetViewport(float X, float Y, float Width, float Height, float MinDepth, float MaxDepth)
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
	void GraphicsCommandListImmediateD3D11::SetScissor(float X, float Y, float Width, float Height)
	{
		D3D11_RECT Rect{};
		Rect.left = (LONG)X;
		Rect.right = (LONG)X + (LONG)Width;
		Rect.top = (LONG)Y;
		Rect.bottom = (LONG)Y + (LONG)Height;

		Context->RSSetScissorRects(1, &Rect);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::StartDrawingToRenderTargets(TRef<RenderTarget2D> RenderTarget, TRef<DepthStencilTarget2D> DepthStencil)
	{
		CHECK(IsRenderThread());
		ID3D11RenderTargetView* View = nullptr;
		if (RenderTarget)
		{
			TRef<RenderTarget2D_D3D11> MyTarget = RefCast<RenderTarget2D_D3D11>(RenderTarget);
			if (MyTarget)
			{
				View = MyTarget->GetView();
			}
		}

		ID3D11DepthStencilView* DepthStencilView = nullptr;
		if (DepthStencil)
		{
			if (auto MyDepthStencil = RefCast<DepthStencilTarget2D_D3D11>(DepthStencil))
			{
				DepthStencilView = MyDepthStencil->GetView();
			}
		}

		Context->OMSetRenderTargets(1, &View, DepthStencilView);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::StartDrawingToRenderTargets(const Array<TRef<RenderTarget2D>>& RenderTargets, TRef<DepthStencilTarget2D> DepthStencil)
	{
		CHECK(IsRenderThread());

		Array<ID3D11RenderTargetView*> ppRTVs;
		ID3D11DepthStencilView* pDSV = nullptr;

		if (RenderTargets.GetLength() > 0)
		{
			ppRTVs.Reserve(RenderTargets.GetLength());
			for (TRef<RenderTarget2D> Target : RenderTargets)
			{
				if (auto MyTarget = RefCast<RenderTarget2D_D3D11>(Target))
				{
					ppRTVs.EmplaceBack(MyTarget->GetView());
				}
			}
		}

		if (auto MyDepthStencil = RefCast<DepthStencilTarget2D_D3D11>(DepthStencil))
		{
			pDSV = MyDepthStencil->GetView();
		}

		Context->OMSetRenderTargets((UINT)ppRTVs.GetLength(), ppRTVs.GetData(), pDSV);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::ClearRenderTarget(TRef<RenderTarget2D> Target, float4 Color)
	{
		if (auto MyTarget = RefCast<RenderTarget2D_D3D11>(Target))
		{
			ID3D11RenderTargetView* View = MyTarget->GetView();
			if (View)
			{
				FLOAT ClearColor[4] = { Color.r, Color.g, Color.b, Color.a };
				Context->ClearRenderTargetView(View, ClearColor);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::ClearDepthTarget(TRef<DepthStencilTarget2D> Target, bool bClearStencil)
	{
		CHECK(IsRenderThread());

		if (auto MyTarget = RefCast<DepthStencilTarget2D_D3D11>(Target))
		{
			ID3D11DepthStencilView* Dsv = MyTarget->GetView();
			if (Dsv)
			{
				Context->ClearDepthStencilView(Dsv, bClearStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH, 1.0f, 0);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void* GraphicsCommandListImmediateD3D11::MapBuffer(TRef<Buffer> buffer)
	{
		CHECK(IsRenderThread());

		D3D11_MAPPED_SUBRESOURCE Subresource;
		HRCHECK(Context->Map((ID3D11Resource*)buffer->GetNativeHandle(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource));
		return Subresource.pData;
	}


	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::UnmapBuffer(TRef<Buffer> Buffer)
	{
		CHECK(IsRenderThread());
		Context->Unmap((ID3D11Resource*)Buffer->GetNativeHandle(), 0);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindParamBuffers(TRef<TParamBuffer> ParamBuffer, u32 Slot)
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
	void GraphicsCommandListImmediateD3D11::BindParamBuffers(Array<TRef<TParamBuffer>> ParamBuffers, u32 Slot)
	{
		CHECK(IsRenderThread());
		bool bAllocatedVSBuffers = false;
		bool bAllocatedPSBuffers = false;
		bool bAllocatedCSBuffers = false;
		Array<ID3D11Buffer*> Buffers;
		const usize BufferCount = ParamBuffers.GetLength();
		Buffers.Resize(BufferCount * 3);

		const usize VSOffset = BufferCount * 0;
		const usize PSOffset = BufferCount * 1;
		const usize CSOffset = BufferCount * 2;

		usize idx = 0;
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
					Buffers[VSOffset + idx] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}

				if (Stages & EShaderStageFlags::Pixel)
				{
					if (!bAllocatedPSBuffers)
					{
						bAllocatedPSBuffers = true;
					}
					Buffers[PSOffset + idx] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}

				if (Stages & EShaderStageFlags::Compute)
				{
					if (!bAllocatedCSBuffers)
					{
						bAllocatedCSBuffers = true;
					}
					Buffers[CSOffset + idx] = (ID3D11Buffer*)MyBuffer->GetNativeHandle();
				}
			}
			idx++;
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
	void GraphicsCommandListImmediateD3D11::Transfer(TRef<TTransferBuffer> From, TRef<Buffer> To, usize DstOffset, usize SrcOffset, usize Size)
	{
		CHECK(IsRenderThread());

		ID3D11Resource* pFrom = (ID3D11Resource*)From->GetNativeHandle();
		ID3D11Resource* pTo = (ID3D11Resource*)To->GetNativeHandle();

		CD3D11_BOX SrcBox(SrcOffset, 0, 0, SrcOffset + Size, 1, 1);
		Context->CopySubresourceRegion(pTo, 0, DstOffset, 0, 0, pFrom, 0, &SrcBox);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::Transfer(TRef<TImage2D> Into, usize X, usize Y, usize Width, usize Height, TRef<AsyncDataBlob> Data)
	{
		CHECK(IsRenderThread());
		if (auto MyImage = RefCast<TImage2D_D3D11>(Into))
		{
			CD3D11_BOX CopyBox((LONG)X, (LONG)Y, 0, (LONG)X + (LONG)Width, (LONG)Y + (LONG)Height, 1);
			Context->UpdateSubresource(MyImage->GetImage(), 0, &CopyBox, Data->GetData(), (UINT)Data->GetSize() / (Height > 0 ? Height : 1), 0);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BeginDebugEvent(const char* Name)
	{
#ifdef ENABLE_DEBUG
		if (!AnnotationInterface)
		{
			HRCHECK(Context->QueryInterface(&AnnotationInterface));
		}

		static constexpr SIZE_T MaxBufferSize = 256;
		wchar_t NameBuffer[MaxBufferSize] = {};
		mbstowcs(NameBuffer, Name, MaxBufferSize);
		AnnotationInterface->BeginEvent(NameBuffer);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::EndDebugEvent()
	{
#ifdef ENABLE_DEBUG
		if (AnnotationInterface)
		{
			AnnotationInterface->EndEvent();
		}
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	void* GraphicsCommandListImmediateD3D11::MapImage2D(TRef<TImage2D> Image, usize& OutAlignment)
	{
		CHECK(IsRenderThread());
		TRef<TImage2D_D3D11> MyImage = RefCast<TImage2D_D3D11>(Image);
		MyImage->RequireReadbackCopy(RefCast<GraphicsCommandListImmediate>(RefFromThis()));

		D3D11_MAPPED_SUBRESOURCE Subresource;
		HRESULT HR = Context->Map(MyImage->GetReadbackImage(), 0, D3D11_MAP_READ, 0, &Subresource);
		HRCHECK(HR);
		OutAlignment = Subresource.RowPitch;
		return Subresource.pData;
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::UnmapImage2D(TRef<TImage2D> Image)
	{
		CHECK(IsRenderThread());
		TRef<TImage2D_D3D11> MyImage = RefCast<TImage2D_D3D11>(Image);
		Context->Unmap(MyImage->GetReadbackImage(), 0);
	}

	void* GraphicsCommandListImmediateD3D11::MapParamBuffer_NextFrame(TRef<TParamBufferD3D11> buffer)
	{
		CHECK(IsRenderThread());

		D3D11_MAPPED_SUBRESOURCE Subresource;
		HRCHECK(Context->Map((ID3D11Resource*)buffer->GetNextFrameHandle(), 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource));
		return Subresource.pData;
	}

	void GraphicsCommandListImmediateD3D11::UnmapParamBuffer_NextFrame(TRef<TParamBufferD3D11> Buffer)
	{
		CHECK(IsRenderThread());
		Context->Unmap((ID3D11Resource*)Buffer->GetNextFrameHandle(), 0);
	}

	//////////////////////////////////////////////////////////////////////////
	void GraphicsCommandListImmediateD3D11::BindIndexBuffer(TRef<TIndexBuffer> IndexBuffer, u32 Offset)
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