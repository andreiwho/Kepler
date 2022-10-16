#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "../Pipelines/ParamPack.h"
#include "Buffer.h"

namespace ke
{
	class TParamBuffer : public IBuffer
	{
	protected:
		TParamBuffer(RefPtr<TPipelineParamMapping> ParamPack);

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
			MarkRenderStateDirty();
			return Params->GetParam<T>(Param);
		}

		template<typename T>
		const T& ReadParamValue(const TString& Param) const
		{
			// MarkRenderStateDirty();
			return Params->GetParam<T>(Param);
		}

		inline bool HasParam(const TString& param) const
		{
			return Params->HasParam(param);
		}

		virtual void RT_UploadToGPU(RefPtr<class GraphicsCommandListImmediate> pImmContext) = 0;

		inline EShaderStageFlags GetShaderStages() const { return Params->GetShaderStages(); }

		static RefPtr<TParamBuffer> New(RefPtr<TPipelineParamMapping> ParamPack);

	protected:
		inline void MarkRenderStateDirty() { bRenderStateDirty = true; }
		inline bool IsRenderStateDirty() const { return bRenderStateDirty; }
		inline void ResetRenderState() { bRenderStateDirty = false; }

		RefPtr<TPipelineParamPack> Params;
	
	private:
		TAtomic<bool> bRenderStateDirty = true;
	};
}