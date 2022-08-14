#pragma once
#ifdef WIN32
#include "D3D11Common.h"
#include "Renderer/RenderDevice.h"

class IDXGIInfoQueue;

namespace Kepler
{
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
		virtual TRef<TSwapChain> CreateSwapChainForWindow(class TWindow* Window) override;

		void Internal_InitInfoMessageStartIndex_Debug();
		std::vector<std::string> GetInfoQueueMessages() const;

	private:
		void CreateFactory();
		void CreateDevice();
		void InitializeInfoQueue();

	private:
		IDXGIFactory7* Factory{};
		ID3D11Device5* Device{};
		ID3D11DeviceContext4* ImmediateContext{};
		IDXGIInfoQueue* InfoQueue{};
		u64 InfoMsgStartIndex = 0;
	};
}
#endif