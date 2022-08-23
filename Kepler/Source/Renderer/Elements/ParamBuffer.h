#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "../Pipelines/ParamPack.h"
#include "Buffer.h"

namespace Kepler
{
	class TParamBuffer : public TBuffer
	{
	public:
		TParamBuffer(TRef<TPipelineParamPack> ParamPack);

		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			Params->Write<T>(Param, Data);
		}

		template<typename T>
		T* GetParam(const TString& Param)
		{
			return Params->GetParam<T>(Param);
		}

		virtual void RT_UploadToGPU(TRef<class TCommandListImmediate> pImmContext) = 0;

		inline EShaderStageFlags GetShaderStages() const { return Params->GetShaderStages(); }

	protected:
		TRef<TPipelineParamPack> Params;
	};
}