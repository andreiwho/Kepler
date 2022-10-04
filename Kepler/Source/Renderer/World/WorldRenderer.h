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

	class TWorldRenderer : public IntrusiveRefCounted
	{
	public:
		enum EReservedSlots
		{
			RS_Camera = 0,
			RS_User,
		};

		TWorldRenderer(TRef<TGameWorld> pWorld);
		~TWorldRenderer();
		
		void Render(TViewport2D ViewportSize);

		static TRef<TWorldRenderer> New(TRef<TGameWorld> pWorld);

		static void ClearStaticState();
	private:
		void RT_UpdateMaterialComponents(TRef<GraphicsCommandListImmediate> pImmCtx);
		void CollectRenderableViews();
		void MeshPass(TRef<GraphicsCommandListImmediate> pImmCtx);
		void FlushPass(TRef<GraphicsCommandListImmediate> pImmCtx);

	private:
		TViewport2D m_CurrentViewport{};
		TRef<TGameWorld> m_CurrentWorld;
		TLowLevelRenderer* m_LLR;

		struct TStaticState
		{
			void Init();
			void Clear();

			bool bInitialized = false;
			TRef<TParamBuffer> RS_CameraBuffer;
		};

		struct RS_CameraBufferStruct
		{
			matrix4x4 ViewProjection;
		};

		static TStaticState StaticState;
	};
}