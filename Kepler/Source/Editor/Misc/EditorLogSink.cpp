#include "EditorLogSink.h"
#include "EditorLogReceiver.h"

namespace ke
{
	
	void TEditorLogSink::sink_it_(const spdlog::details::log_msg& msg)
	{
		if (!HasLogReceiver())
		{
			return;
		}

		spdlog::memory_buf_t formatted;
		spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
		s_Receiver->ReceiveLog(msg, formatted);
	}

	void TEditorLogSink::flush_()
	{
		// NoOp
	}

	void TEditorLogSink::RegisterLogReceiver(TEditorLogReceiver* pReceiver)
	{
		s_Receiver = pReceiver;
	}

	void TEditorLogSink::UnregisterLogReceiver(TEditorLogReceiver* pReceiver)
	{
		s_Receiver = nullptr;
	}

	bool TEditorLogSink::HasLogReceiver()
	{
		return !!s_Receiver;
	}

	ke::TEditorLogReceiver* TEditorLogSink::s_Receiver;
}