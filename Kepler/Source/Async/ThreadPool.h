#pragma once

#include "Core/Types.h"
#include "Core/Malloc.h"
#include "Core/Containers/RingQueue.h"

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
	template <typename T>
	class TFuturePack
	{
	public:
		TFuturePack(const usize FutureCount = 0) : Futures(FutureCount) {}

		inline std::conditional_t<std::is_void_v<T>, void, std::vector<T>> Get()
		{
			if constexpr (std::is_void_v<T>)
			{
				for (usize Index = 0; Index < Futures.size(); ++Index)
					Futures[Index].get();
				return;
			}
			else
			{
				std::vector<T> Results(Futures.size());
				for (usize Index = 0; Index < Futures.size(); ++Index)
					Results[Index] = Futures[Index].get();
				return Results;
			}
		}

		inline std::future<T>& operator[](const usize Index)
		{
			return Futures.at(Index);
		}

		inline void AddFuture(std::future<T>&& future)
		{
			Futures.push_back(std::move(future));
		}

		inline usize GetLength() const
		{
			return Futures.size();
		}

		void Wait() const
		{
			for (usize Index = 0; Index < Futures.size(); ++Index)
				Futures[Index].wait();
		}

	private:
		std::vector<std::future<T>> Futures;
	};

	template <typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
	class TBlockDividor
	{
	public:
		TBlockDividor(const T1 InFirstIndex, const T2 InIndexAfterLast, const usize InBlockCount) : FirstIndex(static_cast<T>(InFirstIndex)), InIndexAfterLast(static_cast<T>(InIndexAfterLast)), BlockCount(InBlockCount)
		{
			if (InIndexAfterLast < FirstIndex)
				std::swap(InIndexAfterLast, FirstIndex);
			TotalSize = static_cast<usize>(InIndexAfterLast - FirstIndex);
			BlockSize = static_cast<usize>(TotalSize / BlockCount);
			if (BlockSize == 0)
			{
				BlockSize = 1;
				BlockCount = (TotalSize > 1) ? TotalSize : 1;
			}
		}

		T Start(const usize Index) const
		{
			return static_cast<T>(Index * BlockSize) + FirstIndex;
		}

		T End(const usize Index) const
		{
			return (Index == BlockCount - 1) ? InIndexAfterLast : (static_cast<T>((Index + 1) * BlockSize) + FirstIndex);
		}

		usize GetLength() const
		{
			return BlockCount;
		}

		usize GetTotalSize() const
		{
			return TotalSize;
		}

	private:
		usize BlockSize = 0;
		T FirstIndex = 0;
		T InIndexAfterLast = 0;
		usize BlockCount = 0;
		usize TotalSize = 0;
	};

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

		template <typename TFUNC, typename T1, typename T2, typename T = std::common_type_t<T1, T2>, typename RETVAL = std::invoke_result_t<std::decay_t<TFUNC>, T, T>>
		TFuturePack<RETVAL> ParallelLoop(const T1 First, const T2 AfterLast, TFUNC&& Loop, const usize DesiredBlockCount = 0)
		{
			TBlockDividor Blocks(First, AfterLast, DesiredBlockCount ? DesiredBlockCount : ThreadCount);
			if (Blocks.GetTotalSize() > 0)
			{
				TFuturePack<RETVAL> OutFutures(Blocks.GetLength());
				for (usize Index = 0; Index < Blocks.GetLength(); ++Index)
					OutFutures[Index] = SubmitTask(std::forward<TFUNC>(Loop), Blocks.start(Index), Blocks.end(Index));
				return OutFutures;
			}
			else
			{
				return TFuturePack<RETVAL>();
			}
		}

		template <typename TFUNC, typename T, typename RETVAL = std::invoke_result_t<std::decay_t<TFUNC>, T, T>>
		TFuturePack<RETVAL> ParallelLoop(const T AfterLast, TFUNC&& Loop, const usize DesiredBlockCount = 0)
		{
			return ParallelLoop(0, AfterLast, std::forward<TFUNC>(Loop), DesiredBlockCount);
		}

		void Pause();

		template <typename TFUNC, typename T1, typename T2, typename T = std::common_type_t<T1, T2>>
		void EnqueueLoop(const T1 First, const T2 AfterLast, TFUNC&& Loop, const usize DesiredBlockCount = 0)
		{
			TBlockDividor Blocks(First, AfterLast, DesiredBlockCount ? DesiredBlockCount : ThreadCount);
			if (Blocks.GetTotalSize() > 0)
			{
				for (usize Index = 0; Index < Blocks.GetLength(); ++Index)
					EnqueueTask(std::forward<TFUNC>(Loop), Blocks.Start(Index), Blocks.End(Index));
			}
		}

		template <typename TFUNC, typename T>
		void EnqueueLoop(const T AfterLast, TFUNC&& Loop, const usize DesiredBlockCount = 0)
		{
			EnqueueLoop(0, AfterLast, std::forward<TFUNC>(Loop), DesiredBlockCount);
		}

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

		template <typename TFUNC, typename... A, typename RETVAL = std::invoke_result_t<std::decay_t<TFUNC>, std::decay_t<A>...>>
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
						Exceptions.Enqueue(std::move(SavedException));
					}
					catch (const std::exception& Exception)
					{
						try
						{
							auto SavedException = std::make_shared<std::exception>(Exception);
							Exceptions.Enqueue(std::move(SavedException));
						}
						catch (...)
						{
						}
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

		std::vector<std::thread> Workers{ };

		std::atomic<bool> bWaiting = false;

		TThreadSafeRingQueue<std::shared_ptr<std::exception>> Exceptions{ 32 };
	};
}