#include "EditorLogSink.h"
#include "EditorLogReceiver.h"

namespace ke
{
	
	void TEditorLogSink::sink_it_(const spdlog::details::log_msg& Msg)
	{
		if (!HasLogReceiver())
		{
			return;
		}

		spdlog::memory_buf_t Formatted;
		spdlog::sinks::base_sink<std::mutex>::formatter_->format(Msg, Formatted);
		Receiver->ReceiveLog(Msg, Formatted);
	}

	void TEditorLogSink::flush_()
	{
		// NoOp
	}

	void TEditorLogSink::RegisterLogReceiver(TEditorLogReceiver* pReceiver)
	{
		Receiver = pReceiver;
	}

	void TEditorLogSink::UnregisterLogReceiver(TEditorLogReceiver* pReceiver)
	{
		Receiver = nullptr;
	}

	bool TEditorLogSink::HasLogReceiver()
	{
		return !!Receiver;
	}

	ke::TEditorLogReceiver* TEditorLogSink::Receiver;
}