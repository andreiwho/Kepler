#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>
#include "Editor/Misc/EditorLogSink.h"

namespace ke
{
	TLog* TLog::Instance = nullptr;

	TLog::TLog()
	{
		assert(!Instance);
		Instance = this;
	}

	std::shared_ptr<spdlog::logger> TLog::FindOrCreateLogger(const TString& name, ELogLevel level)
	{
		std::lock_guard lck{ m_LoggerCreationFence };
		if (m_Loggers.contains(name))
		{
			return m_Loggers.at(name);
		}
		return CreateLogger(name, level);
	}

	std::shared_ptr<spdlog::logger> TLog::CreateLogger(const TString& name, ELogLevel level)
	{
#ifdef ENABLE_EDITOR
		if (!m_EditorSink)
		{
			m_EditorSink = std::make_shared<TEditorLogSink>();
		}
#endif
		std::shared_ptr<spdlog::logger> pLogger = spdlog::stdout_color_mt(name);
		if (!pLogger)
		{
			return nullptr;
		}
		m_Loggers[name] = pLogger;
#ifdef ENABLE_EDITOR
		pLogger->sinks().push_back(m_EditorSink);
#endif
 		return ApplyDefaultLoggerConfig(pLogger, level);
	}

	std::shared_ptr<spdlog::logger> TLog::ApplyDefaultLoggerConfig(std::shared_ptr<spdlog::logger> pLogger, ELogLevel level)
	{
		switch (level)
		{
		case ke::ELogLevel::All:
			pLogger->set_level(spdlog::level::trace);
			break;
		case ke::ELogLevel::Info:
			pLogger->set_level(spdlog::level::info);
			break;
		case ke::ELogLevel::Warning:
			pLogger->set_level(spdlog::level::warn);
			break;
		case ke::ELogLevel::Error:
			pLogger->set_level(spdlog::level::err);
			break;
		case ke::ELogLevel::Critical:
			pLogger->set_level(spdlog::level::critical);
			break;
		default:
			break;
		}
		pLogger->set_pattern("%^|%H:%M:%S|(%n) [%l] -> %v%$");
		return pLogger;
	}

}