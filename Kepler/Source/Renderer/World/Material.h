#pragma once
#include "Renderer/Pipelines/ParamPack.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "../Elements/ParamBuffer.h"
#include "WorldTransform.h"
#include "Camera.h"

namespace ke
{
	class TMaterial : public EnableRefPtrFromThis<TMaterial>
	{
	public:
		TMaterial() = default;
		TMaterial(RefPtr<IGraphicsPipeline> pipeline, const String& parentAssetPath);

		void RT_Update(RefPtr<class ICommandListImmediate> pImmCmd);

		template<typename T>
		void WriteParamData(const String& param, const T* pData)
		{
			m_ParamBuffer->Write<T>(param, pData);
		}

		// Note that calling this function marks render state as dirty which invalidates GPU buffer contents.
		// So calling this function may cause unexpected performance loss.
		// If you have intended to only read the value, then use ReadParamValue function, which does not invalidate the buffer contents
		template<typename T>
		T& GetParamReferenceForWriting(const String& param)
		{
			return m_ParamBuffer->GetParamForWriting<T>(param);
		}

		template<typename T>
		const T& ReadParamValue(const String& param) const
		{
			return m_ParamBuffer->ReadParamValue<T>(param);
		}

		inline RefPtr<IParamBuffer> GetParamBuffer() const { return m_ParamBuffer; }

		inline RefPtr<PipelineSamplerPack> GetSamplers() const { return m_Samplers; }

		inline RefPtr<IGraphicsPipeline> GetPipeline() const { return m_Pipeline; }

		void WriteSampler(const String& Name, RefPtr<ITextureSampler2D> Data);

		static RefPtr<TMaterial> New()
		{
			return MakeRef(ke::New<TMaterial>());
		}

		static RefPtr<TMaterial> New(RefPtr<IGraphicsPipeline> pPipeline, const String& parentAssetPath)
		{
			return MakeRef(ke::New<TMaterial>(pPipeline, parentAssetPath));
		}

		inline bool HasParam(const String& name) const { return m_ParamBuffer->HasParam(name); }

		// Helper functions
		void WriteTransform(WorldTransform transform);
		void WriteCamera(MathCamera camera);
		void WriteId(i32 id);

		inline const String& GetParentAssetPath() const { return m_ParentAssetPath; }
		inline bool UsesPrepass() const { return m_Pipeline->UsesPrepass(); }

	private:
		RefPtr<IGraphicsPipeline> m_Pipeline;
		RefPtr<IParamBuffer> m_ParamBuffer;
		RefPtr<PipelineSamplerPack> m_Samplers;
		String m_ParentAssetPath{};
	};
}