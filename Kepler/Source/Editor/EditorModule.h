#pragma once
#include "Core/Modules/Module.h"
#include "Core/Core.h"
#include "World/Game/GameWorld.h"
#include "Panels/AssetBrowserPanel.h"
#include "Renderer/World/WorldTransform.h"

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
		void SetEditedWorld(RefPtr<GameWorld> pWorld);
		void SelectEntity(GameEntityId id);
		void UnselectEverything();
		EViewportIndex GetHoveredViewport() const { return m_HoveredViewport; }

		virtual void OnPlatformEvent(const TPlatformEventBase& event) override;

	protected:
		virtual void PostWorldInit() override;

	private:
		void LoadEditorSettings();
		void DumpEditorSettings();
		void LoadEditorViewportIcons();
		void SetupStyle();
		void DrawMenuBar();

		void SaveFileWithPicker();

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
		bool OnMouseScroll(const TMouseScrollWheelEvent& event);

		void DrawEngineInfo();

		float3 CalculateSnapVec() const;

		void TrySelectEntity();
		void CreateEditorGrid();
		void EditorCamera_FocusSelectedObject();

		void DrawViewportEntityIcons();
		void DrawSelectableViewportImage(const char* id, const matrix4x4& projection, const matrix4x4& view, GameEntityId entity, RefPtr<ITextureSampler2D> pIcon, EViewportIndex viewport);
		void DrawDirections(GameEntityId id);

	private:
		TWindow* m_pMainWindow{};
		float2 m_ViewportSizes[(u32)EViewportIndex::Max]{};
		float2 m_ViewportPositions[(u32)EViewportIndex::Max]{};

		RefPtr<GameWorld> m_pEditedWorld{};
		GameEntityId m_SelectedEntity{};
		EViewportIndex m_HoveredViewport = EViewportIndex::Max;
		GameEntityId m_EditorCameraEntity{};
		bool m_bIsControllingCamera = false;
		bool m_bIsCursorInViewport = false;
		bool m_bIsGizmoHovered = false;
		bool m_bIsGizmoUsed = false;

		SharedPtr<TLogPanel> m_LogPanel;
		SharedPtr<TAssetBrowserPanel> m_AssetBrowserPanel;

	private:
		i32 m_EditOperationIndex = 0x7;	// Translate by default
		i32 m_EditSpaceIndex = 0x1;		// World by default
		float2 m_ViewportToolbarOffset = float2(8, 8);

		bool m_bSnapEnabled = false;
		ETranslationSnap m_TranslationSnap = ETranslationSnap::_1Unit;
		ERotationSnap m_RotationSnap = ERotationSnap::_1Degree;

		GameEntityId m_EditorGridEntity{};
		const i32 m_GridSize = 1000;
		float m_InViewportIconSize = 50.0;

		// Entity icons
		RefPtr<ITextureSampler2D> m_CameraIcon;
		RefPtr<ITextureSampler2D> m_AmbientLightIcon;
		RefPtr<ITextureSampler2D> m_DirectionalLightIcon;
		float m_MaxViewportIconScreenCoord = 0.9f;

	public:
		WorldTransform EditorCameraTransform{};
		float EditorCameraSensitivity = 32;
		float EditorCameraSpeed = 2.0f;
	};
}