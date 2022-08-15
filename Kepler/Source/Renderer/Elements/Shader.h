#pragma once
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	class TShaderLayout
	{

	};

#define BEGIN_SHADER_LAYOUT(Name)

#define END_SHADER_LAYOUT()

	class TShader : public TRefCounted
	{
	public:
		TShader(TSharedPtr<TDataBlob> ShaderByteCode) {}
		virtual ~TShader() = default;

	private:
	};

	class TVertexShader : public TShader
	{
	public:
		TVertexShader(TSharedPtr<TDataBlob> ShaderByteCode) : TShader(ShaderByteCode) {}
	};

	class TPixelShader : public TShader
	{
	public:
		TPixelShader(TSharedPtr<TDataBlob> ShaderByteCode) : TShader(ShaderByteCode) {}
	};
}