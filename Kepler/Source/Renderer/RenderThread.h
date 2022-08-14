#pragma once
#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/RingQueue.h"

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
		void Enqueue(Fn&& Func)
		{
			std::packaged_task<void()> Task(std::move(Func));
			CHECK(Tasks.Enqueue(std::move(Task)));
			QueueFence.notify_one();
		}

		bool Dequeue(std::packaged_task<void()>& OutTask);
		void FlushAndWait();

	private:
		static void ThreadMain(TRenderThread* This);
		bool bRunning = true;
		void Flush();

	private:
		TThreadSafeRingQueue<std::packaged_task<void()>> Tasks{ 16386 };
		std::thread Thread{};
		std::mutex Mutex{};
		std::condition_variable QueueFence{};

		std::mutex FlushMutex{};
		std::condition_variable FlushFence{};
	};
}

#define ENQUEUE_RENDER_TASK(Task) Kepler::TRenderThread::Get()->Enqueue(Task)
#define FORCE_FLUSH_RENDER_THREAD() Kepler::TRenderThread::Get()->FlushAndWait()
#define ENQUEUE_RENDER_TASK_FLUSH(Task) ENQUEUE_RENDER_TASK(Task); FORCE_FLUSH_RENDER_THREAD()