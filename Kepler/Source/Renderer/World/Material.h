#pragma once
#include "Renderer/Pipelines/ParamPack.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "../Elements/ParamBuffer.h"
#include "WorldTransform.h"
#include "Camera.h"

namespace ke
{
	class TMaterial : public TEnableRefFromThis<TMaterial>
	{
	public:
		TMaterial() = default;
		TMaterial(RefPtr<TGraphicsPipeline> pipeline, const TString& parentAssetPath);

		void RT_Update(RefPtr<class GraphicsCommandListImmediate> pImmCmd);

		template<typename T>
		void WriteParamData(const TString& param, const T* pData)
		{
			m_ParamBuffer->Write<T>(param, pData);
		}

		// Note that calling this function marks render state as dirty which invalidates GPU buffer contents.
		// So calling this function may cause unexpected performance loss.
		// If you have intended to only read the value, then use ReadParamValue function, which does not invalidate the buffer contents
		template<typename T>
		T& GetParamReferenceForWriting(const TString& param)
		{
			return m_ParamBuffer->GetParamForWriting<T>(param);
		}

		template<typename T>
		const T& ReadParamValue(const TString& param) const
		{
			return m_ParamBuffer->ReadParamValue<T>(param);
		}

		inline RefPtr<TParamBuffer> GetParamBuffer() const { return m_ParamBuffer; }

		inline RefPtr<TPipelineSamplerPack> GetSamplers() const { return m_Samplers; }

		inline RefPtr<TGraphicsPipeline> GetPipeline() const { return m_Pipeline; }

		void WriteSampler(const TString& Name, RefPtr<TTextureSampler2D> Data);

		static RefPtr<TMaterial> New()
		{
			return MakeRef(ke::New<TMaterial>());
		}

		static RefPtr<TMaterial> New(RefPtr<TGraphicsPipeline> pPipeline, const TString& parentAssetPath)
		{
			return MakeRef(ke::New<TMaterial>(pPipeline, parentAssetPath));
		}

		inline bool HasParam(const TString& name) const { return m_ParamBuffer->HasParam(name); }

		// Helper functions
		void WriteTransform(TWorldTransform transform);
		void WriteCamera(MathCamera camera);
		void WriteId(i32 id);

		inline const TString& GetParentAssetPath() const { return m_ParentAssetPath; }
		inline bool UsesPrepass() const { return m_Pipeline->UsesPrepass(); }

	private:
		RefPtr<TGraphicsPipeline> m_Pipeline;
		RefPtr<TParamBuffer> m_ParamBuffer;
		RefPtr<TPipelineSamplerPack> m_Samplers;
		TString m_ParentAssetPath{};
	};
}