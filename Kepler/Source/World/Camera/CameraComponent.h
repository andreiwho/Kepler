#pragma once
#include "Core/Types.h"
#include "Renderer/World/Camera.h"
#include "World/Game/Components/EntityComponent.h"
#include "CameraComponent.gen.h"

namespace ke
{
	reflected class CameraComponent : public EntityComponent
	{
	public:
		CameraComponent(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip);
		~CameraComponent();

		inline MathCamera& GetCamera() { return m_Camera; }
		inline const MathCamera& GetCamera() const { return m_Camera; }

		inline void SetRenderTargetName(const String& NewName) { m_RenderTargetName = NewName; }
		inline const String& GetRenderTargetName() const { return m_RenderTargetName; }

	public:
		reflected MathCamera m_Camera;
		reflected String m_RenderTargetName{ "MeshPassTarget" };
	};
}