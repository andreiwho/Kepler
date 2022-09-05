#pragma once
#include "Core/Modules/Module.h"
#include "Core/Core.h"

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

	private:
		void SetupStyle();
		void DrawViewports();

	private:
		float2 ViewportSizes[(u32)EViewportIndex::Max]{};
	};
}