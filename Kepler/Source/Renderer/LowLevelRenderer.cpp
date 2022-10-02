#include "LowLevelRenderer.h"
#include "Platform/Window.h"
#include "RenderTypes.h"
#include "Async/Async.h"
#include "HLSLShaderCompiler.h"

namespace ke
{

	TLowLevelRenderer* TLowLevelRenderer::Instance;

	//////////////////////////////////////////////////////////////////////////
	TLowLevelRenderer::TLowLevelRenderer()
	{
		Instance = this;
		TRenderThread::Submit([this]
			{
				KEPLER_PROFILE_INIT_THREAD("RenderThread");
				RenderDevice = TRenderDevice::CreateRenderDevice();
			});
		TRenderThread::Wait();
		ShaderCache = MakeShared<TShaderCache>();
		PipelineCache = MakeShared<TGraphicsPipelineCache>();

		InitScreenQuad();
		TargetRegistry = MakeShared<TTargetRegistry>();
	}

	//////////////////////////////////////////////////////////////////////////
	TLowLevelRenderer::~TLowLevelRenderer()
	{
		TargetRegistry.reset();
		ShaderCache.reset();
		PipelineCache.reset();
		ScreenQuad.Pipeline.Release();
		ScreenQuad.VertexBuffer.Release();
		ScreenQuad.IndexBuffer.Release();
		ScreenQuad.Samplers.Release();
		TRenderThread::Submit(
			[this]
			{
				SwapChains.Clear();
				RenderDevice.Release();
			});
		TRenderThread::Wait();
	}

	//////////////////////////////////////////////////////////////////////////
	void TLowLevelRenderer::InitRenderStateForWindow(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]()
			{
				SwapChains.EmplaceBack(RenderDevice->CreateSwapChainForWindow(InWindow));
			}
		);
	}

	//////////////////////////////////////////////////////////////////////////
	void TLowLevelRenderer::PresentAll()
	{
		KEPLER_PROFILE_SCOPE();
		TRenderThread::Submit(
			[this]
			{
				for (const auto& SwapChain : SwapChains)
				{
					SwapChain->Present();
				}

				if (++FrameCounter % FlushPendingDeleteResourcesInterval == 0)
				{
					if (RenderDevice)
					{
						RenderDevice->RT_FlushPendingDeleteResources();
					}
				}
			});
		TRenderThread::Wait();


		SwapChainFrame = (SwapChainFrame + 1) % SwapChainFrameCount;
	}

	//////////////////////////////////////////////////////////////////////////
	void TLowLevelRenderer::DestroyRenderStateForWindow(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]
			{
				auto FoundSwapChain = std::find_if(std::begin(SwapChains), std::end(SwapChains),
					[InWindow](const auto& SwapChain)
					{
						return SwapChain->GetAssociatedWindow() == InWindow;
					}
				);
				if (FoundSwapChain != SwapChains.end())
				{
					SwapChains.Remove(FoundSwapChain);
				}
				SwapChains.Shrink();
			});
		TRenderThread::Wait();
	}

	//////////////////////////////////////////////////////////////////////////
	void TLowLevelRenderer::OnWindowResized(class TWindow* InWindow)
	{
		TRenderThread::Submit(
			[this, InWindow]
			{
				if (auto SwapChain = FindAssociatedSwapChain(InWindow))
				{
					SwapChain->Resize(InWindow->GetWidth(), InWindow->GetHeight());
				}
			});
	}

	//////////////////////////////////////////////////////////////////////////
	void TLowLevelRenderer::InitScreenQuad()
	{
		struct TVertex
		{
			float3 Pos{};
			float3 Col{};
			float2 UV{};
		};

		TDynArray<TVertex> QuadVertices =
		{
			{{-1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{ 1.0f, 1.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{ 1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
			{{-1.0f,-1.0f, 0.0f }, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
		};

		TDynArray<u32> Indices = { 0,1,3,1,2,3 };

		auto Task = TRenderThread::Submit(
			[&, this]
			{
				auto Compiler = THLSLShaderCompiler::CreateShaderCompiler();
				auto Shader = Compiler->CompileShader("EngineShaders://DefaultScreenQuad.hlsl", EShaderStageFlags::Vertex | EShaderStageFlags::Pixel);
				CHECK(Shader);

				TGraphicsPipelineConfiguration Config{};
				Config.DepthStencil.bDepthEnable = false;
				Config.VertexInput.VertexLayout = Shader->GetReflection()->VertexLayout;
				Config.ParamMapping = Shader->GetReflection()->ParamMapping;

				ScreenQuad.Pipeline = MakeRef(New<TGraphicsPipeline>(Shader, Config));
				ScreenQuad.VertexBuffer = TVertexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(QuadVertices));
				ScreenQuad.IndexBuffer = TIndexBuffer::New(EBufferAccessFlags::GPUOnly, TDataBlob::New(Indices));
				ScreenQuad.Samplers = ScreenQuad.Pipeline->GetParamMapping()->CreateSamplerPack();
			});
		Await(Task);
	}

	//////////////////////////////////////////////////////////////////////////
	TRef<TSwapChain> TLowLevelRenderer::FindAssociatedSwapChain(class TWindow* InWindow) const
	{
		auto FoundSwapChain = SwapChains.Find(
			[InWindow](const auto& SwapChain)
			{
				return SwapChain->GetAssociatedWindow() == InWindow;
			}
		);
		if (FoundSwapChain)
		{
			return *FoundSwapChain;
		}
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
}