#include "ParamBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace ke
{
	TParamBuffer::TParamBuffer(RefPtr<TPipelineParamMapping> Mapping)
		:	Params(Mapping->CreateParamPack())
	{
	}

	RefPtr<TParamBuffer> TParamBuffer::New(RefPtr<TPipelineParamMapping> ParamPack)
	{
		return GetRenderDevice()->CreateParamBuffer(ParamPack);
	}

}