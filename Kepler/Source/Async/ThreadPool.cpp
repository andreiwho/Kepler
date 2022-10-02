#include "ThreadPool.h"
#include "Platform/Platform.h"

namespace ke
{
	void TThreadPool::WorkerMain()
	{
		KEPLER_PROFILE_INIT_THREAD("Worker");
		while (m_bIsRunning)
		{
			std::function<void()> task;
			std::unique_lock<std::mutex> lck(m_TaskMutex);
			m_TasksAvailableFence.wait(lck, 
				[this] 
				{ 
					return m_Tasks.GetLength() > 0 || !m_bIsRunning; 
				});
			if (m_bIsRunning && !m_bPaused)
			{
				bool bTaskValid = false;
				if (bTaskValid = m_Tasks.Dequeue(task))
				{
					lck.unlock();
					// TODO: Fix cross-thread exception handling
					try
					{
						task();
					}
					catch (const TException& exc)
					{
						if (TPlatform::HandleCrashReported(exc.GetErrorMessage()))
						{
							return;
						}
					}
					catch (const std::exception& exc)
					{
						if (TPlatform::HandleCrashReported(exc.what()))
						{
							return;
						}
					}
					lck.lock();
					--m_TotalTaskNum;
				}
				if (m_bWaiting)
					m_TaskDoneFence.notify_one();
			}
		}
	}

	u32 TThreadPool::CalculateThreadCount(const u32 numThreads)
	{
		if (numThreads > 0)
			return numThreads;
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
		m_bIsRunning = false;
		m_TasksAvailableFence.notify_all();
		for (u32 idx = 0; idx < m_ThreadCount; ++idx)
		{
			m_Workers[idx].join();
		}
	}

	void TThreadPool::CreateThreads()
	{
		m_bIsRunning = true;
		for (u32 idx = 0; idx < m_ThreadCount; ++idx)
		{
			m_Workers[idx] = std::thread(&TThreadPool::WorkerMain, this);
		}
	}

	void TThreadPool::WaitForTasksToFinish()
	{
		m_bWaiting = true;
		std::unique_lock lck(m_TaskMutex);
		m_TaskDoneFence.wait(lck, 
			[this] 
			{ 
				return (m_TotalTaskNum == (m_bPaused ? m_Tasks.GetLength() : 0)); 
			});
		m_bWaiting = false;
	}

	void TThreadPool::Unpause()
	{
		m_bPaused = false;
	}

	void TThreadPool::Reset(const u32 numThreads)
	{
		const bool bWasPaused = m_bPaused;
		m_bPaused = true;
		WaitForTasksToFinish();
		DestroyThreads();
		m_ThreadCount = CalculateThreadCount(numThreads);
		m_Workers = TDynArray<std::thread>(m_ThreadCount);
		m_bPaused = bWasPaused;
		CreateThreads();
	}

	void TThreadPool::Pause()
	{
		m_bPaused = true;
	}

	bool TThreadPool::IsPaused() const
	{
		return m_bPaused;
	}

	u32 TThreadPool::GetNumThreads() const
	{
		return m_ThreadCount;
	}

	usize TThreadPool::GetTotalNumTasks() const
	{
		return m_TotalTaskNum;
	}

	usize TThreadPool::GetNumExecutingTasks() const
	{
		const std::scoped_lock lck(m_TaskMutex);
		return m_TotalTaskNum - m_Tasks.GetLength();
	}

	usize TThreadPool::GetNumTasks() const
	{
		const std::scoped_lock lck(m_TaskMutex);
		return m_Tasks.GetLength();
	}

	TThreadPool::~TThreadPool()
	{
		WaitForTasksToFinish();
		DestroyThreads();
	}

	TThreadPool::TThreadPool(const u32 numThreads) : m_ThreadCount(CalculateThreadCount(numThreads))
		, m_Workers(CalculateThreadCount(m_ThreadCount))
	{
		CreateThreads();
	}

	void TThreadPool::RethrowExceptions_MainThread()
	{
	}

	bool TThreadPool::HasAnyExceptions() const
	{
		return m_Exceptions.GetLength() > 0;
	}

}