#pragma once
#include "Core/Malloc.h"
#include "Core/Types.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{


	class TVertexBuffer : public TRefCounted
	{
	public:
		TVertexBuffer() = default;
		TVertexBuffer(EBufferAccessFlags InAccessFlags, TRef<TDataBlob> Data = nullptr);

	protected:
		EBufferAccessFlags AccessFlags{};
		usize Size{};
		usize Stride{};
	};
}