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

	class TEditorModule : public TApplicationModule
	{
	public:
		TEditorModule(TWindow* pWindow);
		~TEditorModule();

		float2 GetViewportSize(EViewportIndex Index);

		void BeginGUIPass();
		void DrawEditor();
		void EndGUIPass();
		void SetEditedWorld(TRef<TGameWorld> InWorld);
		void SelectEntity(TGameEntityId Id);
		void UnselectEverything();

	private:
		void SetupStyle();
		void DrawMenuBar();
		void DrawViewports();
		void DrawDetailsPanel();

	private:
		TWindow* MainWindow{};
		float2 ViewportSizes[(u32)EViewportIndex::Max]{};
		TRef<TGameWorld> EditedWorld{};
		TGameEntityId SelectedEntity{};
	};
}