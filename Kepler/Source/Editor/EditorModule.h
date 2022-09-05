#pragma once
#include "Core/Modules/Module.h"
#include "Core/Core.h"

namespace Kepler
{
	DEFINE_UNIQUE_LOG_CHANNEL(LogEditor);

	class TEditorModule : public TApplicationModule
	{
	public:
		TEditorModule();
		~TEditorModule();

		void BeginGUIPass();
		void EndGUIPass();

	private:
	};
}