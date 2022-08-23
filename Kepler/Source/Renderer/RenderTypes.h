#pragma once
#include "Core/Malloc.h"
#include "Core/Containers/DynArray.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	enum class ERenderAPI
	{
		Default,	// For now the default one is D3D11 for windows. Other platforms are unsupported

#ifdef WIN32
		DirectX11,
#endif
	};

	//////////////////////////////////////////////////////////////////////////
	enum class EFormat
	{

	};

	//////////////////////////////////////////////////////////////////////////
	struct EShaderStageFlags
	{
		enum Type{
			Vertex = BIT(0),
			Pixel = BIT(1),
			Compute = BIT(2),
		};
		
		u32 Mask{};

		EShaderStageFlags() = default;
		EShaderStageFlags(u32 InMask) : Mask(InMask) {}
		EShaderStageFlags& operator|=(EShaderStageFlags Flags)
		{
			Mask |= Flags.Mask;
			return *this;
		}

		inline TDynArray<EShaderStageFlags::Type> Separate() const
		{
			TDynArray<EShaderStageFlags::Type> OutFlags = 0;

			auto EmplaceIf = [&](EShaderStageFlags::Type CheckMask)
			{
				if (Mask & CheckMask)
				{
					OutFlags.EmplaceBack(CheckMask);
				}
			};

			EmplaceIf(EShaderStageFlags::Vertex);
			EmplaceIf(EShaderStageFlags::Pixel);
			EmplaceIf(EShaderStageFlags::Compute);

			return OutFlags;
		}

		static std::string ToString(Type Value)
		{
			return std::invoke([Value] 
			{
					switch (Value)
					{
					case Kepler::EShaderStageFlags::Vertex:
						return "Vertex";
					case Kepler::EShaderStageFlags::Pixel:
						return "Pixel";
					case Kepler::EShaderStageFlags::Compute:
						return "Compute";
					default:
						break;
					}
					return "Unknown";
			});
		}

		inline operator u32() const { return Mask; }
	};


	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	struct EBufferAccessFlags
	{
		enum
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

	//////////////////////////////////////////////////////////////////////////
	struct ERenderPassId
	{
		enum
		{
			Geometry = BIT(0),
		};

		u32 Mask{};

		ERenderPassId() = default;
		ERenderPassId(u32 InMask) : Mask(InMask) {}
		inline operator u32() const { return Mask; }

		inline ERenderPassId& operator|=(ERenderPassId InId)
		{
			Mask |= InId.Mask;
			return *this;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	enum class EPrimitiveFillMode
	{
		Wireframe,
		Solid,
	};

	//////////////////////////////////////////////////////////////////////////
	enum class EPrimitiveCullMode
	{
		None,
		Front,
		Back,
	};

	//////////////////////////////////////////////////////////////////////////
	struct EDepthBufferAccess
	{
		enum
		{
			None = 0,
			Read = BIT(0),
			Write = BIT(1),
		};

		u32 Mask{};

		EDepthBufferAccess() = default;
		EDepthBufferAccess(u32 InMask) : Mask(InMask) {}
		inline operator u32() const { return Mask; }

		inline EDepthBufferAccess& operator|=(EDepthBufferAccess InId)
		{
			Mask |= InId.Mask;
			return *this;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct EStencilBufferAccess
	{
		enum
		{
			None = 0,
			Read = BIT(0),
			Write = BIT(1),
		};

		u32 Mask{};

		EStencilBufferAccess() = default;
		EStencilBufferAccess(u32 InMask) : Mask(InMask) {}
		inline operator u32() const { return Mask; }

		inline EStencilBufferAccess& operator|=(EStencilBufferAccess InId)
		{
			Mask |= InId.Mask;
			return *this;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct EShaderInputType
	{
		enum EValue : u32
		{
			Float = 0,
			Float2,
			Float3,
			Float4,

			Int,
			Int2,
			Int3,
			Int4,

			UInt,
			UInt2,
			UInt3,
			UInt4,

			Custom,
		} Value;

		EShaderInputType(EValue InValue) :Value(InValue) {}
		inline operator u32() const { return Value; }
	};

	//////////////////////////////////////////////////////////////////////////
	enum class EPrimitiveTopology
	{
		TriangleList,
		LineList,
		PointList,
	};
}