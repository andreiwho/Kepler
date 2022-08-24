#include "ParamBuffer.h"

namespace Kepler
{
	TParamBuffer::TParamBuffer(TRef<TPipelineParamMapping> Mapping)
		:	Params(Mapping->CreatePack())
	{
	}
}