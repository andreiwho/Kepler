#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Core/Containers/DynArray.h"
#include "Core/Malloc.h"

namespace Kepler
{
	struct TVertexAttribute
	{
		TVertexAttribute(u32 Id, const TString& Name, EShaderInputType Type, usize InOffset)
			:	AttributeId(Id)
			,	AttributeName(Name)
			,	InputType(Type)
			,	Offset(InOffset)
		{}

		u32 AttributeId{};
		TString AttributeName{};
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

		inline void AddAttribute(u32 Id, const TString& Name, EShaderInputType Type, usize Offset)
		{
			Attributes.EmplaceBack(Id, Name, Type, Offset);
		}

		const TDynArray<TVertexAttribute>& GetAttributes() const { return Attributes; }

	protected:
		TDynArray<TVertexAttribute> Attributes;
		usize Stride{};
	};
}