#pragma once
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include <string>
#include "ShaderReflection.h"

namespace Kepler
{
	// This is a basic class for all kinds of shader handles 
	// - For D3D11 it is TShaderHandleD3D11, which holds the ID3D11VertexShader*, ID3D11PixelShader* and other kinds of handles
	struct TShaderHandle : public TRefCounted
	{
		EShaderStageFlags StageMask;
	};

	struct TShaderModule
	{
		TRef<TDataBlob> ByteCode;
		EShaderStageFlags StageFlags;
	};

	class TShader : public TRefCounted
	{
	public:
		TShader(const TString& InName, const TDynArray<TShaderModule>& ShaderModules);
		virtual ~TShader() = default;

		inline TRef<TShaderHandle> GetHandle() const { return Handle; }
		inline const TString& GetName() const { return Name; }
		inline TRef<TDataBlob> GetVertexShaderBytecode() const { return TempVertexShaderBytecode; }
		inline TRef<TShaderModuleReflection> GetReflection() const { return ReflectionData; }

	protected:
		TRef<TShaderHandle> Handle{};
		TRef<TDataBlob> TempVertexShaderBytecode;
		EShaderStageFlags ShaderStageMask{};

		// This data gets created and maintained by the child shader class.
		// May be NULL
		TRef<TShaderModuleReflection> ReflectionData{};

	private:
		TString Name{};
	};
}