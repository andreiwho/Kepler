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

		TWorldRenderer(RefPtr<GameWorld> pWorld);
		~TWorldRenderer();
		
		void Render(TViewport2D ViewportSize);
		void UpdateRendererMainThread(float deltaTime);
		void UpdateLightingData_MainThread();

		static RefPtr<TWorldRenderer> New(RefPtr<GameWorld> pWorld);

		static void ClearStaticState();
	private:
		void RT_UpdateMaterialComponents(RefPtr<GraphicsCommandListImmediate> pImmCtx);
		void CollectRenderableViews();
		void PrePass(RefPtr<GraphicsCommandListImmediate> pImmCtx);
		void MeshPass(RefPtr<GraphicsCommandListImmediate> pImmCtx);
		void FlushPass(RefPtr<GraphicsCommandListImmediate> pImmCtx);

	private:
		TViewport2D m_CurrentViewport{};
		RefPtr<GameWorld> m_CurrentWorld;
		LowLevelRenderer* m_LLR;

		struct TStaticState
		{
			void Init();
			void Clear();

			bool bInitialized = false;
			RefPtr<TParamBuffer> RS_CameraBuffer;
			RefPtr<TParamBuffer> RS_LightBuffer;
			RefPtr<TGraphicsPipeline> PrePassPipeline;
		};

		struct RS_CameraBufferStruct
		{
			matrix4x4 ViewProjection;
		};

		struct RS_LightBufferStruct
		{
			float4 Ambient;
			float4 DirectionalLightDirection;
			float4 DirectionalLightColor;
			float DirectionalLightIntensity;
		};

		static TStaticState* StaticState;
	};
}