#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <mutex>

namespace ke
{
	class TException : public std::runtime_error
	{
	public:
		TException(const std::string& Message, const std::string& Name = "Exception");
		inline const std::string& GetErrorMessage() const { return ErrorMessage; }

	private:
		std::string ErrorMessage;
	};

	class TGlobalExceptionContainer
	{
		static TGlobalExceptionContainer* Instance;
	public:
		TGlobalExceptionContainer() { Instance = this; }

		static TGlobalExceptionContainer* Get() { return Instance; }

		void Push(std::shared_ptr<TException> Exception) 
		{ 
			std::lock_guard lck{ ExceptionsMutex };
			Exceptions.push_back(Exception); 
		}
		void Rethrow();

	private:
		std::mutex ExceptionsMutex;
		std::vector<std::shared_ptr<TException>> Exceptions;
	};
}