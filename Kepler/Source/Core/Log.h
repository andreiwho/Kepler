#pragma once
#include "Core/Types.h"

#include <unordered_map>
#include <string>
#include <memory>

#include <spdlog/spdlog.h>

namespace Kepler
{
	class TLog
	{
		static TLog* Instance;

	public:
		TLog();

		static TLog* Get() { return CHECKED(Instance); }

		template<typename ... ARGS>
		static void Trace(const std::string& Channel, fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(Channel))->trace(Format, std::forward<ARGS>(Args)...);
		}

		template<typename ... ARGS>
		static void Info(const std::string& Channel, fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(Channel))->info(Format, std::forward<ARGS>(Args)...);
		}

		template<typename ... ARGS>
		static void Warn(const std::string& Channel, fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(Channel))->warn(Format, std::forward<ARGS>(Args)...);
		}

		template<typename ... ARGS>
		static void Error(const std::string& Channel, fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(Channel))->error(Format, std::forward<ARGS>(Args)...);
		}

		template<typename ... ARGS>
		static void Critical(const std::string& Channel, fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(Channel))->critical(Format, std::forward<ARGS>(Args)...);
		}

	private:
		std::shared_ptr<spdlog::logger> FindOrCreateLogger(const std::string& Name);
		std::shared_ptr<spdlog::logger> CreateLogger(const std::string& Name);
		static std::shared_ptr<spdlog::logger> ApplyDefaultLoggerConfig(std::shared_ptr<spdlog::logger> Logger);

		std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> Loggers;
	};
}