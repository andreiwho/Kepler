#pragma once
#include "Core/Core.h"
#include "Renderer/Elements/CommandList.h"
#include "../World/Camera.h"

namespace ke
{
	/**
	 *  ISubrenderer interface.
	 * All children of this class will be responsible for rendering certain rendering things, that cannot be a part of the world renderer
	 * This can be UI subrenderer or LineBatch subrenderer
	 * ISubrenderer will also be the parent class of the Subrenderer2D, which will be used as the main renderer for 2D objects, lines and points
	 * All instances of this class are not ref counted, so they should be destroyed when the world renderer is destroyed.
	*/
	class ISubrenderer
	{
	public:
		virtual void SetCamera(const MathCamera& camera) {};
		virtual void UpdateRendererMainThread(float deltaTime) = 0;
		virtual void Render(TRef<GraphicsCommandListImmediate> pImmCmd) = 0;
		virtual void ClearState() = 0;
	};
}