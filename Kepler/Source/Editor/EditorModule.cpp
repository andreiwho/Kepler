#include "EditorModule.h"

namespace Kepler
{
	TEditorModule::TEditorModule()
	{
		KEPLER_INFO(LogEditor, "Initializing editor module");
	}

	TEditorModule::~TEditorModule()
	{
		KEPLER_INFO(LogEditor, "Terminating editor module");
	}

	void TEditorModule::BeginGUIPass()
	{

	}

	void TEditorModule::EndGUIPass()
	{

	}

}