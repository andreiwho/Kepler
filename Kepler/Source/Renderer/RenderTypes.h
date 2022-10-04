#pragma once
#include "Core/Malloc.h"
#include "Core/Containers/DynArray.h"

namespace ke
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

	struct EFormat
	{
		// These values are copied from DXGI_FORMAT, for other render apis except d3d11/12 should create a translator function
		enum Type
		{
			Unknown = 0,
			R32G32B32A32_TYPELESS = 1,
			R32G32B32A32_FLOAT = 2,
			R32G32B32A32_UINT = 3,
			R32G32B32A32_SINT = 4,
			R32G32B32_TYPELESS = 5,
			R32G32B32_FLOAT = 6,
			R32G32B32_UINT = 7,
			R32G32B32_SINT = 8,
			R16G16B16A16_TYPELESS = 9,
			R16G16B16A16_FLOAT = 10,
			R16G16B16A16_UNORM = 11,
			R16G16B16A16_UINT = 12,
			R16G16B16A16_SNORM = 13,
			R16G16B16A16_SINT = 14,
			R32G32_TYPELESS = 15,
			R32G32_FLOAT = 16,
			R32G32_UINT = 17,
			R32G32_SINT = 18,
			R32G8X24_TYPELESS = 19,
			D32_FLOAT_S8X24_UINT = 20,
			R32_FLOAT_X8X24_TYPELESS = 21,
			X32_TYPELESS_G8X24_UINT = 22,
			R10G10B10A2_TYPELESS = 23,
			R10G10B10A2_UNORM = 24,
			R10G10B10A2_UINT = 25,
			R11G11B10_FLOAT = 26,
			R8G8B8A8_TYPELESS = 27,
			R8G8B8A8_UNORM = 28,
			R8G8B8A8_UNORM_SRGB = 29,
			R8G8B8A8_UINT = 30,
			R8G8B8A8_SNORM = 31,
			R8G8B8A8_SINT = 32,
			R16G16_TYPELESS = 33,
			R16G16_FLOAT = 34,
			R16G16_UNORM = 35,
			R16G16_UINT = 36,
			R16G16_SNORM = 37,
			R16G16_SINT = 38,
			R32_TYPELESS = 39,
			D32_FLOAT = 40,
			R32_FLOAT = 41,
			R32_UINT = 42,
			R32_SINT = 43,
			R24G8_TYPELESS = 44,
			D24_UNORM_S8_UINT = 45,
			R24_UNORM_X8_TYPELESS = 46,
			X24_TYPELESS_G8_UINT = 47,
			R8G8_TYPELESS = 48,
			R8G8_UNORM = 49,
			R8G8_UINT = 50,
			R8G8_SNORM = 51,
			R8G8_SINT = 52,
			R16_TYPELESS = 53,
			R16_FLOAT = 54,
			D16_UNORM = 55,
			R16_UNORM = 56,
			R16_UINT = 57,
			R16_SNORM = 58,
			R16_SINT = 59,
			R8_TYPELESS = 60,
			R8_UNORM = 61,
			R8_UINT = 62,
			R8_SNORM = 63,
			R8_SINT = 64,
			A8_UNORM = 65,
			R1_UNORM = 66,
		} Value{ Unknown };

		EFormat() = default;
		EFormat(EFormat::Type InValue) : Value(InValue) {}
		inline operator u32() const { return Value; }
	};

	//////////////////////////////////////////////////////////////////////////
	struct EShaderStageFlags
	{
		enum Type {
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

		inline Array<EShaderStageFlags::Type> Separate() const
		{
			Array<EShaderStageFlags::Type> OutFlags = 0;

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

		static TString ToString(Type Value)
		{
			return std::invoke([Value]
				{
					switch (Value)
					{
					case ke::EShaderStageFlags::Vertex:
						return "Vertex";
					case ke::EShaderStageFlags::Pixel:
						return "Pixel";
					case ke::EShaderStageFlags::Compute:
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
	class AsyncDataBlob : public IntrusiveRefCounted
	{
	public:
		static TRef<AsyncDataBlob> CreateGraphicsDataBlob(const void* pData = nullptr, usize size = 0, usize stride = 0);

		template<typename T>
		static TRef<AsyncDataBlob> New(const Array<T>& data)
		{
			return CreateGraphicsDataBlob(data.GetData(), data.GetLength() * sizeof(T), sizeof(T));
		}

		virtual const void* GetData() const = 0;
		virtual usize GetSize() const = 0;
		virtual usize GetStride() const = 0;
		virtual void Write(const void* pData, usize size) = 0;
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

			Matrix3x3,
			Matrix3x4,
			Matrix4x3,
			Matrix4x4,

			Sampler2D,
			Texture2D,

			Custom,
		} Value;

		EShaderInputType(EValue InValue) :Value(InValue) {}
		inline operator u32() const { return Value; }

		usize GetValueSize() const;
	};

	//////////////////////////////////////////////////////////////////////////
	enum class EPrimitiveTopology
	{
		TriangleList,
		LineList,
		PointList,
	};

	//////////////////////////////////////////////////////////////////////////
	struct EImageUsage
	{
		enum Type
		{
			Undefined = 0,
			ShaderResource = BIT(0),
			RenderTarget = BIT(1),
			DepthTarget = BIT(2),
			AllowCPURead = BIT(3),
			AllowCPUWrite = BIT(4)
		};

		u32 Mask{Undefined};
		EImageUsage() = default;
		EImageUsage(u32 InMask) : Mask(InMask) {}

		inline operator u32() const { return Mask; }
	};

	//////////////////////////////////////////////////////////////////////////

}