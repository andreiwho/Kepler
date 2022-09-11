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
#include "ImGuizmo.h"
#include "World/Camera/CameraComponent.h"
#include "World/Game/Components/TransformComponent.h"
#include "imgui_internal.h"
#include "Core/Timer.h"
#include "Platform/Input.h"
#include "Platform/Platform.h"

namespace Kepler
{
	//////////////////////////////////////////////////////////////////////////
	TEditorModule::TEditorModule(TWindow* pWindow)
		: MainWindow(pWindow)
	{
		for (auto& Size : ViewportSizes)
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
		Await(TRenderThread::Submit([]
			{
				ImGui_ImplDX11_Shutdown();
			}));
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
		ImGuizmo::BeginFrame();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawEditor()
	{
		DrawMenuBar();
		// DrawGizmo();
		DrawViewports();
		DrawDetailsPanel();
		DrawSceneGraph();
		ImGui::ShowDemoWindow();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::OnUpdate(float DeltaTime)
	{
		ControlEditorCamera(DeltaTime);
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
	void TEditorModule::SetEditedWorld(TRef<TGameWorld> InWorld)
	{
		EditedWorld = InWorld;
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::SelectEntity(TGameEntityId Id)
	{
		if (!EditedWorld || !EditedWorld->IsValidEntity(Id))
		{
			return;
		}

		SelectedEntity = Id;
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::UnselectEverything()
	{
		SelectedEntity = TGameEntityId{};
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::OnPlatformEvent(const TPlatformEventBase& Event)
	{
		TPlatformEventDispatcher Dispatcher(Event);
		Dispatcher.Dispatch(this, &TEditorModule::OnKeyDown);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseButtonDown);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseButtonUp);
		Dispatcher.Dispatch(this, &TEditorModule::OnMouseMove);
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::SetupStyle()
	{
		ImGuiIO& IO = ImGui::GetIO();
		const TString Font = VFSResolvePath("Engine://Fonts/Roboto-Regular.ttf");
		IO.Fonts->AddFontFromFileTTF(Font.c_str(), 15.0f);

		auto& Style = ImGui::GetStyle();
		ImVec4* Colors = Style.Colors;

		// TEXT
		Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		
		// WINDOWS
		Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.94f);
		Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		Colors[ImGuiCol_Border] = ImVec4(0.2f, 0.2f, 0.2f, 0.50f);
		Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		
		// FRAMES
		Colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.54f);
		Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.40f);
		Colors[ImGuiCol_FrameBgActive] = ImVec4(0.7f, 0.7f, 0.7f, 0.67f);
		
		// TITLES
		Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);


		Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		Colors[ImGuiCol_CheckMark] = ImVec4(0.81f, 0.41f, 0.1f, 1.00f);
		Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
		Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		Colors[ImGuiCol_Separator] = Colors[ImGuiCol_Border];
		Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		Colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
		Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		Colors[ImGuiCol_Tab] = ImLerp(Colors[ImGuiCol_Header], Colors[ImGuiCol_TitleBgActive], 0.80f);
		Colors[ImGuiCol_TabHovered] = Colors[ImGuiCol_HeaderHovered];
		Colors[ImGuiCol_TabActive] = ImLerp(Colors[ImGuiCol_HeaderActive], Colors[ImGuiCol_TitleBgActive], 0.60f);
		Colors[ImGuiCol_TabUnfocused] = ImLerp(Colors[ImGuiCol_Tab], Colors[ImGuiCol_TitleBg], 0.80f);
		Colors[ImGuiCol_TabUnfocusedActive] = ImLerp(Colors[ImGuiCol_TabActive], Colors[ImGuiCol_TitleBg], 0.40f);
		Colors[ImGuiCol_DockingPreview] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		//		ImGui::StyleColorsDark();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File", true))
			{
				ImGui::MenuItem("Noop", "No + Op");
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawViewports()
	{
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport 1");
		{
			if (ImGui::IsWindowHovered())
			{
				HoveredViewport = EViewportIndex::Viewport1;
				bIsCursorInViewport = true;
			}
			else
			{
				bIsCursorInViewport = false;
			}

			ImVec2 Region = ImGui::GetContentRegionAvail();

			ViewportSizes[(u32)EViewportIndex::Viewport1] = float2(Region.x, Region.y);
			auto LLR = TLowLevelRenderer::Get();

			auto RenderTargetGroup = TTargetRegistry::Get()->GetRenderTargetGroup("EditorViewport");
			auto ViewportSampler = RenderTargetGroup->GetTextureSamplerAtArrayLayer(LLR->GetFrameIndex());
			auto Image = ViewportSampler->GetImage();
			ImGui::Image(
				(ImTextureID)ViewportSampler->GetNativeHandle(),
				ImVec2(Image->GetWidth(), Image->GetHeight()));

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
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImGui::SetCursorScreenPos(ImVec2(vMin.x + ViewportToolbarOffset.x, vMin.y + ViewportToolbarOffset.y));

		ImGui::BeginGroup();
		ImGui::Text("Operation");
		ImGui::SameLine();
		if (ImGui::RadioButton("Translate", EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE))
		{
			EditOperationIndex = ImGuizmo::OPERATION::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", EditOperationIndex == ImGuizmo::OPERATION::ROTATE))
		{
			EditOperationIndex = ImGuizmo::OPERATION::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", EditOperationIndex == ImGuizmo::OPERATION::SCALE))
		{
			EditOperationIndex = ImGuizmo::OPERATION::SCALE;
		}

		if (EditOperationIndex != ImGuizmo::OPERATION::SCALE)
		{
			ImGui::SameLine(0.0f, 32);
			ImGui::Text("Mode");
			ImGui::SameLine();
			if (ImGui::RadioButton("World", EditSpaceIndex == ImGuizmo::MODE::WORLD))
			{
				EditSpaceIndex = ImGuizmo::MODE::WORLD;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Local", EditSpaceIndex == ImGuizmo::MODE::LOCAL))
			{
				EditSpaceIndex = ImGuizmo::MODE::LOCAL;
			}

			// Snapping
			ImGui::SameLine(0.0f, 32);
			ImGui::Checkbox("Snap", &bSnapEnabled);

			if (bSnapEnabled)
			{
				ImGui::SameLine();
				if (EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE)
				{
					const char* pEntries = " 1\0 2\0 5\0 10\0 25\0 50\0 100\0";

					i32 CurrentValue = (i32)TranslationSnap;
					ImGui::SetNextItemWidth(50.0f);
					if (ImGui::Combo("##v", &CurrentValue, pEntries))
					{
						TranslationSnap = (ETranslationSnap)CurrentValue;
					}
				}
				if (EditOperationIndex == ImGuizmo::OPERATION::ROTATE)
				{
					const char* pEntries = " 1\0 10\0 30\0 45\0 90\0 180\0 ";

					i32 CurrentValue = (i32)RotationSnap;
					ImGui::SetNextItemWidth(50.0f);
					if (ImGui::Combo("##v", &CurrentValue, pEntries))
					{
						RotationSnap = (ERotationSnap)CurrentValue;
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
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		ImGui::SetCursorScreenPos(ImVec2(vMin.x + ViewportToolbarOffset.x, vMax.y - ImGui::GetTextLineHeight() - ViewportToolbarOffset.y));
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
	void TEditorModule::DrawDetailsPanel()
	{
		if (!EditedWorld)
		{
			return;
		}

		TEditorDetailsPanel Widget(EditedWorld, SelectedEntity);
		Widget.Draw();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawSceneGraph()
	{
		ImGui::Begin("Scene Graph");
		i32 Index = 0;
		if (ImGui::TreeNodeEx((void*)(intptr_t)Index, ImGuiTreeNodeFlags_DefaultOpen, EditedWorld->GetName().c_str()))
		{
			EditedWorld->GetComponentView<TNameComponent, TGameEntity>().each(
				[&, this](auto EntityId, TNameComponent& NC, TGameEntity& GE)
				{
					ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_None;
					if (!GE.ShouldHideInSceneGraph())
					{
						bool bNodeOpen = false;
						if (SelectedEntity == TGameEntityId{ EntityId })
						{
							NodeFlags |= ImGuiTreeNodeFlags_Selected;
						}

						bNodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)Index, NodeFlags, NC.Name.c_str());
						if (ImGui::IsItemClicked())
						{
							SelectedEntity = TGameEntityId{ EntityId };
						}

						if (bNodeOpen)
						{
							ImGui::TreePop();
						}

						Index++;
					}
				});
			ImGui::TreePop();
		}
		ImGui::End();
	}

	//////////////////////////////////////////////////////////////////////////
	void TEditorModule::DrawGizmo()
	{
		if (!EditedWorld)
		{
			return;
		}

		if (HoveredViewport == EViewportIndex::Max)
		{
			return;
		}

		if (EditedWorld->IsValidEntity(SelectedEntity))
		{
			auto MainCameraId = EditedWorld->GetMainCamera();
			// auto& MainCameraEntity = EditedWorld->GetEntityFromId(SelectedEntity);
			TCamera& Camera = EditedWorld->GetComponent<TCameraComponent>(MainCameraId).GetCamera();

			matrix4x4 ViewMatrix = Camera.GenerateViewMatrix();
			matrix4x4 ProjectionMatrix = Camera.GenerateProjectionMatrix();

			TTransformComponent& TransformComponent = EditedWorld->GetComponent<TTransformComponent>(SelectedEntity);
			matrix4x4 Transform = TransformComponent.GetTransform().GenerateWorldMatrix();

			ImGuizmo::SetDrawlist();
			// Snapping
			float3 SnapVec = CalculateSnapVec();

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0, 0, GetViewportSize(HoveredViewport).x, GetViewportSize(HoveredViewport).y);
			if (ImGuizmo::Manipulate(glm::value_ptr(ViewMatrix), glm::value_ptr(ProjectionMatrix),
				static_cast<ImGuizmo::OPERATION>(EditOperationIndex),
				static_cast<ImGuizmo::MODE>(EditSpaceIndex),
				glm::value_ptr(Transform),
				nullptr,
				bSnapEnabled ? &SnapVec.x : nullptr))
			{
				float3 Location, Rotation, Scale;
				ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(Transform), &Location.x, &Rotation.x, &Scale.x);
				TransformComponent.SetTransform(TWorldTransform{ Location, Rotation, Scale });
			}
		}
	}

	void TEditorModule::ControlEditorCamera(float DeltaTime)
	{
		if (!EditedWorld)
		{
			return;
		}

		if (!EditedWorld->IsValidEntity(EditorCameraEntity))
		{
			EditorCameraEntity = EditedWorld->CreateCamera("_EditorCamera");
			TTransformComponent& CameraTransform = EditedWorld->GetComponent<TTransformComponent>(EditorCameraEntity);
			CameraTransform.SetLocation(float3(0.0f, -3.0f, 0.0f));
			EditedWorld->GetEntityFromId(EditorCameraEntity).SetHideInSceneGraph(true);
			// sCameraTransform.SetRotation(float3(0.0f, 0.0f, 90.0f));
		}
		EditedWorld->SetMainCamera(EditorCameraEntity);

		if (bIsControllingCamera)
		{
			TTransformComponent& CameraTransform = EditedWorld->GetComponent<TTransformComponent>(EditorCameraEntity);
			ImGuiIO& IO = ImGui::GetIO();
			auto MouseDelta = TPlatform::Get()->GetMouseState().GetOffset();
			float3 Rotation = CameraTransform.GetRotation();
			Rotation.z -= MouseDelta.X * DeltaTime * EditorCameraSensitivity;
			Rotation.x -= MouseDelta.Y * DeltaTime * EditorCameraSensitivity;
			CameraTransform.SetRotation(Rotation);

			auto Location = CameraTransform.GetLocation();
			if (TInput::GetKey(EKeyCode::LeftAlt))
			{
				// Orbiting camera
			}
			else
			{
				// First person camera
				if (TInput::GetKey(EKeyCode::W))
				{
					Location += CameraTransform.GetForwardVector() * DeltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::S))
				{
					Location -= CameraTransform.GetForwardVector() * DeltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::A))
				{
					Location -= CameraTransform.GetRightVector() * DeltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::D))
				{
					Location += CameraTransform.GetRightVector() * DeltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::E))
				{
					Location += CameraTransform.GetUpVector() * DeltaTime * EditorCameraSpeed;
				}
				if (TInput::GetKey(EKeyCode::Q))
				{
					Location -= CameraTransform.GetUpVector() * DeltaTime * EditorCameraSpeed;
				}
			}
			CameraTransform.SetLocation(Location);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnKeyDown(const TKeyDownEvent& InEvent)
	{
		if (bIsControllingCamera)
		{

		}
		else
		{
			switch (InEvent.Key)
			{
				// Switch gizmos
			case EKeyCode::W:
			{
				EditOperationIndex = ImGuizmo::OPERATION::TRANSLATE;
			} break;
			case EKeyCode::E:
			{
				EditOperationIndex = ImGuizmo::OPERATION::ROTATE;
			} break;
			case EKeyCode::R:
			{
				EditOperationIndex = ImGuizmo::OPERATION::SCALE;
				if (bSnapEnabled)
				{
					static std::once_flag Once;
					std::call_once(Once, [] { KEPLER_WARNING(LogEditor, "Gizmo scaling currently doesn't support snapping"); });
				}
			} break;
			// Switch spaces
			case EKeyCode::T:
			{
				const ImGuizmo::MODE Mode = static_cast<ImGuizmo::MODE>(EditSpaceIndex);
				if (Mode == ImGuizmo::MODE::LOCAL)
				{
					EditSpaceIndex = ImGuizmo::MODE::WORLD;
				}
				else
				{
					EditSpaceIndex = ImGuizmo::MODE::LOCAL;
				}
			} break;
			case EKeyCode::Q:
			{
				bSnapEnabled = !bSnapEnabled;
			}
			break;
			default:
				break;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseButtonDown(const TMouseButtonDownEvent& InEvent)
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseButtonUp(const TMouseButtonUpEvent& InEvent)
	{
		if (bIsControllingCamera && InEvent.Button & EMouseButton::Right)
		{
			TPlatform::Get()->SetCursorMode(ECursorMode::Visible);
			bIsControllingCamera = false;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool TEditorModule::OnMouseMove(const TMouseMoveEvent& InEvent)
	{
		// Start moving camera only if mouse button pressed and mouse is dragged, otherwise, use context menu (probably...)
		if (TInput::GetMouseButon(EMouseButton::Right) && bIsCursorInViewport)
		{
			TPlatform::Get()->SetCursorMode(ECursorMode::HiddenLocked);
			bIsControllingCamera = true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	float3 TEditorModule::CalculateSnapVec() const
	{
		if (bSnapEnabled)
		{
			if (EditOperationIndex == ImGuizmo::OPERATION::TRANSLATE)
			{
				switch (TranslationSnap)
				{
				case Kepler::ETranslationSnap::_1Unit:
					return float3(0.01f);
					break;
				case Kepler::ETranslationSnap::_2Units:
					return float3(0.02f);
					break;
				case Kepler::ETranslationSnap::_5Units:
					return float3(0.05f);
					break;
				case Kepler::ETranslationSnap::_10Units:
					return float3(0.1f);
					break;
				case Kepler::ETranslationSnap::_25Units:
					return float3(0.25f);
					break;
				case Kepler::ETranslationSnap::_50Units:
					return float3(0.5f);
					break;
				case Kepler::ETranslationSnap::_100Units:
					return float3(1.0f);
					break;
				default:
					break;
				}
			}
			if (EditOperationIndex == ImGuizmo::OPERATION::ROTATE)
			{
				switch (RotationSnap)
				{
				case Kepler::ERotationSnap::_1Degree:
					return float3(1.0f);
					break;
				case Kepler::ERotationSnap::_10Degrees:
					return float3(10.0f);
					break;
				case Kepler::ERotationSnap::_30Degrees:
					return float3(30.0f);
					break;
				case Kepler::ERotationSnap::_45Degrees:
					return float3(45.0f);
					break;
				case Kepler::ERotationSnap::_90Degrees:
					return float3(90.0f);
					break;
				case Kepler::ERotationSnap::_180Degrees:
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
}