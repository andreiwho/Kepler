#include "EditorModule.h"
#include "Renderer/RenderGlobals.h"

#include "imgui.h"

#ifdef PLATFORM_DESKTOP
# include "backends/imgui_impl_glfw.h"
#endif

#ifdef WIN32
# include "backends/imgui_impl_dx11.h"
#endif
#include "Platform/PlatformGLFW/WindowGLFW.h"
#include "Renderer/RenderThread.h"
#include "Renderer/LowLevelRenderer.h"
#include "Renderer/RendererD3D11/RenderDeviceD3D11.h"
#include "Async/Async.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TEditorModule::TEditorModule(TWindow* pWindow)
	{
		for (auto& Size : ViewportSizes)
		{
			Size = float2(pWindow->GetWidth(), pWindow->GetHeight());
		}

#ifndef PLATFORM_DESKTOP
		CRASHMSG("Editor is not supported on this platform");
#endif
		KEPLER_INFO(LogEditor, "Initializing editor module");
	
		// Initial ImGui config
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows. 
																	// This currently does not work with the render thread

		// TODO: Override with own style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		CHECK(ImGui_ImplGlfw_InitForOther(static_cast<TWindowGLFW*>(pWindow)->GetGLFWWindow(), true));

		switch (GRenderAPI)
		{
#ifdef WIN32
		case Kepler::ERenderAPI::DirectX11:
		{
			auto InitTask = TRenderThread::Submit([]
				{
					auto Device = RefCast<TRenderDeviceD3D11>(TLowLevelRenderer::Get()->GetRenderDevice());
					auto DeviceHandle = Device->GetDevice();
					auto ContextHandle = Device->GetImmediateContext();
					CHECK(ImGui_ImplDX11_Init(DeviceHandle, ContextHandle));
					KEPLER_INFO(LogEditor, "Editor layer D3D11 Backend initialized on render thread");
				});
			Await(InitTask);
		}
#endif
		break;
		default:
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	TEditorModule::~TEditorModule()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		KEPLER_INFO(LogEditor, "Terminating editor module");
	}

	float2 TEditorModule::GetViewportSize(EViewportIndex Index)
	{
		return ViewportSizes[(u32)Index];
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::BeginGUIPass()
	{
		auto Task = TRenderThread::Submit(
			[]
			{
				ImGui_ImplDX11_NewFrame();
			});
		ImGui_ImplGlfw_NewFrame();
		Await(Task);
		ImGui::NewFrame();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawEditor()
	{
		DrawViewports();
		// ImGui::ShowDemoWindow();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::EndGUIPass()
	{
		ImGui::Render();
		Await(TRenderThread::Submit(
			[] 
			{
				auto LLR = TLowLevelRenderer::Get();
				auto SwapChain = LLR->GetSwapChain(0);
				auto pImmCtx = LLR->GetRenderDevice()->GetImmediateCommandList();
				pImmCtx->StartDrawingToSwapChainImage(SwapChain);
				pImmCtx->ClearSwapChainImage(SwapChain, { 0.1f, 0.1f, 0.1f, 1.0f });

				const auto FrameIndex = LLR->GetFrameIndex();
				// TTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}));

		// This currently doesn't work. The only way to fix this is to handle window creation manually, 
		ImGuiIO& IO = ImGui::GetIO();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			Await(TRenderThread::Submit(
				[]
				{
					ImGui::RenderPlatformWindowsDefault();
				}));
		}

	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::SetupStyle()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawViewports()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::Begin("Viewport 1");
		{
			ImVec2 Region = ImGui::GetContentRegionAvail();
			ViewportSizes[(u32)EViewportIndex::Viewport1] = float2(Region.x, Region.y);

			auto LLR = TLowLevelRenderer::Get();

			auto RenderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
			auto ViewportSampler = RenderTargetGroup->GetTextureSamplerAtArrayLayer(LLR->GetFrameIndex());
			auto Image = ViewportSampler->GetImage();
			ImGui::Image(
				(ImTextureID)ViewportSampler->GetNativeHandle(),
				ImVec2(Image->GetWidth(), Image->GetHeight()));
		}
		ImGui::End();
	}

}