#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/RingQueue.h"
#include "Async/ThreadPool.h"

#include <thread>
#include <future>
#include <condition_variable>

namespace Kepler
{
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
			if (Instance->WorkerPool.HasAnyExceptions())
			{
				Instance->WorkerPool.RethrowExceptions_MainThread();
			}
#endif

			return Instance->WorkerPool.SubmitTask(std::move(Func));
		}

		static void Wait();

	private:
		bool bRunning = true;

	private:
		TThreadPool WorkerPool{ 1 };
	};
}