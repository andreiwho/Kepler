#include "ParamBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace Kepler
{
	TParamBuffer::TParamBuffer(TRef<TPipelineParamMapping> Mapping)
		:	Params(Mapping->CreateParamPack())
	{
	}

	TRef<TParamBuffer> TParamBuffer::New(TRef<TPipelineParamMapping> ParamPack)
	{
		return GetRenderDevice()->CreateParamBuffer(ParamPack);
	}

}