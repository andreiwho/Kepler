#pragma once
#include "Core/Types.h"
#include "Renderer/Elements/Shader.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"
#include "Core/Malloc.h"

namespace Kepler
{
	class TVertexShaderD3D11 : public TVertexShader
	{
	public:
		TVertexShaderD3D11(TSharedPtr<TDataBlob> ShaderByteCode);

	private:
		ID3D11VertexShader* VertexShader{nullptr};
	};
}