#pragma once
#include "ThreadPool.h"

namespace Kepler
{
	extern TThreadPool* GLargeThreadPool;

	template<typename FUNC>
	inline decltype(auto) Async(FUNC&& Func)
	{
		CHECK(GLargeThreadPool);
		return GLargeThreadPool->SubmitTask(std::move(Func));
	}

	template<typename Type>
	inline decltype(auto) Await(std::future<Type>&& Future)
	{
		try
		{
			return Future.get();
		}
		catch (const TException& Exception)
		{
			TGlobalExceptionContainer::Get()->Push(std::make_shared<TException>(Exception));
			throw;
		}
	}

	template<typename Type>
	inline decltype(auto) Await(std::future<Type>& FutureLV)
	{
		return Await(std::move(FutureLV));
	}
}