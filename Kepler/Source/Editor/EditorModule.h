#pragma once
#include "Core/Modules/Module.h"
#include "Core/Core.h"
#include "World/Game/GameWorld.h"
#include "Panels/AssetBrowserPanel.h"

namespace ke
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogEditor, All);

	class TWindow;
	class TLogPanel;

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

	class EditorModule : public EngineModule
	{
	public:
		EditorModule(TWindow* pWindow);
		~EditorModule();

		float2 GetViewportSize(EViewportIndex idx);

		void BeginGUIPass();
		void DrawEditor();
		virtual void OnUpdate(float deltaTime) override;
		void EndGUIPass();
		void SetEditedWorld(TRef<TGameWorld> pWorld);
		void SelectEntity(TGameEntityId id);
		void UnselectEverything();
		EViewportIndex GetHoveredViewport() const { return m_HoveredViewport; }

		virtual void OnPlatformEvent(const TPlatformEventBase& event) override;

	protected:
		virtual void PostWorldInit() override;

	private:
		void SetupStyle();
		void DrawMenuBar();
		void DrawViewports();
		void DrawViewportGizmoControls();
		void DrawViewportCameraControls();
		void DrawDetailsPanel();
		void DrawSceneGraph();
		void DrawDebugTools();
		void DrawGizmo();
		void ControlEditorCamera(float deltaTime);

		bool OnKeyDown(const TKeyDownEvent& event);
		bool OnMouseButtonDown(const TMouseButtonDownEvent& event);
		bool OnMouseButtonUp(const TMouseButtonUpEvent& event);
		bool OnMouseMove(const TMouseMoveEvent& event);
		float3 CalculateSnapVec() const;

		void TrySelectEntity();

		void CreateEditorGrid();

		void EditorCamera_FocusSelectedObject();
	private:
		TWindow* m_pMainWindow{};
		float2 m_ViewportSizes[(u32)EViewportIndex::Max]{};
		float2 m_ViewportPositions[(u32)EViewportIndex::Max]{};

		TRef<TGameWorld> m_pEditedWorld{};
		TGameEntityId m_SelectedEntity{};
		EViewportIndex m_HoveredViewport = EViewportIndex::Max;
		TGameEntityId m_EditorCameraEntity{};
		bool m_bIsControllingCamera = false;
		bool m_bIsCursorInViewport = false;
		bool m_bIsGizmoHovered = false;
		bool m_bIsGizmoUsed = false;

		float m_EditorCameraSensitivity = 32;
		float m_EditorCameraSpeed = 2.0f;

		TSharedPtr<TLogPanel> m_LogPanel;
		TSharedPtr<TAssetBrowserPanel> m_AssetBrowserPanel;

	private:
		i32 m_EditOperationIndex = 0x7;	// Translate by default
		i32 m_EditSpaceIndex = 0x1;		// World by default
		float2 m_ViewportToolbarOffset = float2(8, 8);

		bool m_bSnapEnabled = false;
		ETranslationSnap m_TranslationSnap = ETranslationSnap::_1Unit;
		ERotationSnap m_RotationSnap = ERotationSnap::_1Degree;

		TGameEntityId m_EditorGridEntity{};
		const i32 m_GridSize = 1000;
	};
}