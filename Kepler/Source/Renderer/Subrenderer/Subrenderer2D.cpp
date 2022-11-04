#include "Subrenderer2D.h"
#include "../RenderDevice.h"
#include "../RenderGlobals.h"
#include "Tools/MaterialLoader.h"

namespace ke
{

	Subrenderer2D* Subrenderer2D::Instance = nullptr;

	Subrenderer2D::Subrenderer2D()
	{
		Instance = this;

		CHECK(IsRenderThread());
		{
			for (usize index = 0; index < LowLevelRenderer::m_SwapChainFrameCount; ++index)
			{
				// Preallocate space for 1024 line vertices and indices
				m_LinesBatch[index].VertexBuffer = IVertexBufferDynamic::New(EBufferAccessFlags::WriteAccess, s_InitialLinesSize, sizeof(LineDataVertex));
				m_LinesBatch[index].IndexBuffer = IIndexBufferDynamic::New(EBufferAccessFlags::WriteAccess, s_InitialLinesSize, sizeof(u32));
				m_LinesBatch[index].Data.Reserve(s_InitialLinesSize);
			}
		}
	}

	void Subrenderer2D::AddLine(float3 start, float3 end, float3 color)
	{
		KEPLER_PROFILE_SCOPE();
		std::lock_guard lck{ m_LinesMutex };
		const auto currentFrameIndex = LowLevelRenderer::Get()->GetNextFrameIndex();
		m_LinesBatch[currentFrameIndex].Data.EmplaceBack(LineDataVertex{ start, color });
		m_LinesBatch[currentFrameIndex].Data.EmplaceBack(LineDataVertex{ end, color });
	}

	void Subrenderer2D::AddArrow(float3 start, float3 forward, float3 right, float len, float3 color)
	{
		KEPLER_PROFILE_SCOPE();
		const float3 lineEnd = start + forward * len;
		const float3 arrowMidpoint = forward * len * 0.7f;
		constexpr float wingLen = 0.05f;
		const float3 up = glm::cross(forward, right);
		const float3 midpointStart = start + arrowMidpoint;
		const float3 startRight = midpointStart + right * wingLen;
		const float3 startLeft = midpointStart - right * wingLen;
		const float3 startUp = midpointStart - up * wingLen;
		const float3 startDown = midpointStart + up * wingLen;

		AddLine(start, lineEnd, float3(1.0f, 0.0f, 0.0f));
		AddLine(startRight, lineEnd, float3(0.0f, 1.0f, 0.0f));
		AddLine(startLeft, lineEnd, float3(0.0f, 1.0f, 0.0f));
		AddLine(startUp, lineEnd, float3(0.0f, 0.0f, 1.0f));
		AddLine(startDown, lineEnd, float3(0.0f, 0.0f, 1.0f));
	}

	void Subrenderer2D::UpdateRendererMainThread(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_LinesMaterial)
		{
			m_LinesMaterial = TMaterialLoader::Get()->LoadMaterial("Engine://Materials/Mat_DefaultLines.kmat", true);
			CHECK(m_LinesMaterial);
		}
	}

	void Subrenderer2D::Render(RefPtr<ICommandListImmediate> pImmCmd)
	{
		KEPLER_PROFILE_SCOPE();
		pImmCmd->BeginDebugEvent("Subrenderer2D Render");
		CHECK(IsRenderThread());
		// Push all data to the GPU
		{
			const u8 currentFrameIndex = LowLevelRenderer::Get()->GetFrameIndex();
			S2D_PrimitiveBatch& currentBatch = m_LinesBatch[currentFrameIndex];

			const auto lineVtxCount = currentBatch.Data.GetLength();
			if (currentBatch.VertexBuffer->GetSize() < lineVtxCount)
			{
				currentBatch.VertexBuffer->RT_Resize(lineVtxCount);
			}

			pImmCmd->BindPipeline(m_LinesMaterial->GetPipeline());
			void* pLines = pImmCmd->MapBuffer(currentBatch.VertexBuffer);
			std::memcpy(pLines, currentBatch.Data.GetData(), currentBatch.Data.GetLength() * sizeof(LineDataVertex));
			pImmCmd->UnmapBuffer(currentBatch.VertexBuffer);
		}

		// Now draw the stuff
		RT_DrawLines(pImmCmd);
		pImmCmd->EndDebugEvent();
	}

	void Subrenderer2D::ClearState()
	{
		KEPLER_PROFILE_SCOPE();
		std::lock_guard lck{ m_LinesMutex };
		const auto currentFrameIndex = LowLevelRenderer::Get()->GetFrameIndex();
		m_LinesBatch[currentFrameIndex].Data.Clear();
	}

	void Subrenderer2D::RT_DrawLines(RefPtr<ICommandListImmediate> pImmCmd)
	{
		KEPLER_PROFILE_SCOPE();
		CHECK(IsRenderThread());
		const auto currentFrameIndex = LowLevelRenderer::Get()->GetFrameIndex();
		S2D_PrimitiveBatch& currentBatch = m_LinesBatch[currentFrameIndex];

		pImmCmd->BindVertexBuffers(currentBatch.VertexBuffer);
		pImmCmd->Draw(currentBatch.Data.GetLength(), 0);
	}

}