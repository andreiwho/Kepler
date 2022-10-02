#pragma once
#include "ThreadPool.h"

namespace ke
{
	extern TThreadPool* GLargeThreadPool;

	template<typename FUNC>
	inline decltype(auto) Async(FUNC&& func)
	{
		CHECK(GLargeThreadPool);
		return GLargeThreadPool->SubmitTask(std::move(func));
	}

	template<typename Type>
	inline decltype(auto) Await(std::future<Type>&& fut)
	{
		try
		{
			return fut.get();
		}
		catch (const TException& exc)
		{
			TGlobalExceptionContainer::Get()->Push(std::make_shared<TException>(exc));
			throw;
		}
	}

	template<typename Type>
	inline decltype(auto) Await(std::future<Type>& future)
	{
		return Await(std::move(future));
	}
}