#pragma once
#include "Core/Core.h"
#include "../Misc/EditorLogReceiver.h"
#include "imgui.h"

namespace ke
{
	struct LogPanelEntry
	{
		LogPanelEntry(const String& msg, spdlog::level::level_enum level)
			:	Message(msg)
			,	Level(level)
		{}

		String Message{};
		spdlog::level::level_enum Level{};
	};

	class TLogPanel : public TEditorLogReceiver
	{
	public:
		TLogPanel();
		~TLogPanel();
		virtual void ReceiveLog(const spdlog::details::log_msg& msg, const spdlog::memory_buf_t& formatted) override;
	
		void Clear();
		void Draw();
	private:
		Array<LogPanelEntry> m_Entries;
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets;
		bool                AutoScroll;
	};
}