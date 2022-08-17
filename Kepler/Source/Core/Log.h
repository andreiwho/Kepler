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

#ifdef ENABLE_LOGGING
# define KEPLER_TRACE(Channel, Format, ...)      Kepler::TLog::Trace(Channel, Format, __VA_ARGS__) 
# define KEPLER_INFO(Channel, Format, ...) 		 Kepler::TLog::Info(Channel, Format, __VA_ARGS__)
# define KEPLER_WARNING(Channel, Format, ...) 	 Kepler::TLog::Warn(Channel, Format, __VA_ARGS__)
# define KEPLER_ERROR(Channel, Format, ...) 	 Kepler::TLog::Error(Channel, Format, __VA_ARGS__)
# define KEPLER_ERROR_STOP(Channel, Format, ...)  Kepler::TLog::Error(Channel, Format, __VA_ARGS__); DEBUG_BREAK
# define KEPLER_CRITICAL(Channel, Format, ...) 	 Kepler::TLog::Critical(Channel, Format, __VA_ARGS__)
#else
# define KEPLER_TRACE(Channel, Format, ...)     
# define KEPLER_INFO(Channel, Format, ...) 		
# define KEPLER_WARNING(Channel, Format, ...) 	
# define KEPLER_ERROR(Channel, Format, ...) 	
# define KEPLER_ERROR_STOP(Channel, Format, ...)
# define KEPLER_CRITICAL(Channel, Format, ...) 	
#endif

// It has to be here... though will be moved to other file
#ifdef ENABLE_VALIDATION_BREAK
# define VALIDATED(x) [&](auto&& arg) { static bool bFired = false; if(!(x) && !bFired) { KEPLER_ERROR_STOP("VALIDATE", "Validation failed: {} on line {} in file {}", #x, __LINE__, __FILE__); bFired = true; } return arg; }(x)
# define VALIDATEDMSG(x, msg) [&](auto&& arg) { static bool bFired = false; if(!(x) && !bFired) { KEPLER_ERROR_STOP("VALIDATE", "Validation failed: {} on line {} in file {}", msg, __LINE__, __FILE__); bFired = true;} return arg; }(x)
#else
# define VALIDATED(x) x
# define VALIDATEDMSG(x, msg) x
#endif