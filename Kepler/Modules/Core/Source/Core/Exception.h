#pragma once
#include "Build.h"
#include <string>
#include <stdexcept>
#include <vector>
#include <mutex>

namespace ke
{
	class CORE_API TException : public std::runtime_error
	{
	public:
		TException(const std::string& msg, const std::string& name = "Exception");
		inline const std::string& GetErrorMessage() const { return m_ErrorMessage; }

	private:
		std::string m_ErrorMessage;
	};
}