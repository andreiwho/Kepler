#pragma once
#include <string>
#include <stdexcept>

namespace Kepler
{
	class TException : public std::runtime_error
	{
	public:
		TException(const std::string& Message, const std::string& Name = "Exception");
		inline const std::string& GetErrorMessage() const { return ErrorMessage; }

	private:
		std::string ErrorMessage;
	};
}