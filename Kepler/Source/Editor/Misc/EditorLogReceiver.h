#pragma once
#include "Core/Log.h"

namespace ke
{
	class TEditorLogReceiver
	{
	public:
		virtual void ReceiveLog(const spdlog::details::log_msg& msg, const spdlog::memory_buf_t& formatteed) {}
	};
}