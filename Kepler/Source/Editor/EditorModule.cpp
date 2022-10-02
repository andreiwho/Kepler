#include "EditorModule.h"
#include "Renderer/RenderGlobals.h"

#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

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
#include "Widgets/Elements.h"
#include "World/Game/GameWorld.h"
#include "World/Game/GameEntity.h"
#include "Panels/DetailsPanel.h"
#include "Panels/LogPanel.h"
#include "ImGuizmo.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Components/TransformComponent.h"
#include "imgui_internal.h"
#include "Core/Timer.h"
#include "Platform/Input.h"
#include "Platform/Platform.h"
#include "World/Game/Helpers/EntityHelper.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	TEditorModule::TEditorModule(TWindow* pWindow)
		: m_pMainWindow(pWindow)
	{
		for (auto& Size : m_ViewportSizes)
		{
			Size = float2(pWindow->GetWidth(), pWindow->GetHeight());
		}

#ifndef PLATFORM_DESKTOP
		CRASHMSG("Editor is not supported on this platform");
#endif
		KEPLER_INFO(LogEditor, "Initializing editor module");
		ImGui::SetAllocatorFunctions(
			[](size_t Size, void* UserData) { return TMalloc::Get()->Allocate(Size); },
			[](void* Data, void*) { TMalloc::Get()->Free(Data); }
		);
		// Initial ImGui config
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;      // Enable Multi-Viewport / Platform Windows. 
																	// This currently does not work with the render thread
		SetupStyle();

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
		case ke::ERenderAPI::DirectX11:
		{
			auto initTask = TRenderThread::Submit([]
				{
					auto pDevice = RefCast<TRenderDeviceD3D11>(TLowLevelRenderer::Get()->GetRenderDevice());
					auto pDeviceHandle = pDevice->GetDevice();
					auto pContextHandle = pDevice->GetImmediateContext();
					CHECK(ImGui_ImplDX11_Init(pDeviceHandle, pContextHandle));
					KEPLER_INFO(LogEditor, "Editor layer D3D11 Backend initialized on render thread");
				});
			Await(initTask);
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
		m_LogPanel.reset();
		Await(TRenderThread::Submit([]
			{
				ImGui_ImplDX11_Shutdown();
			}));
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		KEPLER_INFO(LogEditor, "Terminating editor module");
	}

	float2 TEditorModule::GetViewportSize(EViewportIndex idx)
	{
		return m_ViewportSizes[(u32)idx];
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::BeginGUIPass()
	{
		KEPLER_PROFILE_SCOPE();
		auto task = TRenderThread::Submit(
			[]
			{
				KEPLER_PROFILE_SCOPE();
				ImGui_ImplDX11_NewFrame();
			});
		{
			KEPLER_PROFILE_SCOPE("GUI GLFW NEW FRAME");
			ImGui_ImplGlfw_NewFrame();
		}
		{
			KEPLER_PROFILE_SCOPE("GUI AWAIT RENDER");
			Await(task);
		}

		{
			KEPLER_PROFILE_SCOPE("GUI IMGUI NEW FRAME");
			ImGui::NewFrame();
		}

		{
			KEPLER_PROFILE_SCOPE("GUI IMGUIZMO NEW FRAME");
			ImGuizmo::BeginFrame();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawEditor()
	{
		KEPLER_PROFILE_SCOPE();
		DrawMenuBar();
		// DrawGizmo();
		DrawViewports();
		DrawDetailsPanel();
		DrawSceneGraph();
		DrawDebugTools();
		ImGui::ShowDemoWindow();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::OnUpdate(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		ControlEditorCamera(deltaTime);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::EndGUIPass()
	{
		KEPLER_PROFILE_SCOPE();
		ImGui::Render();
		Await(TRenderThread::Submit(
			[]
			{
				auto pLLR = TLowLevelRenderer::Get();
				auto pSwapChain = pLLR->GetSwapChain(0);
				auto pImmCtx = pLLR->GetRenderDevice()->GetImmediateCommandList();
				pImmCtx->StartDrawingToSwapChainImage(pSwapChain);
				pImmCtx->ClearSwapChainImage(pSwapChain, { 0.1f, 0.1f, 0.1f, 1.0f });

				const auto FrameIndex = pLLR->GetFrameIndex();
				// TTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}));

		// This currently doesn't work. The only way to fix this is to handle window creation manually, 
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
	void TEditorModule::SetEditedWorld(TRef<TGameWorld> pWorld)
	{
		KEPLER_PROFILE_SCOPE();
		m_pEditedWorld = pWorld;
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::SelectEntity(TGameEntityId id)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld || !m_pEditedWorld->IsValidEntity(id))
		{
			return;
		}

		m_SelectedEntity = id;
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::UnselectEverything()
	{
		m_SelectedEntity = TGameEntityId{};
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::OnPlatformEvent(const TPlatformEventBase& event)
	{
		KEPLER_PROFILE_SCOPE();
		TPlatformEventDispatcher Dispatcher(event);
		Dispatcher.Dispatch(this, &TEditorModule::OnKeyDown);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseButtonDown);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseButtonUp);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseMove);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::SetupStyle()
	{
		ImGuiIO& io = ImGui::GetIO();
		const TString font = VFSResolvePath("Engine://Fonts/Roboto-Regular.ttf");
		io.Fonts->AddFontFromFileTTF(font.c_str(), 15.0f);

		auto& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		// TEXT
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		
		// WINDOWS
		colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.94f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		
		// FRAMES
		colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.7f, 0.7f, 0.7f, 0.67f);
		
		// TITLES
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);

		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.81f, 0.41f, 0.1f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.21f, 0.21f, 0.21f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.31f, 0.31f, 0.31f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.81f, 0.41f, 0.1f, 1.00f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

		// Tabs
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		
		// Other
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		//		ImGui::StyleColorsDark();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawMenuBar()
	{
		KEPLER_PROFILE_SCOPE();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File", true))
			{
				ImGui::MenuItem("NoOp", "No + Op");
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawViewports()
	{
		KEPLER_PROFILE_SCOPE();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport 1");
		{
			if (ImGui::IsWindowHovered())
			{
				m_HoveredViewport = EViewportIndex::Viewport1;
				m_bIsCursorInViewport = true;
			}
			else
			{
				m_bIsCursorInViewport = false;
			}

			ImVec2 region = ImGui::GetContentRegionAvail();

			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;

			m_ViewportSizes[(u32)EViewportIndex::Viewport1] = float2(region.x, region.y);
			m_ViewportPositions[(u32)EViewportIndex::Viewport1] = float2(vMin.x, vMin.y);
			auto pLLR = TLowLevelRenderer::Get();

			auto pRenderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
			auto pViewportSampler = pRenderTargetGroup->GetTextureSamplerAtArrayLayer(pLLR->GetFrameIndex());
			auto pImage = pViewportSampler->GetImage();
			ImGui::Image(
				(ImTextureID)pViewportSampler->GetNativeHandle(),
				ImVec2(pImage->GetWidth(), pImage->GetHeight()));

			DrawGizmo();
			DrawViewportGizmoControls();
			DrawViewportCameraControls();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawViewportGizmoControls()
	{
		KEPLER_PROFILE_SCOPE();
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImGui::SetCursorScreenPos(ImVec2(vMin.x + m_ViewportToolbarOffset.x, vMin.y + m_ViewportToolbarOffset.y));

		ImGui::BeginGroup();
		ImGui::Text("Operation");
		ImGui::SameLine();
		if (ImGui::RadioButton("Translate", m_EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE))
		{
			m_EditOperationIndex = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_EditOperationIndex == ImGuizmo::OPERATION::ROTATE))
		{
			m_EditOperationIndex = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_EditOperationIndex == ImGuizmo::OPERATION::SCALE))
		{
			m_EditOperationIndex = ImGuizmo::OPERATION::SCALE;
		}

		if (m_EditOperationIndex != ImGuizmo::OPERATION::SCALE)
		{
			ImGui::SameLine(0.0f, 32);
			ImGui::Text("Mode");
			ImGui::SameLine();
			if (ImGui::RadioButton("World", m_EditSpaceIndex == ImGuizmo::MODE::WORLD))
			{
				m_EditSpaceIndex = ImGuizmo::MODE::WORLD;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", m_EditSpaceIndex == ImGuizmo::MODE::LOCAL))
			{
				m_EditSpaceIndex = ImGuizmo::MODE::LOCAL;
			}

			// Snapping
			ImGui::SameLine(0.0f, 32);
			ImGui::Checkbox("Snap", &m_bSnapEnabled);

			if (m_bSnapEnabled)
			{
				ImGui::SameLine();
				if (m_EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE)
				{
					const char* pEntries = " 1\0 2\0 5\0 10\0 25\0 50\0 100\0";

					i32 curValue = (i32)m_TranslationSnap;
					ImGui::SetNextItemWidth(50.0f);
					if (ImGui::Combo("##v", &curValue, pEntries))
					{
						m_TranslationSnap = (ETranslationSnap)curValue;
					}
				}
				if (m_EditOperationIndex == ImGuizmo::OPERATION::ROTATE)
				{
					const char* pEntries = " 1\0 10\0 30\0 45\0 90\0 180\0 ";

					i32 curVal = (i32)m_RotationSnap;
					ImGui::SetNextItemWidth(50.0f);
					if (ImGui::Combo("##v", &curVal, pEntries))
					{
						m_RotationSnap = (ERotationSnap)curVal;
					}
				}
			}
		}

		ImGui::EndGroup();
		// ImGui::SameLine();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawViewportCameraControls()
	{
		KEPLER_PROFILE_SCOPE();
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImGui::SetCursorScreenPos(ImVec2(vMin.x + m_ViewportToolbarOffset.x, vMax.y - ImGui::GetTextLineHeight() - m_ViewportToolbarOffset.y));
		ImGui::Text("Camera");
		ImGui::SameLine(0.0f, 32.0f);
		ImGui::Text("Sensitivity");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat("##Sensitivity", &m_EditorCameraSensitivity, 0.1f, 128.0f);
		ImGui::SameLine(0.0f, 32.0f);
		ImGui::Text("Movement Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat("##MovementSpeed", &m_EditorCameraSpeed, 0.1f, 128.0f);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawDetailsPanel()
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		TEditorDetailsPanel widget(m_pEditedWorld, m_SelectedEntity);
		widget.Draw();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawSceneGraph()
	{
		KEPLER_PROFILE_SCOPE();
		ImGui::Begin("Scene Graph");
		i32 idx = 0;
		if (ImGui::TreeNodeEx((void*)(intptr_t)idx, ImGuiTreeNodeFlags_DefaultOpen, m_pEditedWorld->GetName().c_str()))
		{
			m_pEditedWorld->GetComponentView<TNameComponent, TGameEntity>().each(
				[&, this](auto id, TNameComponent& NC, TGameEntity& GE)
				{
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
					if (!GE.ShouldHideInSceneGraph())
					{
						bool bNodeOpen = false;
						if (m_SelectedEntity == TGameEntityId{ id })
						{
							flags |= ImGuiTreeNodeFlags_Selected;
						}

						bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)idx, flags, NC.Name.c_str());
						if (ImGui::IsItemClicked())
						{
							m_SelectedEntity = TGameEntityId{ id };
						}

						if (bNodeOpen)
						{
							ImGui::TreePop();
						}

						idx++;
					}
				});
			ImGui::TreePop();
		}
		ImGui::End();
	}

	void TEditorModule::DrawDebugTools()
	{
		KEPLER_PROFILE_SCOPE();
		// Draw console
		if (!m_LogPanel)
		{
			m_LogPanel = MakeShared<TLogPanel>();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawGizmo()
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		if (m_HoveredViewport == EViewportIndex::Max)
		{
			return;
		}

		if (m_pEditedWorld->IsValidEntity(m_SelectedEntity))
		{
			ImVec2 vMin = ImGui::GetWindowContentRegionMin();
			ImVec2 vMax = ImGui::GetWindowContentRegionMax();

			vMin.x += ImGui::GetWindowPos().x;
			vMin.y += ImGui::GetWindowPos().y;

			auto mainCameraId = m_pEditedWorld->GetMainCamera();
			// auto& MainCameraEntity = EditedWorld->GetEntityFromId(SelectedEntity);
			TCamera& camera = m_pEditedWorld->GetComponent<TCameraComponent>(mainCameraId).GetCamera();

			matrix4x4 viewMatrix = camera.GenerateViewMatrix();
			matrix4x4 projMatrix = camera.GenerateProjectionMatrix();

			TTransformComponent& transformComp = m_pEditedWorld->GetComponent<TTransformComponent>(m_SelectedEntity);
			matrix4x4 transform = transformComp.GetTransform().GenerateWorldMatrix();

			ImGuizmo::SetDrawlist();
			// Snapping
			float3 snapVec = CalculateSnapVec();

			m_bIsGizmoHovered = ImGuizmo::IsOver();
			m_bIsGizmoUsed = ImGuizmo::IsUsing();

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(vMin.x, vMin.y, vMax.x, vMax.y);
			if (ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix),
				static_cast<ImGuizmo::OPERATION>(m_EditOperationIndex),
				static_cast<ImGuizmo::MODE>(m_EditSpaceIndex),
				glm::value_ptr(transform),
				nullptr,
				m_bSnapEnabled ? &snapVec.x : nullptr))
			{
				float3 Location, Rotation, Scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), &Location.x, &Rotation.x, &Scale.x);
				transformComp.SetTransform(TWorldTransform{ Location, Rotation, Scale });
			}
		}
	}

	void TEditorModule::ControlEditorCamera(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		if (!m_pEditedWorld->IsValidEntity(m_EditorCameraEntity))
		{
			m_EditorCameraEntity = m_pEditedWorld->CreateCamera("_EditorCamera");
			TEntityHandle camera{ m_pEditedWorld, m_EditorCameraEntity };
			
			camera->SetLocation(float3(0.0f, -3.0f, 0.0f));
			camera->SetHideInSceneGraph(true);
		}
		m_pEditedWorld->SetMainCamera(m_EditorCameraEntity);

		if (m_bIsControllingCamera)
		{
			TEntityHandle camera{ m_pEditedWorld, m_EditorCameraEntity };
			ImGuiIO& IO = ImGui::GetIO();
			auto mouseDelta = TPlatform::Get()->GetMouseState().GetOffset();
			float3 rotation = camera->GetRotation();
			rotation.z -= mouseDelta.X * deltaTime * m_EditorCameraSensitivity;
			rotation.x -= mouseDelta.Y * deltaTime * m_EditorCameraSensitivity;
			camera->SetRotation(rotation);

			auto location = camera->GetLocation();
			if (TInput::GetKey(EKeyCode::LeftAlt))
			{
				// Orbiting camera
			}
			else
			{
				// First person camera
				if (TInput::GetKey(EKeyCode::W))
				{
					location += camera->GetForwardVector() * deltaTime * m_EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::S))
				{
					location -= camera->GetForwardVector() * deltaTime * m_EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::A))
				{
					location -= camera->GetRightVector() * deltaTime * m_EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::D))
				{
					location += camera->GetRightVector() * deltaTime * m_EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::E))
				{
					location += camera->GetUpVector() * deltaTime * m_EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::Q))
				{
					location -= camera->GetUpVector() * deltaTime * m_EditorCameraSpeed;
				}
			}
			camera->SetLocation(location);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnKeyDown(const TKeyDownEvent& event)
	{
		if (m_bIsControllingCamera)
		{

		}
		else
		{
			switch (event.Key)
			{
				// Switch gizmos
			case EKeyCode::W:
			{
				m_EditOperationIndex = ImGuizmo::OPERATION::TRANSLATE;
			} break;
			case EKeyCode::E:
			{
				m_EditOperationIndex = ImGuizmo::OPERATION::ROTATE;
			} break;
			case EKeyCode::R:
			{
				m_EditOperationIndex = ImGuizmo::OPERATION::SCALE;
				if (m_bSnapEnabled)
				{
					static std::once_flag once;
					std::call_once(once, [] { KEPLER_WARNING(LogEditor, "Gizmo scaling currently doesn't support snapping"); });
				}
			} break;
			// Switch spaces
			case EKeyCode::T:
			{
				const ImGuizmo::MODE mode = static_cast<ImGuizmo::MODE>(m_EditSpaceIndex);
				if (mode == ImGuizmo::MODE::LOCAL)
				{
					m_EditSpaceIndex = ImGuizmo::MODE::WORLD;
				}
				else
				{
					m_EditSpaceIndex = ImGuizmo::MODE::LOCAL;
				}
			} break;
			case EKeyCode::Q:
			{
				m_bSnapEnabled = !m_bSnapEnabled;
			}
			break;
			default:
				break;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseButtonDown(const TMouseButtonDownEvent& event)
	{
		// Read entity id under render target
		if (event.Button & EMouseButton::Left)
		{
			if (m_bIsCursorInViewport && !m_bIsControllingCamera && !m_bIsGizmoHovered && !m_bIsGizmoUsed)
			{
				TrySelectEntity();
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseButtonUp(const TMouseButtonUpEvent& event)
	{
		if (m_bIsControllingCamera && event.Button & EMouseButton::Right)
		{
			TPlatform::Get()->SetCursorMode(ECursorMode::Visible);
			m_bIsControllingCamera = false;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseMove(const TMouseMoveEvent& event)
	{
		// Start moving camera only if mouse button pressed and mouse is dragged, otherwise, use context menu (probably...)
		if (TInput::GetMouseButon(EMouseButton::Right) && m_bIsCursorInViewport)
		{
			TPlatform::Get()->SetCursorMode(ECursorMode::HiddenLocked);
			m_bIsControllingCamera = true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	float3 TEditorModule::CalculateSnapVec() const
	{
		KEPLER_PROFILE_SCOPE();
		if (m_bSnapEnabled)
		{
			if (m_EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE)
			{
				switch (m_TranslationSnap)
				{
				case ke::ETranslationSnap::_1Unit:
					return float3(0.01f);
					break;
				case ke::ETranslationSnap::_2Units:
					return float3(0.02f);
					break;
				case ke::ETranslationSnap::_5Units:
					return float3(0.05f);
					break;
				case ke::ETranslationSnap::_10Units:
					return float3(0.1f);
					break;
				case ke::ETranslationSnap::_25Units:
					return float3(0.25f);
					break;
				case ke::ETranslationSnap::_50Units:
					return float3(0.5f);
					break;
				case ke::ETranslationSnap::_100Units:
					return float3(1.0f);
					break;
				default:
					break;
				}
			}
			if (m_EditOperationIndex == ImGuizmo::OPERATION::ROTATE)
			{
				switch (m_RotationSnap)
				{
				case ke::ERotationSnap::_1Degree:
					return float3(1.0f);
					break;
				case ke::ERotationSnap::_10Degrees:
					return float3(10.0f);
					break;
				case ke::ERotationSnap::_30Degrees:
					return float3(30.0f);
					break;
				case ke::ERotationSnap::_45Degrees:
					return float3(45.0f);
					break;
				case ke::ERotationSnap::_90Degrees:
					return float3(90.0f);
					break;
				case ke::ERotationSnap::_180Degrees:
					return float3(180.0f);
					break;
				default:
					break;
				}
			}
		}
		return float3(0.0f);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::TrySelectEntity()
	{
		// Read render target
		if (TTargetRegistry::Get()->RenderTargetGroupExists("IdTarget"))
		{
			auto pTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup("IdTarget");
			TRef<TRenderTarget2D> pTarget = pTargetGroup->GetRenderTargetAtArrayLayer(0);
			TRef<TImage2D> pTargetImage = pTarget->GetImage();
			if (auto pImmCmd = TLowLevelRenderer::Get()->GetRenderDevice()->GetImmediateCommandList())
			{
				i32 idColor = Await(TRenderThread::Submit([&, this]
					{
						usize align;
						i32* pData = (i32*)pImmCmd->MapImage2D(pTargetImage, align);

						float x, y;
						TInput::GetMousePosition(x, y);
						const auto ViewportPos = m_ViewportPositions[(u32)EViewportIndex::Viewport1];
						x -= ViewportPos.x;
						y -= ViewportPos.y;

						const auto width = align / sizeof(i32);
						const auto outIndex = (u32)width * (u32)y + (u32)x;
						const i32 retVal = pData[outIndex];
						pImmCmd->UnmapImage2D(pTargetImage);
						return retVal;
					}));

				if (idColor != -1)
				{
					m_SelectedEntity = TGameEntityId{ (entt::entity)idColor };
				}
			}
		}
		else
		{
			KEPLER_WARNING(LogEditor, "Tried to access IdTarget render target, which doesn't exist");
		}
	}

	//////////////////////////////////////////////////////////////////////////
}