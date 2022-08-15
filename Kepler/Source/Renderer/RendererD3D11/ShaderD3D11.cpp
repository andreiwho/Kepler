#include "ShaderD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Renderer/RenderGlobals.h"

#include "D3D11Common.h"

namespace Kepler
{
	struct TShaderCommonInterface
	{
		static ID3D11Device5* GetDevice()
		{
			return CHECKED(TRenderDeviceD3D11::Get())->GetDevice();
		}

		static ID3D11ClassLinkage* GetDeviceClassLinkage()
		{
			TRenderDeviceD3D11* Device = CHECKED(TRenderDeviceD3D11::Get());
			return CHECKED(Device->GetClassLinkage());
		}
	};

	TVertexShaderD3D11::TVertexShaderD3D11(TSharedPtr<TDataBlob> ShaderByteCode)
		:	TVertexShader(ShaderByteCode)
	{
		CHECK(IsRenderThread());

		auto Device = TShaderCommonInterface::GetDevice();
		HRCHECK(Device->CreateVertexShader(
			ShaderByteCode->GetData(),
			ShaderByteCode->GetSize(),
			TShaderCommonInterface::GetDeviceClassLinkage(),
			&VertexShader));

	}

}

