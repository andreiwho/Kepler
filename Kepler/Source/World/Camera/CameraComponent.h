#pragma once
#include "Core/Types.h"
#include "Renderer/World/Camera.h"

namespace ke
{
	class TCameraComponent
	{
	public:
		TCameraComponent(float InFOVDegrees, u32 InWidth, u32 InHeight, float InNearClip, float InFarClip);
		~TCameraComponent();

		inline TCamera& GetCamera() { return Camera; }
		inline const TCamera& GetCamera() const { return Camera; }

		inline void SetRenderTargetName(const TString& NewName) { RenderTargetName = NewName; }
		inline const TString& GetRenderTargetName() const { return RenderTargetName; }

	private:
		TCamera Camera;
		TString RenderTargetName{ "MeshPassTarget" };
	};
}