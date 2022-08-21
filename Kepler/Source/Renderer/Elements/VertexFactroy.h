#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	struct TVertexAttribute
	{
		TVertexAttribute(u32 Id, const std::string& Name, EShaderInputType Type, usize InOffset)
			:	AttributeId(Id)
			,	AttributeName(Name)
			,	InputType(Type)
			,	Offset(InOffset)
		{}

		u32 AttributeId{};
		std::string AttributeName{};
		EShaderInputType InputType{EShaderInputType::Float4};
		usize Offset{};
	};

	class TVertexFactory
	{
	public:
	private:
	};
}