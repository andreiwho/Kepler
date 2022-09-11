#include "LogPanel.h"
#include "../Misc/EditorLogSink.h"
#include <iostream>

namespace Kepler
{

	TLogPanel::TLogPanel()
	{
		TEditorLogSink::RegisterLogReceiver(this);
	}

	TLogPanel::~TLogPanel()
	{
		TEditorLogSink::UnregisterLogReceiver(this);
	}

	void TLogPanel::ReceiveLog(const spdlog::details::log_msg& Msg, const spdlog::memory_buf_t& Formatted)
	{
		std::cout << fmt::to_string(Formatted);
	}
}