#include "LowLevelRenderer.h"
#include "Platform/Window.h"
#include "RenderTypes.h"
#include "Async/Async.h"
#include "HLSLShaderCompiler.h"

namespace ke
{

	LowLevelRenderer* LowLevelRenderer::Instance;

	//////////////////////////////////////////////////////////////////////////
	LowLevelRenderer::LowLevelRenderer()
	{
		Instance = this;
		TRenderThread::Submit([this]
			{
				KEPLER_PROFILE_INIT_THREAD("RenderThread");
				m_RenderDevice = TRenderDevice::CreateRenderDevice();
			});
		TRenderThread::Wait();
		m_ShaderCache = MakeShared<TShaderCache>();
		m_PipelineCache = MakeShared<GraphicsPipelineCache>();

		InitScreenQuad();
		m_TargetRegistry = MakeShared<RenderTargetRegistry>();
	}

	//////////////////////////////////////////////////////////////////////////
	LowLevelRenderer::~LowLevelRenderer()
	{
		m_TargetRegistry.reset();
		m_ShaderCache.reset();
		m_PipelineCache.reset();
		m_ScreenQuad.Pipeline.Release();
		m_ScreenQuad.VertexBuffer.Release();
		m_ScreenQuad.IndexBuffer.Release();
		m_ScreenQuad.Samplers.Release();
		TRenderThread::Submit(
			[this]
			{
				m_SwapChains.Clear();
				m_RenderDevice.Release();
			});
		TRenderThread::Wait();
	}

	//////////////////////////////////////////////////////////////////////////
	void LowLevelRenderer::InitRenderStateForWindow(class TWindow* pWindow)
	{
		TRenderThread::Submit(
			[this, pWindow]()
			{
				m_SwapChains.EmplaceBack(m_RenderDevice->CreateSwapChainForWindow(pWindow));
			}
		);
	}

	//////////////////////////////////////////////////////////////////////////
	void LowLevelRenderer::PresentAll()
	{
		KEPLER_PROFILE_SCOPE();
		TRenderThread::Submit(
			[this]
			{
				for (const auto& swapChain : m_SwapChains)
				{
					swapChain->Present();
				}

				if (++m_FrameCounter % m_FlushPendingDeleteResourcesInterval == 0)
				{
					if (m_RenderDevice)
					{
						m_RenderDevice->RT_FlushPendingDeleteResources();
					}
				}
			});
		TRenderThread::Wait();


		m_SwapChainFrame = (m_SwapChainFrame + 1) % m_SwapChainFrameCount;
		m_NextFrameIndex = (m_NextFrameIndex + 1) % m_SwapChainFrameCount;
	}

	//////////////////////////////////////////////////////////////////////////
	void LowLevelRenderer::DestroyRenderStateForWindow(class TWindow* pWindow)
	{
		TRenderThread::Submit(
			[this, pWindow]
			{
				auto foundSwapChain = std::find_if(std::begin(m_SwapChains), std::end(m_SwapChains),
					[pWindow](const auto& swapChain)
					{
						return swapChain->GetAssociatedWindow() == pWindow;
					}
				);
				if (foundSwapChain != m_SwapChains.end())
				{
					m_SwapChains.Remove(foundSwapChain);
				}
				m_SwapChains.Shrink();
			});
		TRenderThread::Wait();
	}

	//////////////////////////////////////////////////////////////////////////
	void LowLevelRenderer::OnWindowResized(class TWindow* pWindow)
	{
		TRenderThread::Submit(
			[this, pWindow]
			{
				if (auto pSwapChain = FindAssociatedSwapChain(pWindow))
				{
					pSwapChain->Resize(pWindow->GetWidth(), pWindow->GetHeight());
				}
			});
	}

	//////////////////////////////////////////////////////////////////////////
	void LowLevelRenderer::InitScreenQuad()
	{
		struct TVertex
		{
			float3 Pos{};
			float3 Col{};
			float2 UV{};
		};

		Array<TVertex> QuadVertices =
		{
			{{-1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{ 1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{{-1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		};

		Array<u32> indices = { 0,1,3,1,2,3 };

		auto task = TRenderThread::Submit(
			[&, this]
			{
				auto pCompiler = THLSLShaderCompiler::CreateShaderCompiler();
				auto pShader = pCompiler->CompileShader("EngineShaders://DefaultScreenQuad.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
				CHECK(pShader);

				GraphicsPipelineConfig config{};
				config.DepthStencil.bDepthEnable = false;
				config.VertexInput.VertexLayout = pShader->GetReflection()->VertexLayout;
				config.ParamMapping = pShader->GetReflection()->ParamMapping;

				m_ScreenQuad.Pipeline = MakeRef(New<IGraphicsPipeline>(pShader, config));
				m_ScreenQuad.VertexBuffer = IVertexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(QuadVertices));
				m_ScreenQuad.IndexBuffer = IIndexBuffer::New(EBufferAccessFlags::GPUOnly, IAsyncDataBlob::New(indices));
				m_ScreenQuad.Samplers = m_ScreenQuad.Pipeline->GetParamMapping()->CreateSamplerPack();
			});
		Await(task);
	}

	//////////////////////////////////////////////////////////////////////////
	RefPtr<ISwapChain> LowLevelRenderer::FindAssociatedSwapChain(class TWindow* pWindow) const
	{
		auto foundSwapChain = m_SwapChains.Find(
			[pWindow](const auto& pSwapChain)
			{
				return pSwapChain->GetAssociatedWindow() == pWindow;
			}
		);
		if (foundSwapChain)
		{
			return *foundSwapChain;
		}
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
}