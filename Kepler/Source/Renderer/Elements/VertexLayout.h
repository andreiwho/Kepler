#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Core/Containers/DynArray.h"
#include "Core/Malloc.h"

namespace ke
{
	struct VertexAttribute
	{
		VertexAttribute() = default;
		VertexAttribute(const String& name, u32 id, EShaderInputType type, usize offset)
			:	AttributeName(name)
			,	AttributeId(id)
			,	InputType(type)
			,	Offset(offset)
		{}

		String AttributeName{};
		u32 AttributeId{};
		EShaderInputType InputType{EShaderInputType::Float4};
		usize Offset{};
	};

	class VertexLayout
	{
	public:
		VertexLayout() = default;

		VertexLayout(usize stride)
			:	m_Stride(stride)
		{}

		inline void AddAttribute(const String& name, u32 id, EShaderInputType type, usize offset)
		{
			m_Attributes.EmplaceBack(name, id, type, offset);
		}

		inline void AddAttribute(const VertexAttribute& attribute)
		{
			m_Attributes.EmplaceBack(attribute);
		}

		const Array<VertexAttribute>& GetAttributes() const { return m_Attributes; }

	protected:
		Array<VertexAttribute> m_Attributes;
		usize m_Stride{};
	};
}