#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "../Pipelines/ParamPack.h"
#include "Buffer.h"

namespace Kepler
{
	class TParamBuffer : public TBuffer
	{
	protected:
		TParamBuffer(TRef<TPipelineParamMapping> ParamPack);

	public:
		template<typename T>
		void Write(const TString& Param, const T* Data)
		{
			Params->Write<T>(Param, Data);
			bRenderStateDirty = true;
		}

		// Note that calling this function marks render state as dirty which invalidates GPU buffer contents.
		// So calling this function may cause unexpected performance loss.
		// If you have intended to only read the value, then use ReadParamValue function, which does not invalidate the buffer contents
		template<typename T>
		T& GetParamForWriting(const TString& Param)
		{
			bRenderStateDirty = true;
			return Params->GetParam<T>(Param);
		}

		template<typename T>
		const T& ReadParamValue(const TString& Param) const
		{
			return Params->GetParam<T>(Param);
		}

		virtual void RT_UploadToGPU(TRef<class TCommandListImmediate> pImmContext) = 0;

		inline EShaderStageFlags GetShaderStages() const { return Params->GetShaderStages(); }

		static TRef<TParamBuffer> New(TRef<TPipelineParamMapping> ParamPack);
	protected:
		TRef<TPipelineParamPack> Params;
		TAtomic<bool> bRenderStateDirty = true;
	};
}