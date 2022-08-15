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
		return Future.get();
	}

	template<typename Type>
	inline decltype(auto) Await(std::future<Type>& FutureLV)
	{
		return Await(std::move(FutureLV));
	}
}