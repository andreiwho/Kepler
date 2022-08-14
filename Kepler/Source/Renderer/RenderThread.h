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

		template<typename Fn>
		auto Submit(Fn&& Func)
		{
#ifdef USE_ASSERT
			if (WorkerPool.HasAnyExceptions())
			{
				WorkerPool.RethrowExceptions_MainThread();
			}
#endif

			return WorkerPool.SubmitTask(std::move(Func));
		}

		void Wait();

	private:
		bool bRunning = true;

	private:
		TThreadPool WorkerPool{ 1 };
	};
}

#define ENQUEUE_RENDER_TASK(Task) Kepler::TRenderThread::Get()->Submit(Task)
#define WAIT_RENDER_THREAD() Kepler::TRenderThread::Get()->Wait()
#define ENQUEUE_RENDER_TASK_AWAITED(Task) ENQUEUE_RENDER_TASK(Task); WAIT_RENDER_THREAD()