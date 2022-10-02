#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Core/Containers/DynArray.h"
#include "Core/Malloc.h"

namespace ke
{
	struct TVertexAttribute
	{
		TVertexAttribute() = default;
		TVertexAttribute(const TString& Name, u32 Id, EShaderInputType Type, usize InOffset)
			:	AttributeName(Name)
			,	AttributeId(Id)
			,	InputType(Type)
			,	Offset(InOffset)
		{}

		TString AttributeName{};
		u32 AttributeId{};
		EShaderInputType InputType{EShaderInputType::Float4};
		usize Offset{};
	};

	class TVertexLayout
	{
	public:
		TVertexLayout() = default;

		TVertexLayout(usize InStride)
			:	Stride(InStride)
		{}

		inline void AddAttribute(const TString& Name, u32 Id, EShaderInputType Type, usize Offset)
		{
			Attributes.EmplaceBack(Name, Id, Type, Offset);
		}

		inline void AddAttribute(const TVertexAttribute& Attribute)
		{
			Attributes.EmplaceBack(Attribute);
		}

		const TDynArray<TVertexAttribute>& GetAttributes() const { return Attributes; }

	protected:
		TDynArray<TVertexAttribute> Attributes;
		usize Stride{};
	};
}