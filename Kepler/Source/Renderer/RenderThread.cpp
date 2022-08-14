#include "RenderThread.h"
#include "Core/Log.h"
#include "Renderer/RenderGlobals.h"

namespace Kepler
{
	TRenderThread* TRenderThread::Instance = nullptr;

	TRenderThread::TRenderThread()
	{
		CHECK(!Instance);
		Instance = this;
		WorkerPool.SubmitTask([] { GRenderThreadID = std::this_thread::get_id(); });
	}

	TRenderThread::~TRenderThread()
	{
		Wait();
	}

	void TRenderThread::Wait()
	{
		WorkerPool.WaitForTasksToFinish();
	}
}