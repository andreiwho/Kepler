#pragma once
#include <spdlog/sinks/base_sink.h>

namespace Kepler
{
	class TEditorLogReceiver;

	class TEditorLogSink : public spdlog::sinks::base_sink<std::mutex>
	{
	protected:
		virtual void sink_it_(const spdlog::details::log_msg& Msg) override;
		virtual void flush_() override;

	public:
		static void RegisterLogReceiver(TEditorLogReceiver* pReceiver);
		static void UnregisterLogReceiver(TEditorLogReceiver* pReceiver);
		static bool HasLogReceiver();

	private:
		static TEditorLogReceiver* Receiver;
	};
}