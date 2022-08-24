#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	class TImage : public TEnableRefFromThis<TImage>
	{
	public:
		TImage(EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);

		inline EFormat GetFormat() const { return Format; }
		inline EImageUsage GetUsage() const { return Usage; }

		inline bool IsShaderResource() const { return Usage & EImageUsage::ShaderResource; }
		inline bool IsRenderTarget() const { return Usage & EImageUsage::RenderTarget; }
		inline bool IsDepthTarget() const { return Usage & EImageUsage::DepthTarget; }
		inline bool IsArray() const { return ArraySize > 1; }
		inline u32 GetArraySize() const { return ArraySize; }
		inline bool HasMultipleMips() const { return MipLevels > 1; }
		inline u32 GetMipLevels() const { return MipLevels; }

	protected:
		EFormat Format{};
		EImageUsage Usage{};
		u32 MipLevels;
		u32 ArraySize{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TImage1D : public TImage
	{
	public:
		TImage1D(u32 InWidth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);

		inline u32 GetWidth() const { return Width; }

	protected:
		u32 Width{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TImage2D : public TImage
	{
	public:
		TImage2D(u32 InWidth, u32 InHeight, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);

		inline u32 GetWidth() const { return Width; }
		inline u32 GetHeight() const { return Height; }

	protected:
		u32 Width{};
		u32 Height{};
	};

	//////////////////////////////////////////////////////////////////////////
	class TImage3D : public TImage
	{
	public:
		TImage3D(u32 InWidth, u32 InHeight, u32 InDepth, EFormat InFormat, EImageUsage InUsage, u32 InMipLevels = 1, u32 InArraySize = 1);

		inline u32 GetWidth() const { return Width; }
		inline u32 GetHeight() const { return Height; }
		inline u32 GetDepth() const { return Depth; }

	protected:
		u32 Width{};
		u32 Height{};
		u32 Depth{};
	};

	// TODO:
	// Add class for cube textures
}