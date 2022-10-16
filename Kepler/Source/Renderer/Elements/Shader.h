#pragma once
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include <string>
#include "ShaderReflection.h"

namespace ke
{
	// This is a basic class for all kinds of shader handles 
	// - For D3D11 it is TShaderHandleD3D11, which holds the ID3D11VertexShader*, ID3D11PixelShader* and other kinds of handles
	struct TShaderHandle : public IntrusiveRefCounted
	{
		EShaderStageFlags StageMask;
	};

	struct TShaderModule
	{
		RefPtr<AsyncDataBlob> ByteCode;
		EShaderStageFlags StageFlags;
	};

	class TShader;
	class TShaderCache
	{
		static TShaderCache* Instance;
	public:
		static TShaderCache* Get() { return Instance; }

		TShaderCache() { Instance = this; }

		bool Exists(const TString& Name) const;
		void Add(const TString& Name, RefPtr<TShader> Shader);
		RefPtr<TShader> GetShader(const TString& Name) const;
		void Invalidate();

	private:
		Map<TString, RefPtr<TShader>> LoadedShaders;
	};

	class TShader : public IntrusiveRefCounted
	{
	public:
		TShader(const TString& InName, const Array<TShaderModule>& ShaderModules);
		virtual ~TShader() = default;

		inline RefPtr<TShaderHandle> GetHandle() const { return Handle; }
		inline const TString& GetName() const { return Name; }
		inline RefPtr<AsyncDataBlob> GetVertexShaderBytecode() const { return TempVertexShaderBytecode; }
		inline RefPtr<TShaderModuleReflection> GetReflection() const { return ReflectionData; }

	protected:
		RefPtr<TShaderHandle> Handle{};
		RefPtr<AsyncDataBlob> TempVertexShaderBytecode;
		EShaderStageFlags ShaderStageMask{};

		// This data gets created and maintained by the child shader class.
		// May be NULL
		RefPtr<TShaderModuleReflection> ReflectionData{};

	private:
		TString Name{};
	};
}