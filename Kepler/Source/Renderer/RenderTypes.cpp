#include "RenderTypes.h"

namespace Kepler
{
	usize EShaderInputType::GetValueSize() const
	{
		switch (Value)
		{
		case Kepler::EShaderInputType::Float:
			return sizeof(float);
		case Kepler::EShaderInputType::Float2:
			return sizeof(float2);
		case Kepler::EShaderInputType::Float3:
			return sizeof(float3);
		case Kepler::EShaderInputType::Float4:
			return sizeof(float4);
		case Kepler::EShaderInputType::Int:
			return sizeof(i32);
		case Kepler::EShaderInputType::Int2:
			return sizeof(int2);
		case Kepler::EShaderInputType::Int3:
			return sizeof(int3);
		case Kepler::EShaderInputType::Int4:
			return sizeof(int4);
		case Kepler::EShaderInputType::UInt:
			return sizeof(u32);
		case Kepler::EShaderInputType::UInt2:
			return sizeof(uint2);
		case Kepler::EShaderInputType::UInt3:
			return sizeof(uint3);
		case Kepler::EShaderInputType::UInt4:
			return sizeof(uint4);
		case Kepler::EShaderInputType::Custom:
			return 0;
		default:
			CRASH();
		}
		CRASH();
	}

}