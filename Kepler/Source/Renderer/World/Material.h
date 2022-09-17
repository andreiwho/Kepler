#pragma once
#include "Renderer/Pipelines/ParamPack.h"
#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "../Elements/ParamBuffer.h"
#include "WorldTransform.h"
#include "Camera.h"

namespace Kepler
{
	class TMaterial : public TEnableRefFromThis<TMaterial>
	{
	public:
		TMaterial() = default;
		TMaterial(TRef<TGraphicsPipeline> InPipeline, const TString& InParentAssetPath);

		void RT_Update(TRef<class TCommandListImmediate> pImmCmd);

		template<typename T>
		void WriteParamData(const TString& Param, const T* Data)
		{
			ParamBuffer->Write<T>(Param, Data);
		}

		// Note that calling this function marks render state as dirty which invalidates GPU buffer contents.
		// So calling this function may cause unexpected performance loss.
		// If you have intended to only read the value, then use ReadParamValue function, which does not invalidate the buffer contents
		template<typename T>
		T& GetParamReferenceForWriting(const TString& Param)
		{
			return ParamBuffer->GetParamForWriting<T>(Param);
		}

		template<typename T>
		const T& ReadParamValue(const TString& Param) const
		{
			return ParamBuffer->ReadParamValue<T>(Param);
		}

		inline TRef<TParamBuffer> GetParamBuffer() const { return ParamBuffer; }

		inline TRef<TPipelineSamplerPack> GetSamplers() const { return Samplers; }

		inline TRef<TGraphicsPipeline> GetPipeline() const { return Pipeline; }

		void WriteSampler(const TString& Name, TRef<TTextureSampler2D> Data);

		static TRef<TMaterial> New()
		{
			return MakeRef(Kepler::New<TMaterial>());
		}

		static TRef<TMaterial> New(TRef<TGraphicsPipeline> InPipeline, const TString& InParentAssetPath)
		{
			return MakeRef(Kepler::New<TMaterial>(InPipeline, InParentAssetPath));
		}

		// Helper functions
		void WriteTransform(TWorldTransform Transform);
		void WriteCamera(TCamera Camera);

		inline const TString& GetParentAssetPath() const { return ParentAssetPath; }

	private:
		TRef<TGraphicsPipeline> Pipeline;
		TRef<TParamBuffer> ParamBuffer;
		TRef<TPipelineSamplerPack> Samplers;
		TString ParentAssetPath{};
	};
}