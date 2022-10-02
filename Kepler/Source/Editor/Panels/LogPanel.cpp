#include "LogPanel.h"
#include "../Misc/EditorLogSink.h"
#include <iostream>

namespace ke
{

	TLogPanel::TLogPanel()
	{
		TEditorLogSink::RegisterLogReceiver(this);
	}

	TLogPanel::~TLogPanel()
	{
		TEditorLogSink::UnregisterLogReceiver(this);
	}

	void TLogPanel::ReceiveLog(const spdlog::details::log_msg& msg, const spdlog::memory_buf_t& formatted)
	{
	}

	void TLogPanel::Draw()
	{

	}
}