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
	TRef<TRenderTarget2D> TRenderTargetGroup::GetRenderTargetAtArrayLayer(u32 idx) const
	{
		CHECK(idx < m_ArrayLayers);
		return m_RenderTargets[idx];
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TTextureSampler2D> TRenderTargetGroup::GetTextureSamplerAtArrayLayer(u32 idx) const
	{
		CHECK(idx < m_ArrayLayers);
		return m_TextureSamplers[idx];
	}

	//////////////////////////////////////////////////////////////////////////
	void TRenderTargetGroup::Resize(u32 width, u32 height, EFormat format, bool bAllowCPURead)
	{
		if (m_Width != width || m_Height != height || m_Format.Value != format)
		{
			m_Width = width > 0 ? width : 1;
			m_Height = height > 0 ? height : 1;
			m_Format = format;

			TRef<TImage2D> pTargetImage = TImage2D::New(m_Width, m_Height, format, 
				EImageUsage::RenderTarget 
				| (bAllowCPURead ? EImageUsage::AllowCPURead : EImageUsage::ShaderResource), 
				1, 
				m_ArrayLayers);
			for (u32 idx = 0; idx < m_ArrayLayers; ++idx)
			{
				m_RenderTargets[idx] = TRenderTarget2D::New(pTargetImage, 0, idx);
				if (!bAllowCPURead)
				{
					m_TextureSamplers[idx] = TTextureSampler2D::New(pTargetImage, 0, idx);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TRenderTargetGroup::New(u32 width, u32 height, EFormat format, u32 layers, bool bAllowCPURead)
	{
		return MakeRef(ke::New<TRenderTargetGroup>(width, height, format, layers));
	}

	//////////////////////////////////////////////////////////////////////////
	// RENDER TARGET REGISTRY
	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry* TTargetRegistry::Instance = nullptr;

	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry::TTargetRegistry()
	{
		Instance = this;
	}

	//////////////////////////////////////////////////////////////////////////
	TTargetRegistry::~TTargetRegistry()
	{
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TRenderTargetGroup> TTargetRegistry::GetRenderTargetGroup(const TString& name, u32 width, u32 height, EFormat format, u32 layers, bool bAllowCPURead)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_RenderTargets.Contains(name))
		{
			CHECK(width != UINT32_MAX && height != UINT32_MAX && format.Value != EFormat::Unknown && layers > 0);
			m_RenderTargets[name] = TRenderTargetGroup::New(width, height, format, layers);
		}

		// This may cause a performance problem, or may not. Needs to be checked
		TRef<TRenderTargetGroup> pTarget = m_RenderTargets[name];
		if (width != UINT32_MAX || height != UINT32_MAX || format != EFormat::Unknown)
		{
			pTarget->Resize(width, height, format, bAllowCPURead);
		}
		return pTarget;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TTargetRegistry::RenderTargetGroupExists(const TString& name) const
	{
		return m_RenderTargets.Contains(name);
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TDepthStencilTarget2D> TTargetRegistry::GetDepthTarget(const TString& name, u32 width, u32 height, EFormat format, bool bSampled)
	{
		// Create
		auto newWidth = width > 0 ? width : 1;
		auto newHeight = height > 0 ? height : 1;
		if (!m_DepthTargets.Contains(name))
		{
			CHECK(width != UINT32_MAX && height != UINT32_MAX && format.Value != EFormat::Unknown);
			EImageUsage Flags = EImageUsage::DepthTarget;
			if (bSampled)
			{
				Flags.Mask |= EImageUsage::ShaderResource;
			}

			TRef<TImage2D> DepthImage = TImage2D::New(newWidth, newHeight, format, Flags);
			m_DepthTargets[name] = TDepthStencilTarget2D::New(DepthImage);
		}

		// Acquire | Resize
		TRef<TDepthStencilTarget2D> depthTarget = m_DepthTargets[name];
		if (newWidth != UINT32_MAX || newHeight != UINT32_MAX)
		{
			if (depthTarget->GetWidth() != newWidth || depthTarget->GetHeight() != newHeight)
			{
				TRef<TImage2D> depthImage = TImage2D::New(newWidth, newHeight, depthTarget->GetFormat(), depthTarget->GetImage()->GetUsage());
				depthTarget = TDepthStencilTarget2D::New(depthImage);
				m_DepthTargets[name] = depthTarget;
			}
		}
		return depthTarget;
	}

}