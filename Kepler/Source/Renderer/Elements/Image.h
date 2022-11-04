#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	class IImage : public EnableRefPtrFromThis<IImage>
	{
	public:
		IImage(EFormat format, EImageUsage usage, u32 mipLevels = 1, u32 arraySize = 1);

		inline EFormat GetFormat() const { return m_Format; }
		inline EImageUsage GetUsage() const { return m_Usage; }

		inline bool IsShaderResource() const { return m_Usage & EImageUsage::ShaderResource; }
		inline bool IsRenderTarget() const { return m_Usage & EImageUsage::RenderTarget; }
		inline bool IsDepthTarget() const { return m_Usage & EImageUsage::DepthTarget; }
		inline bool IsArray() const { return m_ArraySize > 1; }
		inline u32 GetArraySize() const { return m_ArraySize; }
		inline bool HasMultipleMips() const { return m_MipLevels > 1; }
		inline u32 GetMipLevels() const { return m_MipLevels; }

	protected:
		EFormat m_Format{};
		EImageUsage m_Usage{};
		u32 m_MipLevels;
		u32 m_ArraySize{};
	};

	//////////////////////////////////////////////////////////////////////////
	class IImage2D : public IImage
	{
	protected:
		IImage2D(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mipLevels = 1, u32 arraySize = 1);

	public:
		inline u32 GetWidth() const { return m_Width; }
		inline u32 GetHeight() const { return m_Height; }
		
		static RefPtr<IImage2D> New(u32 width, u32 height, EFormat format, EImageUsage usage, u32 mipLevels = 1, u32 arraySize = 1);
		void Write(RefPtr<class ICommandListImmediate> pImmCmd, usize x, usize y, usize width, usize height, RefPtr<IAsyncDataBlob> pData);

	protected:
		u32 m_Width{};
		u32 m_Height{};
	};
}