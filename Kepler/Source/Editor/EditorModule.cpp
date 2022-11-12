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
#include "Tools/MaterialLoader.h"
#include "World/Game/Components/MaterialComponent.h"
#include "World/Game/Components/StaticMeshComponent.h"
#include "Tools/ImageLoader.h"
#include "World/Game/Components/Light/AmbientLightComponent.h"
#include "World/Game/Components/Light/DirectionalLightComponent.h"
#include "Renderer/Subrenderer/Subrenderer2D.h"
#include "World/Game/GameWorldSerializer.h"
#include "Core/Filesystem/FileUtils.h"
#include "Core/App.h"
#include "Core/Json/Serialization.h"
#include "Renderer/World/WorldRenderer.h"

namespace ke
{
	//////////////////////////////////////////////////////////////////////////
	EditorModule::EditorModule(TWindow* pWindow)
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
					auto pDevice = RefCast<TRenderDeviceD3D11>(LowLevelRenderer::Get()->GetRenderDevice());
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

		// Asset browser
		m_AssetBrowserPanel = MakeShared<TAssetBrowserPanel>();
		m_MaterialEditor = MakeShared<MaterialEditor>();
	}

	//////////////////////////////////////////////////////////////////////////
	EditorModule::~EditorModule()
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

	float2 EditorModule::GetViewportSize(EViewportIndex idx)
	{
		return m_ViewportSizes[(u32)idx];
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::BeginGUIPass()
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
	void EditorModule::DrawEditor()
	{
		KEPLER_PROFILE_SCOPE();
		DrawMenuBar();
		// DrawGizmo();
		DrawViewports();
		DrawDetailsPanel();
		DrawSceneGraph();
		m_AssetBrowserPanel->Draw();
		// m_AssetBrowserPanel->DrawInSaveMode(EFieldAssetType::All);
		DrawDebugTools();
		DrawEngineInfo();
		m_MaterialEditor->Draw();
		// ImGui::ShowDemoWindow();
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::OnUpdate(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		ControlEditorCamera(deltaTime);
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawEngineInfo()
	{
		bool bOpen = ImGui::Begin("Engine Info");
		if (!bOpen)
		{
			ImGui::End();
		}

		if (bOpen)
		{
			auto pEngineClass = GetReflectedClass<Engine>();
			if (pEngineClass)
			{
				TEditorElements::DrawReflectedObjectFields("ENGINE", pEngineClass->GetClassId(), Engine::Get());
			}

			auto pRendererClass = GetReflectedClass<WorldRenderer>();
			if (pRendererClass)
			{
				TEditorElements::DrawReflectedObjectFields("RENDERER", pRendererClass->GetClassId(), WorldRenderer::Get());
			}

			auto pLLRClass = GetReflectedClass<LowLevelRenderer>();
			if (pLLRClass)
			{
				TEditorElements::DrawReflectedObjectFields("LLR", pLLRClass->GetClassId(), LowLevelRenderer::Get());
			}
			ImGui::End();
		}

	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::EndGUIPass()
	{
		KEPLER_PROFILE_SCOPE();
		ImGui::Render();
		Await(TRenderThread::Submit(
			[]
			{
				auto pLLR = LowLevelRenderer::Get();
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
	void EditorModule::SetEditedWorld(RefPtr<GameWorld> pWorld)
	{
		KEPLER_PROFILE_SCOPE();
		if (m_pEditedWorld && m_pEditedWorld->IsValidEntity(m_EditorCameraEntity))
		{
			m_pEditedWorld->DestroyEntity(m_EditorCameraEntity);
			m_EditorCameraEntity = GameEntityId();
		}
		m_pEditedWorld = pWorld;
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::SelectEntity(GameEntityId id)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld || !m_pEditedWorld->IsValidEntity(id))
		{
			return;
		}

		m_SelectedEntity = id;
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::UnselectEverything()
	{
		m_SelectedEntity = GameEntityId{};
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::OnPlatformEvent(const TPlatformEventBase& event)
	{
		KEPLER_PROFILE_SCOPE();
		TPlatformEventDispatcher Dispatcher(event);
		Dispatcher.Dispatch(this, &EditorModule::OnKeyDown);
		Dispatcher.Dispatch(this, &EditorModule::OnMouseButtonDown);
		Dispatcher.Dispatch(this, &EditorModule::OnMouseButtonUp);
		Dispatcher.Dispatch(this, &EditorModule::OnMouseMove);
		Dispatcher.Dispatch(this, &EditorModule::OnMouseScroll);
	}

	void EditorModule::PostWorldInit()
	{
		if (Engine::Get()->CurrentWorldState == EWorldUpdateKind::Edit)
		{
			CreateEditorGrid();
		}

		LoadEditorViewportIcons();
	}

	void EditorModule::SaveCurrentWorld()
	{
		if (auto pWorldAsset = Engine::Get()->WorldAsset)
		{
			GameWorldSerializer serializer{ m_pEditedWorld };
			TFuture<String> json = serializer.SerializeToJson();
			TFileUtils::WriteTextFileAsync(pWorldAsset->GetPath(), Await(json));
		}
		else
		{
			SaveFileWithPicker();
		}
	}

	void EditorModule::SaveCurrentWorldAs()
	{
		SaveFileWithPicker();
	}

	void EditorModule::OpenWorld()
	{
		String assetPath;
		if (FilePickers::OpenAssetPicker(assetPath, EFieldAssetType::Map))
		{
			JsonDeserializer deserializer{ Await(TFileUtils::ReadTextFileAsync(assetPath)) };
			GameWorldDeserializer worldCreator;

			Engine::Get()->SetMainWorld(worldCreator.Deserialize(deserializer.GetRootNode()));
			Engine::Get()->WorldAsset = Await(AssetManager::Get()->FindAssetNode(assetPath));
		}
	}

	void EditorModule::LoadEditorSettings()
	{
		/*JsonSerializer serializer{};
		JsonObject& rootNode = serializer.CreateRootObject("Editor");

		if (auto pClass = GetReflectedClass<EditorModule>())
		{
			for (const auto& [name, field] : pClass->GetFields())
			{
				switch (field.GetTypeHash())
				{
					switch (field.GetTypeHash())
					{
					default:
						break;
					}
				}
			}
		}*/
	}

	void EditorModule::DumpEditorSettings()
	{

	}

	void EditorModule::LoadEditorViewportIcons()
	{
		m_CameraIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_Camera.png");
		m_AmbientLightIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_AmbientLight.png");
		m_DirectionalLightIcon = TImageLoader::Get()->LoadSamplerCached("Engine://Editor/Icons/Icon_DirectionLight.png");
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::SetupStyle()
	{
		ImGuiIO& io = ImGui::GetIO();
		const String font = VFSResolvePath("Engine://Fonts/Roboto-Regular.ttf");
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
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.26f, 0.26f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.81f, 0.41f, 0.1f, 1.00f);
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
	void EditorModule::DrawMenuBar()
	{
		KEPLER_PROFILE_SCOPE();
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File", true))
			{
				if (ImGui::MenuItem("Save", "Ctrl + S"))
				{
					SaveCurrentWorld();
					//KEPLER_INFO(LogEditor, "Serialized world: {}", json);	
				}

				if (ImGui::MenuItem("Save as", "Ctrl + Shift + S"))
				{
					SaveCurrentWorldAs();
				}

				if (ImGui::MenuItem("Load", "Ctrl + O"))
				{
					OpenWorld();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Open Material Editor", nullptr))
				{
					m_MaterialEditor->Open();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("World", true))
			{
				if (ImGui::MenuItem("Create Entity"))
				{
					auto entity = m_pEditedWorld->CreateEntity("NewEntity");
					m_SelectedEntity = entity;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void EditorModule::SaveFileWithPicker()
	{
		String outPath;
		if (FilePickers::SaveAssetPicker(outPath, EFieldAssetType::Map))
		{
			GameWorldSerializer serializer{ m_pEditedWorld };
			TFuture<String> json = serializer.SerializeToJson();
			TFileUtils::WriteTextFileAsync(outPath, Await(json));

			AssetManager::Get()->RescanAssets();
			Engine::Get()->WorldAsset = Await(AssetManager::Get()->FindAssetNode(outPath));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawViewports()
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
			auto pLLR = LowLevelRenderer::Get();

			auto pRenderTargetGroup = RenderTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
			auto pViewportSampler = pRenderTargetGroup->GetTextureSamplerAtArrayLayer(pLLR->GetFrameIndex());
			auto pImage = pViewportSampler->GetImage();
			ImGui::Image(
				(ImTextureID)pViewportSampler->GetNativeHandle(),
				ImVec2(pImage->GetWidth(), pImage->GetHeight()));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload("MAP"))
				{
					if (AssetTreeNode** ppWorldData = (AssetTreeNode**)pPayload->Data)
					{
						GameWorldDeserializer deserializer;
						RefPtr<GameWorld> pWorld = deserializer.Deserialize(JsonDeserializer{
							Await(TFileUtils::ReadTextFileAsync((*ppWorldData)->GetPath()))
							}.GetRootNode()
						);
						Engine::Get()->WorldAsset = *ppWorldData;
						Engine::Get()->SetMainWorld(pWorld);
					}
				}
				ImGui::EndDragDropTarget();
			}

			if (Engine::Get()->CurrentWorldState == EWorldUpdateKind::Edit)
			{
				DrawGizmo();
				DrawViewportEntityIcons();
				DrawViewportGizmoControls();
				DrawViewportCameraControls();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawViewportGizmoControls()
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
	void EditorModule::DrawViewportCameraControls()
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
		ImGui::DragFloat("##Sensitivity", &EditorCameraSensitivity, 0.1f, 128.0f);
		ImGui::SameLine(0.0f, 32.0f);
		ImGui::Text("Movement Speed");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(128);
		ImGui::DragFloat("##MovementSpeed", &EditorCameraSpeed, 0.1f, 128.0f);
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawDetailsPanel()
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		EntityDetailsPanel widget(m_pEditedWorld, m_SelectedEntity);
		widget.Draw();
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawSceneGraph()
	{
		KEPLER_PROFILE_SCOPE();
		if (ImGui::Begin("Scene Graph"))
		{
			i32 idx = 0;
			if (ImGui::TreeNodeEx((void*)(intptr_t)idx, ImGuiTreeNodeFlags_DefaultOpen, m_pEditedWorld->GetName().c_str()))
			{
				m_pEditedWorld->GetComponentView<TNameComponent>().each(
					[&, this](auto id, TNameComponent& NC)
					{
						ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
						auto handle = EntityHandle{ m_pEditedWorld, id };
						if (!handle->ShouldHideInSceneGraph())
						{
							bool bNodeOpen = false;
							if (m_SelectedEntity == GameEntityId{ id })
							{
								flags |= ImGuiTreeNodeFlags_Selected;
							}

							bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)idx, flags, NC.Name.c_str());
							if (ImGui::IsItemClicked())
							{
								m_SelectedEntity = GameEntityId{ id };
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
	}

	void EditorModule::DrawDebugTools()
	{
		KEPLER_PROFILE_SCOPE();
		// Draw console
		if (!m_LogPanel)
		{
			m_LogPanel = MakeShared<TLogPanel>();
		}

		m_LogPanel->Draw();
	}

	//////////////////////////////////////////////////////////////////////////
	void EditorModule::DrawGizmo()
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		if (Engine::Get()->CurrentWorldState != EWorldUpdateKind::Edit)
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
			MathCamera& camera = m_pEditedWorld->GetComponent<CameraComponent>(mainCameraId).GetCamera();

			matrix4x4 viewMatrix = camera.GenerateViewMatrix();
			matrix4x4 projMatrix = camera.GenerateProjectionMatrix();

			TransformComponent& transformComp = m_pEditedWorld->GetComponent<TransformComponent>(m_SelectedEntity);
			matrix4x4 transform = std::invoke([&]
				{
					matrix4x4 mat;
					const float3 loc = transformComp.GetLocation();
					const float3 rot = transformComp.GetRotation();
					const float3 sc = transformComp.GetScale();
					ImGuizmo::RecomposeMatrixFromComponents(&loc.x, &rot.x, &sc.x, glm::value_ptr(mat));
					return mat;
				});
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
				transformComp.SetTransform(WorldTransform{ Location, Rotation, Scale });
			}
		}
	}

	void EditorModule::ControlEditorCamera(float deltaTime)
	{
		KEPLER_PROFILE_SCOPE();
		if (!m_pEditedWorld)
		{
			return;
		}

		if (Engine::Get()->CurrentWorldState != EWorldUpdateKind::Edit)
		{
			return;
		}

		if (!m_pEditedWorld->IsValidEntity(m_EditorCameraEntity))
		{
			m_EditorCameraEntity = m_pEditedWorld->CreateCamera("_EditorCamera");
			EntityHandle camera{ m_pEditedWorld, m_EditorCameraEntity };

			camera->SetTransform(EditorCameraTransform);
			camera->SetHideInSceneGraph(true);
		}
		m_pEditedWorld->SetMainCamera(m_EditorCameraEntity);

		if (m_bIsControllingCamera)
		{
			EntityHandle camera{ m_pEditedWorld, m_EditorCameraEntity };
			ImGuiIO& IO = ImGui::GetIO();
			auto mouseDelta = TPlatform::Get()->GetMouseState().GetOffset();
			float3 rotation = camera->GetRotation();
			rotation.z -= mouseDelta.X * deltaTime * EditorCameraSensitivity;
			rotation.x -= mouseDelta.Y * deltaTime * EditorCameraSensitivity;
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
					location += camera->GetForwardVector() * deltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::S))
				{
					location -= camera->GetForwardVector() * deltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::A))
				{
					location -= camera->GetRightVector() * deltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::D))
				{
					location += camera->GetRightVector() * deltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::E))
				{
					location += camera->GetUpVector() * deltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::Q))
				{
					location -= camera->GetUpVector() * deltaTime * EditorCameraSpeed;
				}
			}
			camera->SetLocation(location);

			EditorCameraTransform = camera->GetTransform();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorModule::OnKeyDown(const TKeyDownEvent& event)
	{
		if (m_AssetBrowserPanel->IsHovered())
		{
			m_AssetBrowserPanel->OnKey(event.Key);
			return true;
		}

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
			case EKeyCode::F:
			{
				EditorCamera_FocusSelectedObject();
			}
			break;
			case EKeyCode::Delete:
			{
				if (m_pEditedWorld && m_pEditedWorld->IsValidEntity(m_SelectedEntity))
				{
					m_pEditedWorld->DestroyEntity(m_SelectedEntity);
					m_SelectedEntity = GameEntityId();
				}
			}
			break;
			case EKeyCode::S:
			{
				if (TInput::GetKey(EKeyCode::LeftControl))
				{
					if (TInput::GetKey(EKeyCode::LeftShift))
					{
						SaveCurrentWorldAs();
					}
					else
					{
						SaveCurrentWorld();
					}
				}

			}
			break;
			case EKeyCode::O:
			{
				if (TInput::GetKey(EKeyCode::LeftControl))
				{
					OpenWorld();
				}
			}
			break;
			default:
				break;
			}
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorModule::OnMouseButtonDown(const TMouseButtonDownEvent& event)
	{
		// Read entity id under render target
		if (event.Button & EMouseButton::Left)
		{
			if (m_bIsCursorInViewport && !m_bIsControllingCamera && !m_bIsGizmoHovered && !m_bIsGizmoUsed)
			{
				TrySelectEntity();
			}
		}

		if (event.Button & EMouseButton::Middle)
		{
			if (EntityHandle h{ m_pEditedWorld, m_EditorCameraEntity })
			{
				h.GetComponent<CameraComponent>()->GetCamera().SetFOV(45.0f);
			}
		}

		if (m_AssetBrowserPanel->IsHovered())
		{
			m_AssetBrowserPanel->OnMouseButton(event.Button);
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorModule::OnMouseButtonUp(const TMouseButtonUpEvent& event)
	{
		if (m_bIsControllingCamera && event.Button & EMouseButton::Right)
		{
			TPlatform::Get()->SetCursorMode(ECursorMode::Visible);
			m_bIsControllingCamera = false;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool EditorModule::OnMouseMove(const TMouseMoveEvent& event)
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
	bool EditorModule::OnMouseScroll(const TMouseScrollWheelEvent& event)
	{
		if (m_bIsControllingCamera)
		{
			if (Engine::Get()->CurrentWorldState != EWorldUpdateKind::Edit)
			{
				return false;
			}

			EditorCameraSpeed += event.Amount * GGlobalTimer->Delta() * 10.0f;
			EditorCameraSpeed = glm::clamp(EditorCameraSpeed, 0.0f, 100.0f);

			return true;
		}
		else
		{
			if (!m_bIsCursorInViewport)
			{
				return false;
			}

			if (EntityHandle handle{ m_pEditedWorld, m_EditorCameraEntity })
			{
				auto pCameraComponent = handle.GetComponent<CameraComponent>();
				auto& mathCam = pCameraComponent->GetCamera();
				auto fov = mathCam.GetFOV();
				fov -= event.Amount * GGlobalTimer->Delta() * 300.0f;
				fov = glm::clamp(fov, 10.0f, 179.0f);
				mathCam.SetFOV(fov);
			}
		}

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	float3 EditorModule::CalculateSnapVec() const
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
	void EditorModule::TrySelectEntity()
	{
		if (Engine::Get()->CurrentWorldState != EWorldUpdateKind::Edit)
		{
			return;
		}

		// Read render target
		if (RenderTargetRegistry::Get()->RenderTargetGroupExists("IdTarget"))
		{
			auto pTargetGroup = RenderTargetRegistry::Get()->GetRenderTargetGroup("IdTarget");
			RefPtr<IRenderTarget2D> pTarget = pTargetGroup->GetRenderTargetAtArrayLayer(0);
			RefPtr<IImage2D> pTargetImage = pTarget->GetImage();
			if (auto pImmCmd = LowLevelRenderer::Get()->GetRenderDevice()->GetImmediateCommandList())
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
						if (x < 0 || y < 0)
						{
							return -1;
						}
						const auto width = align / sizeof(i32);
						const auto outIndex = (u32)width * (u32)y + (u32)x;
						const i32 retVal = pData[outIndex];
						pImmCmd->UnmapImage2D(pTargetImage);
						return retVal;
					}));

				if (idColor != -1)
				{
					m_SelectedEntity = GameEntityId{ (entt::entity)idColor };
				}
				else
				{
					m_SelectedEntity = GameEntityId{ (entt::entity)entt::null };
				}
			}
		}
		else
		{
			KEPLER_WARNING(LogEditor, "Tried to access IdTarget render target, which doesn't exist");
		}
	}

	void EditorModule::CreateEditorGrid()
	{
		Array<TStaticMeshVertex> vertices;
		vertices.Reserve((usize)m_GridSize * 4);
		Array<u32> indices;
		indices.Reserve((usize)m_GridSize * 4);

		for (i32 x = 0; x < m_GridSize; ++x)
		{
			float realX = (float)x - ((float)m_GridSize * 0.5f);
			float realY = (float)m_GridSize * 0.5f;

			vertices.AppendBack(TStaticMeshVertex{ {realX, -realY, 0.0f} });
			vertices.AppendBack(TStaticMeshVertex{ {realX, realY, 0.0f} });
			vertices.AppendBack(TStaticMeshVertex{ {-realY, realX, 0.0f} });
			vertices.AppendBack(TStaticMeshVertex{ {realY,  realX, 0.0f} });
			indices.AppendBack(x * 4 + 0);
			indices.AppendBack(x * 4 + 1);
			indices.AppendBack(x * 4 + 2);
			indices.AppendBack(x * 4 + 3);
		}

		m_EditorGridEntity = m_pEditedWorld->CreateEntity("_editorgrid");
		auto gridEntity = EntityHandle{ m_pEditedWorld, m_EditorGridEntity };
		gridEntity.AddComponent<StaticMeshComponent>(vertices, indices);
		gridEntity.AddComponent<MaterialComponent>(TMaterialLoader::Get()->LoadMaterial("Engine://Editor/Materials/Grid.kmat"));
		gridEntity->SetHideInSceneGraph(true);
	}

	void EditorModule::EditorCamera_FocusSelectedObject()
	{
		if (m_pEditedWorld->IsValidEntity(m_SelectedEntity))
		{
			// TODO
		}
	}

	void EditorModule::DrawViewportEntityIcons()
	{
		KEPLER_PROFILE_SCOPE();

		if (Engine::Get()->CurrentWorldState != EWorldUpdateKind::Edit)
		{
			return;
		}

		EntityHandle editorCameraEntity = { m_pEditedWorld, m_EditorCameraEntity };
		auto view = editorCameraEntity.GetComponent<CameraComponent>()->GetCamera().GenerateViewMatrix();
		auto proj = editorCameraEntity.GetComponent<CameraComponent>()->GetCamera().GenerateProjectionMatrix();

		m_pEditedWorld->GetComponentView<CameraComponent>().each(
			[&proj, &view, &editorCameraEntity, this](entt::entity e, auto&)
			{
				if (e == m_EditorCameraEntity.Entity)
				{
					return;
				}

				DrawSelectableViewportImage(fmt::format("##gizmo{}", (u32)e).c_str(), proj, view, GameEntityId{ e }, m_CameraIcon, EViewportIndex::Viewport1);
			});

		m_pEditedWorld->GetComponentView<AmbientLightComponent>().each(
			[&proj, &view, this](auto e, auto&)
			{
				DrawSelectableViewportImage(fmt::format("##gizmo{}", (u32)e).c_str(), proj, view, GameEntityId{ e }, m_AmbientLightIcon, EViewportIndex::Viewport1);
			});


		m_pEditedWorld->GetComponentView<DirectionalLightComponent>().each(
			[&proj, &view, this](auto e, auto&)
			{
				GameEntityId entity{ e };
				DrawSelectableViewportImage(fmt::format("##gizmo{}", (u32)e).c_str(), proj, view, entity, m_DirectionalLightIcon, EViewportIndex::Viewport1);

				if (m_SelectedEntity == entity)
				{
					DrawDirections(entity);
				}
			});
	}

	void EditorModule::DrawSelectableViewportImage(const char* id, const matrix4x4& projection, const matrix4x4& view, GameEntityId entity, RefPtr<ITextureSampler2D> pIcon, EViewportIndex viewport)
	{
		KEPLER_PROFILE_SCOPE();
		EntityHandle handle{ m_pEditedWorld, GameEntityId{entity} };

		auto transform = handle->GetTransform();
		auto world = transform.GenerateWorldMatrix();
		auto mvp = projection * view * world;

		float4 v = mvp * float4(0, 0, 0, 1);

		if (glm::epsilonEqual(v.w, 0.0f, FLT_EPSILON))
		{
			v.w = 0.01f;
		}
		float3 screenSpace = v / -v.w;

		if (screenSpace.x < -m_MaxViewportIconScreenCoord || screenSpace.x > m_MaxViewportIconScreenCoord
			|| screenSpace.y < -m_MaxViewportIconScreenCoord || screenSpace.y > m_MaxViewportIconScreenCoord || v.w < 0)
		{
			return;
		}

		ImVec2 posNormalized = ImVec2(1.0f - (screenSpace.x + 1.0f) * 0.5f, (screenSpace.y + 1.0f) * 0.5f);
		auto vpSize = m_ViewportSizes[(usize)viewport];

		const auto iconSize = m_InViewportIconSize - v.w;
		if (iconSize < 0)
		{
			return;
		}

		ImVec2 iconPos = { vpSize.x * posNormalized.x - iconSize * 0.5f, vpSize.y * posNormalized.y - iconSize * 0.5f };
		ImGui::SetCursorPos(iconPos);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));

		bool bDisabled = m_bIsGizmoHovered || m_bIsGizmoUsed;
		if (bDisabled)
		{
			ImGui::Image((ImTextureID)pIcon->GetNativeHandle(), ImVec2(iconSize, iconSize), ImVec2(0.0f, 0.0f), ImVec2(1, 1), ImVec4(1, 1, 1, 0.5f));
		}
		else if (ImGui::ImageButton(id, (ImTextureID)pIcon->GetNativeHandle(), ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 0.7f)))
		{
			m_SelectedEntity = entity;
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
	}

	void EditorModule::DrawDirections(GameEntityId id)
	{
		if (Subrenderer2D* pS2D = Subrenderer2D::Get())
		{
			EntityHandle handle{ m_pEditedWorld, id };
			static constexpr float drawLineLen = 0.5f;
			pS2D->AddArrow(handle->GetLocation(), handle->GetForwardVector(), handle->GetRightVector(), 0.5f);
		}
	}

	//////////////////////////////////////////////////////////////////////////
}