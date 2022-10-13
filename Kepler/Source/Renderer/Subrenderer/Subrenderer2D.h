#pragma once
#include "Subrenderer.h"
#include "../World/Material.h"
#include "../LowLevelRenderer.h"

namespace ke
{
	struct LineDataVertex
	{
		float3 Position;
		float3 Color;
	};

	struct S2D_PrimitiveBatch
	{
		TRef<DynamicVertexBuffer> VertexBuffer{};
		TRef<DynamicIndexBuffer> IndexBuffer{};
		Array<LineDataVertex> Data{};
		Array<u32> Indices{};
	};

	class Subrenderer2D : public ISubrenderer
	{
		static Subrenderer2D* Instance;
	public:
		Subrenderer2D();

		void AddLine(float3 start, float3 end, float3 color = float3(1.0f, 1.0f, 1.0f));
		virtual void UpdateRendererMainThread(float deltaTime) override;
		virtual void Render(TRef<GraphicsCommandListImmediate> pImmCmd) override;
		virtual void ClearState() override;

		static Subrenderer2D* Get() { return Instance; }

	private:
		void RT_DrawLines(TRef<GraphicsCommandListImmediate> pImmCmd);

	private:
		static constexpr usize s_InitialLinesSize = 1024;
		std::mutex m_LinesMutex;
		std::array<S2D_PrimitiveBatch, TLowLevelRenderer::m_SwapChainFrameCount> m_LinesBatch{};

		TRef<TMaterial> m_LinesMaterial{};

		matrix4x4 m_ViewProjectionMatrix{};
	};
}