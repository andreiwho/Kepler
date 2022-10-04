#include "RenderTypes.h"

namespace ke
{
	usize EShaderInputType::GetValueSize() const
	{
		switch (Value)
		{
		case ke::EShaderInputType::Float:
			return sizeof(float);
		case ke::EShaderInputType::Float2:
			return sizeof(float2);
		case ke::EShaderInputType::Float3:
			return sizeof(float3);
		case ke::EShaderInputType::Float4:
			return sizeof(float4);
		case ke::EShaderInputType::Int:
			return sizeof(i32);
		case ke::EShaderInputType::Int2:
			return sizeof(int2);
		case ke::EShaderInputType::Int3:
			return sizeof(int3);
		case ke::EShaderInputType::Int4:
			return sizeof(int4);
		case ke::EShaderInputType::UInt:
			return sizeof(u32);
		case ke::EShaderInputType::UInt2:
			return sizeof(uint2);
		case ke::EShaderInputType::UInt3:
			return sizeof(uint3);
		case ke::EShaderInputType::UInt4:
			return sizeof(uint4);
		case ke::EShaderInputType::Matrix3x3:
			return sizeof(matrix3x3);
		case ke::EShaderInputType::Matrix3x4:
			return sizeof(matrix3x4);
		case ke::EShaderInputType::Matrix4x3:
			return sizeof(matrix4x3);
		case ke::EShaderInputType::Matrix4x4:
			return sizeof(matrix4x4);
		case ke::EShaderInputType::Custom:
			return 0;
		default:
			CRASH();
		}
		CRASH();
	}

}