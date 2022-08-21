#include "HLSLShaderD3D11.h"
#include "RenderDeviceD3D11.h"
#include "Renderer/RenderGlobals.h"

#include "D3D11Common.h"
#include "Async/Async.h"

namespace Kepler
{

	THLSLShaderD3D11::THLSLShaderD3D11(const std::string& Name, const TDynArray<TShaderModule>& Modules)
		: THLSLShader(Name, Modules)
	{
		InitHandle();
		InitShaders(Modules);
	}

	void THLSLShaderD3D11::InitHandle()
	{
		Handle = MakeRef(New<TShaderHandleD3D11>());
		Handle->StageMask = ShaderStageMask;	// We have this being setup inside the TShader
	}

	void THLSLShaderD3D11::InitShaders(const TDynArray<TShaderModule>& Modules)
	{
		// CHECK(IsRenderThread());
		auto Device = TRenderDeviceD3D11::Get();
		if (Modules.GetLength() > 0)
		{
			CHECK(Device);
		}

		for (const auto& Module : Modules)
		{
			if (Module.ByteCode && VALIDATED(Module.ByteCode->GetSize() > 0))
			{
				if (Module.StageFlags & EShaderStageFlags::Vertex)
				{
					HRCHECK(Device->GetDevice()->CreateVertexShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->VertexShader
					));
				}

				if (Module.StageFlags & EShaderStageFlags::Pixel)
				{
					HRCHECK(Device->GetDevice()->CreatePixelShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->PixelShader
					));
				}

				if (Module.StageFlags & EShaderStageFlags::Compute)
				{
					HRCHECK(Device->GetDevice()->CreateComputeShader(
						Module.ByteCode->GetData(),
						Module.ByteCode->GetSize(),
						Device->GetClassLinkage(),
						&CHECKED(GetD3D11Handle())->ComputeShader
					));
				}
			}
		}
	}

	TShaderHandleD3D11::~TShaderHandleD3D11()
	{
		TRenderDeviceD3D11* Device = TRenderDeviceD3D11::Get();
		CHECK_NOTHROW(Device);
		if (VertexShader)
		{
			Device->RegisterPendingDeleteResource(VertexShader);
		}
		if (PixelShader)
		{
			Device->RegisterPendingDeleteResource(PixelShader);
		}
		if (ComputeShader)
		{
			Device->RegisterPendingDeleteResource(ComputeShader);
		}
	}

}

