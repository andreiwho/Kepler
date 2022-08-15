#include "ThreadPool.h"

namespace Kepler
{
	void TThreadPool::WorkerMain()
	{
		while (bIsRunning)
		{
			std::function<void()> Task;
			std::unique_lock<std::mutex> Lck(TasksMutex);
			TasksAvailableFence.wait(Lck, [this] { return Tasks.GetLength() > 0 || !bIsRunning; });
			if (bIsRunning && !bPaused)
			{
				bool bTaskValid = false;
				if (bTaskValid = Tasks.Dequeue(Task))
				{
					Lck.unlock();
					Task();
					Lck.lock();
					--TotalTaskNum;
				}
				if (bWaiting)
					TaskDoneFence.notify_one();
			}
		}
	}

	u32 TThreadPool::CalculateThreadCount(const u32 InThreadCount)
	{
		if (InThreadCount > 0)
			return InThreadCount;
		else
		{
			const u32 HWConcurrency = std::thread::hardware_concurrency();
			if (HWConcurrency > 0)
				return HWConcurrency;
			else
				return 1;
		}
	}

	void TThreadPool::DestroyThreads()
	{
		bIsRunning = false;
		TasksAvailableFence.notify_all();
		for (u32 Index = 0; Index < ThreadCount; ++Index)
		{
			Workers[Index].join();
		}
	}

	void TThreadPool::CreateThreads()
	{
		bIsRunning = true;
		for (u32 Index = 0; Index < ThreadCount; ++Index)
		{
			Workers[Index] = std::thread(&TThreadPool::WorkerMain, this);
		}
	}

	void TThreadPool::WaitForTasksToFinish()
	{
		bWaiting = true;
		std::unique_lock Lck(TasksMutex);
		TaskDoneFence.wait(Lck, [this] { return (TotalTaskNum == (bPaused ? Tasks.GetLength() : 0)); });
		bWaiting = false;
	}

	void TThreadPool::Unpause()
	{
		bPaused = false;
	}

	void TThreadPool::Reset(const u32 NumThreads)
	{
		const bool bWasPaused = bPaused;
		bPaused = true;
		WaitForTasksToFinish();
		DestroyThreads();
		ThreadCount = CalculateThreadCount(NumThreads);
		Workers = TDynArray<std::thread>(ThreadCount);
		bPaused = bWasPaused;
		CreateThreads();
	}

	void TThreadPool::Pause()
	{
		bPaused = true;
	}

	bool TThreadPool::IsPaused() const
	{
		return bPaused;
	}

	u32 TThreadPool::GetNumThreads() const
	{
		return ThreadCount;
	}

	usize TThreadPool::GetTotalNumTasks() const
	{
		return TotalTaskNum;
	}

	usize TThreadPool::GetNumExecutingTasks() const
	{
		const std::scoped_lock Lck(TasksMutex);
		return TotalTaskNum - Tasks.GetLength();
	}

	usize TThreadPool::GetNumTasks() const
	{
		const std::scoped_lock Lck(TasksMutex);
		return Tasks.GetLength();
	}

	TThreadPool::~TThreadPool()
	{
		WaitForTasksToFinish();
		DestroyThreads();
	}

	TThreadPool::TThreadPool(const u32 InNumThreads) : ThreadCount(CalculateThreadCount(InNumThreads))
		, Workers(CalculateThreadCount(ThreadCount))
	{
		CreateThreads();
	}

	void TThreadPool::RethrowExceptions_MainThread()
	{
		std::shared_ptr<std::exception> Exception;
		while (Exceptions.Dequeue(Exception))
		{
			if (auto Casted = std::dynamic_pointer_cast<TException>(Exception))
			{
				throw* Casted;
			}
			throw *Exception;
		}
	}

	bool TThreadPool::HasAnyExceptions() const
	{
		return Exceptions.GetLength() > 0;
	}

}