#include "ParamBuffer.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"

namespace ke
{
	IParamBuffer::IParamBuffer(RefPtr<PipelineParamMapping> pMapping)
		:	m_pParams(pMapping->CreateParamPack())
	{
	}

	RefPtr<IParamBuffer> IParamBuffer::New(RefPtr<PipelineParamMapping> pParamPack)
	{
		return GetRenderDevice()->CreateParamBuffer(pParamPack);
	}

}