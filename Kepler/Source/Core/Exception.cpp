#include "Exception.h"
#include <spdlog/fmt/fmt.h>

namespace Kepler
{
	TException::TException(const std::string& Message, const std::string& Name)
		: ErrorMessage(fmt::format("{} -> {}", Name, Message))
		, std::runtime_error(ErrorMessage)
	{
	}
}