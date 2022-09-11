#pragma once
#include "Core/Log.h"

namespace Kepler
{
	class TEditorLogReceiver
	{
	public:
		virtual void ReceiveLog(const spdlog::details::log_msg& Msg, const spdlog::memory_buf_t& Formatted) {}
	};
}