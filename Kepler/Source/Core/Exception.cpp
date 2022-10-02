#include "Exception.h"
#include <spdlog/fmt/fmt.h>
#include <sstream>

namespace ke
{
	TException::TException(const std::string& Message, const std::string& Name)
		: ErrorMessage(fmt::format("{} -> {}", Name, Message))
		, std::runtime_error("TException needs to be caught")
	{
	}

	TGlobalExceptionContainer* TGlobalExceptionContainer::Instance = nullptr;

	void TGlobalExceptionContainer::Rethrow()
	{
		std::lock_guard lck{ ExceptionsMutex };
		if (Exceptions.empty())
		{
			return;
		}

		std::shared_ptr<TException> Exception;
		std::stringstream ErrorStream;
		for (const auto& Exception : Exceptions)
		{
			ErrorStream << Exception->GetErrorMessage() << std::endl;
		}
		throw TException(ErrorStream.str(), "RethrowExceptions_MainThread");
	}

}