#pragma once
#include "../Misc/EditorLogReceiver.h"

namespace Kepler
{
	class TLogPanel : public TEditorLogReceiver
	{
	public:
		TLogPanel();
		~TLogPanel();
		virtual void ReceiveLog(const spdlog::details::log_msg& Msg, const spdlog::memory_buf_t& Formatted) override;
	
		void Draw();

	};
}