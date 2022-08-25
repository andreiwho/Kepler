#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

#include <cassert>

namespace Kepler
{
	TLog* TLog::Instance = nullptr;

	TLog::TLog()
	{
		assert(!Instance);
		Instance = this;
	}

	std::shared_ptr<spdlog::logger> TLog::FindOrCreateLogger(const TString& Name)
	{
		std::lock_guard Lck{ LoggerCreationFence };
		if (Loggers.contains(Name))
		{
			return Loggers.at(Name);
		}
		return CreateLogger(Name);
	}

	std::shared_ptr<spdlog::logger> TLog::CreateLogger(const TString& Name)
	{
		std::shared_ptr<spdlog::logger> Logger = spdlog::stdout_color_mt(Name);
		if (!Logger)
		{
			return nullptr;
		}
		Loggers[Name] = Logger;
 		return ApplyDefaultLoggerConfig(Logger);
	}

	std::shared_ptr<spdlog::logger> TLog::ApplyDefaultLoggerConfig(std::shared_ptr<spdlog::logger> Logger)
	{
		Logger->set_level(spdlog::level::trace);
		Logger->set_pattern("%^|%H:%M:%S|(%n) [%l] -> %v%$");
		return Logger;
	}

}