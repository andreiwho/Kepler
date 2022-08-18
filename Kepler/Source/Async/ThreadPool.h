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

namespace Kepler
{
	class TThreadPool
	{
	public:
		TThreadPool(const u32 InNumThreads = 0);

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
				const std::scoped_lock Lck(TasksMutex);
				Tasks.Enqueue(std::move(Task));
			}
			++TotalTaskNum;
			TasksAvailableFence.notify_one();
		}

		void Reset(const u32 NumThreads = 0);

		template <typename TFUNC, typename RETVAL = std::invoke_result_t<std::decay_t<TFUNC>>>
		std::future<RETVAL> SubmitTask(TFUNC&& Task)
		{
			TSharedPtr<std::promise<RETVAL>> Promise = MakeShared<std::promise<RETVAL>>();
			EnqueueTask(
				[Func = std::move(Task), Promise, this]
				{
					try
					{
						if constexpr (std::is_void_v<RETVAL>)
						{
							std::invoke(Func);
							Promise->set_value();
						}
						else
						{
							Promise->set_value(std::invoke(Func));
						}
					}
					catch (const TException& Exception)
					{
						auto SavedException = std::make_shared<TException>(Exception);
						TGlobalExceptionContainer::Get()->Push(std::move(SavedException));
					}
					catch (const std::exception& Exception)
					{
						auto SavedException = std::make_shared<TException>(Exception.what(), "StdException");
						TGlobalExceptionContainer::Get()->Push(std::move(SavedException));
					}
					
				});
			return Promise->get_future();
		}

		void Unpause();

		void WaitForTasksToFinish();

		bool HasAnyExceptions() const;

		void RethrowExceptions_MainThread();

	private:
		void CreateThreads();

		void DestroyThreads();

		u32 CalculateThreadCount(const u32 InThreadCount);

		void WorkerMain();

		std::atomic<bool> bPaused = false;

		std::atomic<bool> bIsRunning = false;

		std::condition_variable TasksAvailableFence = {};

		std::condition_variable TaskDoneFence = {};

		static constexpr usize MAX_TASK_COUNT = 10000;
		TThreadRelaxedRingQueue<std::function<void()>> Tasks{ MAX_TASK_COUNT };

		std::atomic<usize> TotalTaskNum = 0;

		mutable std::mutex TasksMutex = {};

		u32 ThreadCount = 0;

		TDynArray<std::thread> Workers{ };

		std::atomic<bool> bWaiting = false;

		TThreadSafeRingQueue<std::shared_ptr<TException>> Exceptions{ 32 };
	};
}