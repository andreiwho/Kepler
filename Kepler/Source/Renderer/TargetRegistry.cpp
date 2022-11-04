#include "TargetRegistry.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET GROUP
	//////////////////////////////////////////////////////////////////////////
	TRenderTargetGroup::TRenderTargetGroup(u32 width, u32 height, EFormat format, u32 layers, bool bAllowCPURead)
		:	m_Width(0)
		,	m_Height(0)
		,	m_Format(format)
		,	m_ArrayLayers(layers)
	{
		m_RenderTargets.Resize(m_ArrayLayers);
		if (!bAllowCPURead)
		{
			m_TextureSamplers.Resize(m_ArrayLayers);
		}
		Resize(width, height, format, bAllowCPURead);
	}

	//////////////////////////////////////////////////////////////////////////
	TRenderTargetGroup::~TRenderTargetGroup()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IRenderTarget2D> TRenderTargetGroup::GetRenderTargetAtArrayLayer(u32 idx) const
	{
		CHECK(idx < m_ArrayLayers);
		return m_RenderTargets[idx];
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ITextureSampler2D> TRenderTargetGroup::GetTextureSamplerAtArrayLayer(u32 idx) const
	{
		CHECK(idx < m_ArrayLayers);
		return m_TextureSamplers[idx];
	}

	//////////////////////////////////////////////////////////////////////////
	void TRenderTargetGroup::Resize(u32 width, u32 height, EFormat format, bool bAllowCPURead)
	{
		if (m_Width != width || m_Height != height || m_Format != format)
		{
			m_Width = width > 0 ? width : 1;
			m_Height = height > 0 ? height : 1;
			m_Format = format;

			RefPtr<IImage2D> pTargetImage = IImage2D::New(m_Width, m_Height, format, 
				EImageUsage::RenderTarget 
				| (bAllowCPURead ? EImageUsage::AllowCPURead : EImageUsage::ShaderResource), 
				1, 
				m_ArrayLayers);
			for (u32 idx = 0; idx < m_ArrayLayers; ++idx)
			{
				m_RenderTargets[idx] = IRenderTarget2D::New(pTargetImage, 0, idx);
				if (!bAllowCPURead)
				{
					m_TextureSamplers[idx] = ITextureSampler2D::New(pTargetImage, 0, idx);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<TRenderTargetGroup> TRenderTargetGroup::New(u32 width, u32 height, EFormat format, u32 layers, bool bAllowCPURead)
	{
		return MakeRef(ke::New<TRenderTargetGroup>(width, height, format, layers));
	}

	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET REGISTRY
	//////////////////////////////////////////////////////////////////////////
	RenderTargetRegistry* RenderTargetRegistry::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	RenderTargetRegistry::RenderTargetRegistry()
	{
		Instance = this;
	}

	//////////////////////////////////////////////////////////////////////////
	RenderTargetRegistry::~RenderTargetRegistry()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<TRenderTargetGroup> RenderTargetRegistry::GetRenderTargetGroup(const String& name, u32 width, u32 height, EFormat format, u32 layers, bool bAllowCPURead)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_RenderTargets.Contains(name))
		{
			CHECK(width != UINT32_MAX && height != UINT32_MAX && format != EFormat::Unknown && layers > 0);
			m_RenderTargets[name] = TRenderTargetGroup::New(width, height, format, layers);
		}

		// This may cause a performance problem, or may not. Needs to be checked
		RefPtr<TRenderTargetGroup> pTarget = m_RenderTargets[name];
		if (width != UINT32_MAX || height != UINT32_MAX || format != EFormat::Unknown)
		{
			pTarget->Resize(width, height, format, bAllowCPURead);
		}
		return pTarget;
	}

	//////////////////////////////////////////////////////////////////////////
	bool RenderTargetRegistry::RenderTargetGroupExists(const String& name) const
	{
		return m_RenderTargets.Contains(name);
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<IDepthStencilTarget2D> RenderTargetRegistry::GetDepthTarget(const String& name, u32 width, u32 height, EFormat format, bool bSampled)
	{
		// Create
		auto newWidth = width > 0 ? width : 1;
		auto newHeight = height > 0 ? height : 1;
		if (!m_DepthTargets.Contains(name))
		{
			CHECK(width != UINT32_MAX && height != UINT32_MAX && format != EFormat::Unknown);
			EImageUsage Flags = EImageUsage::DepthTarget;
			if (bSampled)
			{
				Flags.Mask |= EImageUsage::ShaderResource;
			}

			RefPtr<IImage2D> DepthImage = IImage2D::New(newWidth, newHeight, format, Flags);
			m_DepthTargets[name] = IDepthStencilTarget2D::New(DepthImage);
		}

		// Acquire | Resize
		RefPtr<IDepthStencilTarget2D> depthTarget = m_DepthTargets[name];
		if (newWidth != UINT32_MAX || newHeight != UINT32_MAX)
		{
			if (depthTarget->GetWidth() != newWidth || depthTarget->GetHeight() != newHeight)
			{
				RefPtr<IImage2D> depthImage = IImage2D::New(newWidth, newHeight, depthTarget->GetFormat(), depthTarget->GetImage()->GetUsage());
				depthTarget = IDepthStencilTarget2D::New(depthImage);
				m_DepthTargets[name] = depthTarget;

				if (m_ReadOnlyDepthTargets.Contains(name))
				{
					m_ReadOnlyDepthTargets.Remove(m_ReadOnlyDepthTargets.FindIterator(name));
				}
			}
		}
		return depthTarget;
	}

	RefPtr<IDepthStencilTarget2D> RenderTargetRegistry::GetReadOnlyDepthTarget(const String& name)
	{
		if (m_ReadOnlyDepthTargets.Contains(name))
		{
			return m_ReadOnlyDepthTargets[name];
		}

		CHECK(m_DepthTargets.Contains(name));
		RefPtr<IImage2D> pTargetImage = CHECKED(m_DepthTargets[name]->GetImage());
		RefPtr<IDepthStencilTarget2D> pOutTarget = IDepthStencilTarget2D::NewReadOnly(pTargetImage);
		m_ReadOnlyDepthTargets[name] = pOutTarget;
		return pOutTarget;
	}

}