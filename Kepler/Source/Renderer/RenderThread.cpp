#include "RenderThread.h"
#include "Core/Log.h"

namespace Kepler
{
	TRenderThread* TRenderThread::Instance = nullptr;

	TRenderThread::TRenderThread()
	{
		CHECK(!Instance);
		Instance = this;
		Thread = std::thread{ &TRenderThread::ThreadMain, this };
	}

	TRenderThread::~TRenderThread()
	{
		if (Thread.joinable())
		{
			bRunning = false;
			QueueFence.notify_one();
			Thread.join();
		}
	}

	bool TRenderThread::Dequeue(std::packaged_task<void()>& OutTask)
	{
		return Tasks.Dequeue(OutTask);
	}

	void TRenderThread::FlushAndWait()
	{
		if (Tasks.GetLength() > 0)
		{
			ENQUEUE_RENDER_TASK([this] { Flush(); });
		
			std::unique_lock lck{ FlushMutex };
			FlushFence.wait(lck);
		}

	}

	void TRenderThread::ThreadMain(TRenderThread* This)
	{
		while (This->bRunning)
		{
			if (This->Tasks.GetLength() == 0)
			{
				std::unique_lock lck{ This->Mutex };
				This->QueueFence.wait(lck);

				if (!This->bRunning)
				{
					This->Flush();
					break;
				}
			}			
			This->Flush();
		}
	}

	void TRenderThread::Flush()
	{
		std::packaged_task<void()> CurrentTask;
		while (Dequeue(CurrentTask))
		{
			CurrentTask();
		}
		FlushFence.notify_one();
	}

}