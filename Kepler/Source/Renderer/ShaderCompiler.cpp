#pragma once
#include "ShaderCompiler.h"
#include "RenderGlobals.h"
#include "Core/Malloc.h"

#ifdef WIN32
# include "RendererD3D11/ShaderCompilerD3D11.h"
#endif

namespace Kepler
{
	TRef<TShaderCompiler> TShaderCompiler::CreateShaderCompiler()
	{
		switch (GRenderAPI)
		{
		case ERenderAPI::DirectX11:
			return MakeRef(New<TShaderCompilerD3D11>());
		default:
			break;
		}
		return nullptr;
	}
}