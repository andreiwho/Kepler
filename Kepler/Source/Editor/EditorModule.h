#pragma once
#include "Core/Modules/Module.h"
#include "Core/Core.h"
#include "World/Game/GameWorld.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogEditor);

	class TWindow;

	enum class EViewportIndex
	{
		Viewport1 = 0,
		Max,
	};

	enum class ETranslationSnap
	{
		_1Unit,
		_2Units,
		_5Units,
		_10Units,
		_25Units,
		_50Units,
		_100Units,
	};

	enum class ERotationSnap
	{
		_1Degree,
		_10Degrees,
		_30Degrees,
		_45Degrees,
		_90Degrees,
		_180Degrees,
	};

	class TEditorModule : public TApplicationModule
	{
	public:
		TEditorModule(TWindow* pWindow);
		~TEditorModule();

		float2 GetViewportSize(EViewportIndex Index);

		void BeginGUIPass();
		void DrawEditor();
		virtual void OnUpdate(float DeltaTime) override;
		void EndGUIPass();
		void SetEditedWorld(TRef<TGameWorld> InWorld);
		void SelectEntity(TGameEntityId Id);
		void UnselectEverything();
		EViewportIndex GetHoveredViewport() const { return HoveredViewport; }

		virtual void OnPlatformEvent(const TPlatformEventBase& Event) override;

	private:
		void SetupStyle();
		void DrawMenuBar();
		void DrawViewports();
		void DrawViewportGizmoControls();
		void DrawViewportCameraControls();
		void DrawDetailsPanel();
		void DrawGizmo();
		void ControlEditorCamera(float DeltaTime);

		bool OnKeyDown(const TKeyDownEvent& InEvent);
		bool OnMouseButtonDown(const TMouseButtonDownEvent& InEvent);
		bool OnMouseButtonUp(const TMouseButtonUpEvent& InEvent);
		bool OnMouseMove(const TMouseMoveEvent& InEvent);
		float3 CalculateSnapVec() const;

	private:
		TWindow* MainWindow{};
		float2 ViewportSizes[(u32)EViewportIndex::Max]{};
		TRef<TGameWorld> EditedWorld{};
		TGameEntityId SelectedEntity{};
		EViewportIndex HoveredViewport = EViewportIndex::Max;
		TGameEntityId EditorCameraEntity{};
		bool bIsControllingCamera = false;
		bool bIsCursorInViewport = false;
		float EditorCameraSensitivity = 32;
		float EditorCameraSpeed = 2.0f;

	private:
		i32 EditOperationIndex = 0x7;	// Translate by default
		i32 EditSpaceIndex = 0x1;		// World by default
		float2 ViewportToolbarOffset = float2(8, 8);

		bool bSnapEnabled = false;
		ETranslationSnap TranslationSnap = ETranslationSnap::_1Unit;
		ERotationSnap RotationSnap = ERotationSnap::_1Degree;
	};
}