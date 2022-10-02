#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/RingQueue.h"
#include "Async/ThreadPool.h"

#include <thread>
#include <future>
#include <condition_variable>

namespace ke
{
	extern bool IsRenderThread();

	class TRenderThread
	{
		static TRenderThread* Instance;
	public:
		TRenderThread();
		~TRenderThread();

		static TRenderThread* Get() { return CHECKED(Instance); }

		template<typename FUNC>
		static decltype(auto) Submit(FUNC&& Func)
		{
#ifdef USE_ASSERT
			if (GetWorkingPool().HasAnyExceptions())
			{
				GetWorkingPool().RethrowExceptions_MainThread();
			}
#endif
			return GetWorkingPool().SubmitTask(std::move(Func));
		}

		static void Wait();

	private:
		bool bRunning = true;
		static TThreadPool& GetWorkingPool() { return Get()->WorkerPool; }

	private:
		TThreadPool WorkerPool{ 1 };
	};
}