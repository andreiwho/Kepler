#pragma once

#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/RingQueue.h"
#include "Core/Containers/DynArray.h"

#include <atomic>            
#include <chrono>            
#include <condition_variable>
#include <exception>         
#include <functional>        
#include <future>            
#include <iostream>          
#include <memory>            
#include <mutex>             
#include <queue>             
#include <thread>            
#include <type_traits>       
#include <utility>           
#include <vector>            

namespace ke
{
	class TThreadPool
	{
	public:
		TThreadPool(const u32 numThreads = 0);

		~TThreadPool();

		usize GetNumTasks() const;

		usize GetNumExecutingTasks() const;

		usize GetTotalNumTasks() const;

		u32 GetNumThreads() const;

		bool IsPaused() const;

		void Pause();

		template <typename TFUNC>
		void EnqueueTask(TFUNC&& Task)
		{
			{
				const std::scoped_lock lck(m_TaskMutex);
				m_Tasks.Enqueue(std::move(Task));
			}
			++m_TotalTaskNum;
			m_TasksAvailableFence.notify_one();
		}

		void Reset(const u32 NumThreads = 0);

		template <typename TFUNC, typename RETVAL = std::invoke_result_t<std::decay_t<TFUNC>>>
		std::future<RETVAL> SubmitTask(TFUNC&& Task)
		{
			SharedPtr<std::promise<RETVAL>> promise = MakeShared<std::promise<RETVAL>>();
			EnqueueTask(
				[Func = std::move(Task), promise, this]
				{
					if constexpr (std::is_void_v<RETVAL>)
					{
						std::invoke(Func);
						promise->set_value();
					}
					else
					{
						auto Value = std::invoke(Func);
						promise->set_value(Value);
					}

				});
			return promise->get_future();
		}

		void Unpause();

		void WaitForTasksToFinish();

		bool HasAnyExceptions() const;

		[[deprecated("This function does nothing")]]
		void RethrowExceptions_MainThread();

	private:
		void CreateThreads();

		void DestroyThreads();

		u32 CalculateThreadCount(const u32 numThreads);

		void WorkerMain();

	private:
		std::atomic<bool> m_bPaused = false;

		std::atomic<bool> m_bIsRunning = false;

		std::condition_variable m_TasksAvailableFence = {};

		std::condition_variable m_TaskDoneFence = {};

		static constexpr usize MAX_TASK_COUNT = 10000;
		TThreadRelaxedRingQueue<std::function<void()>> m_Tasks{ MAX_TASK_COUNT };

		std::atomic<usize> m_TotalTaskNum = 0;

		mutable std::mutex m_TaskMutex = {};

		u32 m_ThreadCount = 0;

		Array<std::thread> m_Workers{ };

		std::atomic<bool> m_bWaiting = false;

		TThreadSafeRingQueue<std::exception_ptr> m_Exceptions{ 32 };
	};
}