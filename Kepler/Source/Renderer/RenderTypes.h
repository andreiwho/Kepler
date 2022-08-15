#pragma once
#include "Core/Malloc.h"

namespace Kepler
{
	enum class ERenderAPI
	{
		Default,	// For now the default one is D3D11 for windows. Other platforms are unsupported

#ifdef WIN32
		DirectX11,
#endif
	};

	enum class EFormat
	{

	};

	enum class EShaderType
	{
		Vertex,
		Pixel,
	};

	class TDataBlob : public TRefCounted
	{
	public:
		static TRef<TDataBlob> CreateGraphicsDataBlob(const void* Data = nullptr, usize Size = 0);

		virtual const void* GetData() const = 0;
		virtual usize GetSize() const = 0;
		virtual void Write(const void* Data, usize Size) = 0;
	};
}