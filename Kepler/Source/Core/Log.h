#pragma once
#include "Core/Types.h"

#include <unordered_map>
#include <string>
#include <memory>

#include <spdlog/spdlog.h>

namespace ke
{
	// C++ 20 supports compile time std::string
#define PRODUCE_INTERNAL_LOG_NAME(Name) T ## Name ## LogChannel
#define PRODUCE_INTERNAL_LOG_NAME_STRING(Name) #Name
#define DEFINE_UNIQUE_LOG_CHANNEL(Name)\
	struct PRODUCE_INTERNAL_LOG_NAME(Name)															\
	{																								\
		static constexpr const char* Internal_GetStringId__() {return PRODUCE_INTERNAL_LOG_NAME_STRING(Name);}	\
	}

	class TLog
	{
		static TLog* Instance;

	public:
		TLog();

		static TLog* Get() { return CHECKED(Instance); }

		template<typename TChannel, typename ... ARGS>
		static void Trace(fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(TChannel::Internal_GetStringId__()))->trace(Format, std::forward<ARGS>(Args)...);
		}

		template<typename TChannel, typename ... ARGS>
		static void Info(fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(TChannel::Internal_GetStringId__()))->info(Format, std::forward<ARGS>(Args)...);
		}

		template<typename TChannel, typename ... ARGS>
		static void Warn(fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(TChannel::Internal_GetStringId__()))->warn(Format, std::forward<ARGS>(Args)...);
		}

		template<typename TChannel, typename ... ARGS>
		static void Error(fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(TChannel::Internal_GetStringId__()))->error(Format, std::forward<ARGS>(Args)...);
		}

		template<typename TChannel, typename ... ARGS>
		static void Critical(fmt::format_string<ARGS...> Format, ARGS&&... Args)
		{
			CHECKED(Get()->FindOrCreateLogger(TChannel::Internal_GetStringId__()))->critical(Format, std::forward<ARGS>(Args)...);
		}

	private:
		std::shared_ptr<spdlog::logger> FindOrCreateLogger(const TString& Name);
		std::shared_ptr<spdlog::logger> CreateLogger(const TString& Name);
		static std::shared_ptr<spdlog::logger> ApplyDefaultLoggerConfig(std::shared_ptr<spdlog::logger> Logger);

		std::unordered_map<TString, std::shared_ptr<spdlog::logger>> Loggers;
		std::mutex LoggerCreationFence;

#ifdef ENABLE_EDITOR
		std::shared_ptr<class TEditorLogSink> EditorSink;
#endif
	};
}

#ifdef ENABLE_LOGGING
# define KEPLER_TRACE(Channel, Format, ...)      ke::TLog::Trace<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__) 
# define KEPLER_INFO(Channel, Format, ...) 		 ke::TLog::Info<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__)
# define KEPLER_WARNING(Channel, Format, ...) 	 ke::TLog::Warn<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__)
# define KEPLER_ERROR(Channel, Format, ...) 	 ke::TLog::Error<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__)
# define KEPLER_ERROR_STOP(Channel, Format, ...)  ke::TLog::Error<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__); DEBUG_BREAK
# define KEPLER_CRITICAL(Channel, Format, ...) 	 ke::TLog::Critical<PRODUCE_INTERNAL_LOG_NAME(Channel)>(Format, __VA_ARGS__)
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
DEFINE_UNIQUE_LOG_CHANNEL(VALIDATE);
# define VALIDATED(x) [&](auto&& arg) { static bool bFired = false; if(!(x) && !bFired) { KEPLER_ERROR_STOP(VALIDATE, "Validation failed: {} on line {} in file {}", #x, __LINE__, __FILE__); bFired = true; } return arg; }(x)
# define VALIDATEDMSG(x, msg) [&](auto&& arg) { static bool bFired = false; if(!(x) && !bFired) { KEPLER_ERROR_STOP(VALIDATE, "Validation failed: {} on line {} in file {}", msg, __LINE__, __FILE__); bFired = true;} return arg; }(x)
#else
# define VALIDATED(x) x
# define VALIDATEDMSG(x, msg) x
#endif

// MAIN LOGGING CATEGORIES
DEFINE_UNIQUE_LOG_CHANNEL(LogInit);
DEFINE_UNIQUE_LOG_CHANNEL(LogApp);
