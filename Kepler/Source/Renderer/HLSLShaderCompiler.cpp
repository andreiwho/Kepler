#pragma once
#include "HLSLShaderCompiler.h"
#include "RenderGlobals.h"
#include "Core/Malloc.h"

#ifdef WIN32
# include "RendererD3D11/ShaderCompilerD3D11.h"
#endif

namespace ke
{
	RefPtr<THLSLShaderCompiler> THLSLShaderCompiler::CreateShaderCompiler()
	{
		switch (GRenderAPI)
		{
		case ERenderAPI::DirectX11:
			return MakeRef(New<THLSLShaderCompilerD3D11>());
		default:
			break;
		}
		return nullptr;
	}
}