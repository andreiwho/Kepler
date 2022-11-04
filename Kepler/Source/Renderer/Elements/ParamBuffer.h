#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "../Pipelines/ParamPack.h"
#include "Buffer.h"

namespace ke
{
	class IParamBuffer : public IBuffer
	{
	protected:
		IParamBuffer(RefPtr<PipelineParamMapping> pParamMapping);

	public:
		template<typename T>
		void Write(const String& param, const T* pData)
		{
			m_pParams->Write<T>(param, pData);
			m_bRenderStateDirty = true;
		}

		// Note that calling this function marks render state as dirty which invalidates GPU buffer contents.
		// So calling this function may cause unexpected performance loss.
		// If you have intended to only read the value, then use ReadParamValue function, which does not invalidate the buffer contents
		template<typename T>
		T& GetParamForWriting(const String& param)
		{
			MarkRenderStateDirty();
			return m_pParams->GetParam<T>(param);
		}

		template<typename T>
		const T& ReadParamValue(const String& param) const
		{
			// MarkRenderStateDirty();
			return m_pParams->GetParam<T>(param);
		}

		inline bool HasParam(const String& param) const
		{
			return m_pParams->HasParam(param);
		}

		virtual void RT_UploadToGPU(RefPtr<class ICommandListImmediate> pImmContext) = 0;

		inline EShaderStageFlags GetShaderStages() const { return m_pParams->GetShaderStages(); }

		static RefPtr<IParamBuffer> New(RefPtr<PipelineParamMapping> ParamPack);

	protected:
		inline void MarkRenderStateDirty() { m_bRenderStateDirty = true; }
		inline bool IsRenderStateDirty() const { return m_bRenderStateDirty; }
		inline void ResetRenderState() { m_bRenderStateDirty = false; }

		RefPtr<TPipelineParamPack> m_pParams;
	
	private:
		TAtomic<bool> m_bRenderStateDirty = true;
	};
}