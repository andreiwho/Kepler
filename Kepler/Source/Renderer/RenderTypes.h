#pragma once
#include "Core/Malloc.h"
#include "Core/Containers/DynArray.h"

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
		static TRef<TDataBlob> CreateGraphicsDataBlob(const void* Data = nullptr, usize Size = 0, usize ElemSize = 0);
		
		template<typename T>
		static TRef<TDataBlob> CreateGraphicsDataBlob(const TDynArray<T>& Data)
		{
			return CreateGraphicsDataBlob(Data.GetData(), Data.GetLength() * sizeof(T), sizeof(T));
		}

		virtual const void* GetData() const = 0;
		virtual usize GetSize() const = 0;
		virtual usize GetStride() const = 0;
		virtual void Write(const void* Data, usize Size) = 0;
	};

	struct EBufferAccessFlags
	{
		enum EValues
		{
			GPUOnly = BIT(0),
			ReadAccess = BIT(1),
			WriteAccess = BIT(2),
		};

		u32 Mask{};

		EBufferAccessFlags() = default;
		EBufferAccessFlags(u32 InMask) : Mask(InMask) {}
		inline operator u32() const { return Mask; }
	};
}