#pragma once
#include "Core/Core.h"
#include "Core/Types.h"
#include "Renderer/Elements/CommandList.h"
#include "World/Game/GameWorld.h"
#include "../LowLevelRenderer.h"

namespace ke
{
	struct TViewport2D
	{
		u32 X = 0, Y = 0;
		u32 Width = 0, Height = 0;
	};

	class TWorldRenderer : public TRefCounted
	{
	public:
		TWorldRenderer(TRef<TGameWorld> pWorld);
		~TWorldRenderer();
		
		void Render(TViewport2D ViewportSize);

		static TRef<TWorldRenderer> New(TRef<TGameWorld> pWorld);

	private:
		void RT_UpdateMaterialComponents(TRef<TCommandListImmediate> pImmCtx);
		void CollectRenderableViews();
		void MeshPass(TRef<TCommandListImmediate> pImmCtx);
		void FlushPass(TRef<TCommandListImmediate> pImmCtx);

	private:
		TViewport2D m_CurrentViewport{};
		TRef<TGameWorld> m_CurrentWorld;
		TLowLevelRenderer* m_LLR;
	};
}