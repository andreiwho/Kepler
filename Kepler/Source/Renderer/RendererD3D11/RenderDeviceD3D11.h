#pragma once
#ifdef WIN32
#include "D3D11Common.h"
#include "Renderer/RenderDevice.h"

namespace Kepler
{
	class TRenderDeviceD3D11 : public TRenderDevice
	{
	public:
		TRenderDeviceD3D11();
		~TRenderDeviceD3D11();

	private:
		void CreateFactory();
		void CreateDevice();

	private:
		IDXGIFactory7* Factory{};
		ID3D11Device5* Device{};
		ID3D11DeviceContext* ImmediateContext{};
	};
}
#endif