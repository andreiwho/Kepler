#pragma once
#include "Core/Core.h"
#include "Renderer/RenderTypes.h"
#include "Image.h"

namespace ke
{
	class ITextureSampler2D : public EnableRefPtrFromThis<ITextureSampler2D>
	{
	protected:
		ITextureSampler2D(RefPtr<IImage2D> pImage) : m_Image(pImage) {}

	public:
		static RefPtr<ITextureSampler2D> New(RefPtr<IImage2D> pImage, u32 mipLevel = 0, u32 arrayLayer = 0);

		inline RefPtr<IImage2D> GetImage() const { return m_Image; }

		virtual void* GetNativeHandle() const = 0;
	
	protected:
		RefPtr<IImage2D> m_Image;
	};
}