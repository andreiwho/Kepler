#pragma once
#ifdef WIN32
#include "D3D11Common.h"
#include "Renderer/RenderDevice.h"
#include "Core/Containers/DynArray.h"
#include "Core/Containers/RingQueue.h"
#include <mutex>
#include "../Elements/RenderTarget.h"

class IDXGIInfoQueue;

namespace ke
{
	class AsyncDataBlobD3D11 : public AsyncDataBlob
	{
	public:
		AsyncDataBlobD3D11(const void* Data, usize Size, usize ElemSize = 0);
		virtual const void* GetData() const override;
		virtual usize GetSize() const override;
		virtual void Write(const void* Data, usize Size) override;
		ID3DBlob* GetBlob() const { return Blob; }
		virtual usize GetStride() const override { return Stride; }

	private:
		CComPtr<ID3DBlob> Blob{};
		usize Stride{};
	};

	class TRenderDeviceD3D11 : public TRenderDevice
	{
		static TRenderDeviceD3D11* Instance;
	public:
		TRenderDeviceD3D11();
		~TRenderDeviceD3D11();

		static TRenderDeviceD3D11* Get() { return Instance; }
		inline IDXGIFactory7* GetFactory() const { return Factory; }
		inline ID3D11Device5* GetDevice() const { return Device; }
		inline IDXGIInfoQueue* GetInfoQueue() const { return InfoQueue; }
		inline ID3D11DeviceContext4* GetImmediateContext() const { return ImmediateContext; }
		inline ID3D11ClassLinkage* GetClassLinkage() const { return ClassLinkage; }
		virtual TRef<TVertexBuffer> CreateVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data) override;
		virtual TRef<TIndexBuffer> CreateIndexBuffer(EBufferAccessFlags InAccessFlags, TRef<AsyncDataBlob> Data) override;
		virtual TRef<TTransferBuffer> CreateTransferBuffer(usize Size, TRef<AsyncDataBlob> InitialData) override;
		virtual TRef<TParamBuffer> CreateParamBuffer(TRef<TPipelineParamMapping> Params) override;
		virtual TRef<TSwapChain> CreateSwapChainForWindow(TWindow* Window) override;
		virtual TRef<TImage2D> CreateImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 MipLevels, u32 InArraySize = 1) override;
		virtual TRef<RenderTarget2D> CreateRenderTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0) override;
		virtual TRef<DepthStencilTarget2D> CreateDepthStencilTarget2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0, bool bReadOnly = false) override;
		virtual TRef<TTextureSampler2D> CreateTextureSampler2D(TRef<TImage2D> InImage, u32 MipLevel = 0, u32 ArrayLayer = 0) override;

		void Internal_InitInfoMessageStartIndex_Debug();
		Array<TString> GetInfoQueueMessages() const;

		virtual bool RT_FlushPendingDeleteResources() override;

		void RegisterPendingDeleteResource(ID3D11DeviceChild* Resource);

	private:
		void CreateFactory();
		void CreateDevice();
		void InitializeInfoQueue();
		void CreateClassLinkage();

	private:
		IDXGIFactory7* Factory{};
		ID3D11Device5* Device{};
		ID3D11DeviceContext4* ImmediateContext{};
		IDXGIInfoQueue* InfoQueue{};
		u64 InfoMsgStartIndex = 0;
		ID3D11ClassLinkage* ClassLinkage{};

		TThreadSafeRingQueue<ID3D11DeviceChild*> PendingDeleteResources{ 100000 };

		std::mutex ResourceMutex;
	};
}
#endif