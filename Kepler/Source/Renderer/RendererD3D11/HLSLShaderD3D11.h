#pragma once
#include "Core/Types.h"
#include "Renderer/Elements/HLSLShader.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"
#include "Core/Malloc.h"
#include "Renderer/Elements/VertexLayout.h"
#include "Renderer/Pipelines/ParamPack.h"

namespace ke
{
	struct TShaderHandleD3D11 : public IShaderHandle
	{
		TShaderHandleD3D11() = default;
		~TShaderHandleD3D11();

		TShaderHandleD3D11(EShaderStageFlags Mask)
		{
			StageMask = Mask;
		}

		ID3D11VertexShader* VertexShader{};
		ID3D11PixelShader* PixelShader{};
		ID3D11ComputeShader* ComputeShader{};
	};

	class THLSLShaderD3D11 : public HLSLShader
	{
	public:
		THLSLShaderD3D11(const TString& Name, const Array<ShaderModule>& Modules);

	protected:
		inline RefPtr<TShaderHandleD3D11> GetD3D11Handle() const { return RefCast<TShaderHandleD3D11>(m_Handle); }

	private:
		void InitHandle();
		void InitShaders(const Array<ShaderModule>& Modules);

		// Reflection interface
		void InitReflection(const Array<ShaderModule>& Modules);
		VertexLayout ReflectVertexLayout(ID3D11ShaderReflection* pReflection, const ShaderModule& VertexShaderModule);
		RefPtr<PipelineParamMapping> ReflectParams(ID3D11ShaderReflection* pReflection, EShaderStageFlags StageFlags, RefPtr<PipelineParamMapping> ToMerge);
	};
}