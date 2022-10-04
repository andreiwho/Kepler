#include "Exception.h"
#include <spdlog/fmt/fmt.h>
#include <sstream>

namespace ke
{
	TException::TException(const std::string& msg, const std::string& name)
		: m_ErrorMessage(fmt::format("{} -> {}", name, msg))
		, std::runtime_error("TException needs to be caught")
	{
	}
}