#pragma once
#include "Core/Types.h"
#include "Renderer/World/Camera.h"

namespace ke
{
	class CameraComponent
	{
	public:
		CameraComponent(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip);
		~CameraComponent();

		inline MathCamera& GetCamera() { return m_Camera; }
		inline const MathCamera& GetCamera() const { return m_Camera; }

		inline void SetRenderTargetName(const TString& NewName) { m_RenderTargetName = NewName; }
		inline const TString& GetRenderTargetName() const { return m_RenderTargetName; }

	private:
		MathCamera m_Camera;
		TString m_RenderTargetName{ "MeshPassTarget" };
	};
}