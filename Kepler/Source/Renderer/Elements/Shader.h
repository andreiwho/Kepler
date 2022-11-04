#pragma once
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"
#include <string>
#include "ShaderReflection.h"

namespace ke
{
	// This is a basic class for all kinds of shader handles 
	// - For D3D11 it is TShaderHandleD3D11, which holds the ID3D11VertexShader*, ID3D11PixelShader* and other kinds of handles
	struct IShaderHandle : public IntrusiveRefCounted
	{
		EShaderStageFlags StageMask;
	};

	struct ShaderModule
	{
		RefPtr<IAsyncDataBlob> ByteCode;
		EShaderStageFlags StageFlags;
	};

	class IShader;
	class TShaderCache
	{
		static TShaderCache* Instance;
	public:
		static TShaderCache* Get() { return Instance; }

		TShaderCache() { Instance = this; }

		bool Exists(const String& name) const;
		void Add(const String& name, RefPtr<IShader> pShader);
		RefPtr<IShader> GetShader(const String& name) const;
		void Invalidate();

	private:
		Map<String, RefPtr<IShader>> m_LoadedShaders;
	};

	class IShader : public IntrusiveRefCounted
	{
	public:
		IShader(const String& name, const Array<ShaderModule>& shaderModules);
		virtual ~IShader() = default;

		inline RefPtr<IShaderHandle> GetHandle() const { return m_Handle; }
		inline const String& GetName() const { return m_Name; }
		inline RefPtr<IAsyncDataBlob> GetVertexShaderBytecode() const { return m_TempVertexShaderBytecode; }
		inline RefPtr<TShaderModuleReflection> GetReflection() const { return m_ReflectionData; }

	protected:
		RefPtr<IShaderHandle> m_Handle{};
		RefPtr<IAsyncDataBlob> m_TempVertexShaderBytecode;
		EShaderStageFlags m_ShaderStageMask{};

		// This data gets created and maintained by the child shader class.
		// May be NULL
		RefPtr<TShaderModuleReflection> m_ReflectionData{};

	private:
		String m_Name{};
	};
}