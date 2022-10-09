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
			RS_Light = 1,
			RS_User,
		};

		TWorldRenderer(TRef<TGameWorld> pWorld);
		~TWorldRenderer();
		
		void Render(TViewport2D ViewportSize);
		void UpdateRendererMainThread(float deltaTime);
		void UpdateLightingData_MainThread();

		static TRef<TWorldRenderer> New(TRef<TGameWorld> pWorld);

		static void ClearStaticState();
	private:
		void RT_UpdateMaterialComponents(TRef<GraphicsCommandListImmediate> pImmCtx);
		void CollectRenderableViews();
		void PrePass(TRef<GraphicsCommandListImmediate> pImmCtx);
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
			TRef<TParamBuffer> RS_LightBuffer;
			TRef<TGraphicsPipeline> PrePassPipeline;
		};

		struct RS_CameraBufferStruct
		{
			matrix4x4 ViewProjection;
		};

		struct RS_LightBufferStruct
		{
			float3 Ambient;
		};

		static TStaticState* StaticState;
	};
}