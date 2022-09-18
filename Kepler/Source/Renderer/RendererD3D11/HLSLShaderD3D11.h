#pragma once
#include "Core/Types.h"
#include "Renderer/Elements/HLSLShader.h"
#include "Renderer/RenderTypes.h"
#include "D3D11Common.h"
#include "Core/Malloc.h"
#include "Renderer/Elements/VertexLayout.h"
#include "Renderer/Pipelines/ParamPack.h"

namespace Kepler
{
	struct TShaderHandleD3D11 : public TShaderHandle
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

	class THLSLShaderD3D11 : public THLSLShader
	{
	public:
		THLSLShaderD3D11(const TString& Name, const TDynArray<TShaderModule>& Modules);

	protected:
		inline TRef<TShaderHandleD3D11> GetD3D11Handle() const { return RefCast<TShaderHandleD3D11>(Handle); }

	private:
		void InitHandle();
		void InitShaders(const TDynArray<TShaderModule>& Modules);

		// Reflection interface
		void InitReflection(const TDynArray<TShaderModule>& Modules);
		TVertexLayout ReflectVertexLayout(CComPtr<ID3D11ShaderReflection> pReflection, const TShaderModule& VertexShaderModule);
		TRef<TPipelineParamMapping> ReflectParams(CComPtr<ID3D11ShaderReflection> pReflection, EShaderStageFlags StageFlags, TRef<TPipelineParamMapping> ToMerge);
	};
}