#pragma once

#include <Windows.h>
#include <concurrent_queue.h>

namespace Threadpool
{
	class Threadpool
	{
	public:
		Threadpool();
		Threadpool(DWORD dwThreadCount);
		~Threadpool();

		VOID AddTask(LPTHREAD_START_ROUTINE lpTask);
		VOID Wait();
		VOID Wait(DWORD dwTimeout);
	protected:
		DWORD m_dwThreadCount;
		HANDLE *m_hThreads;
		concurrency::concurrent_queue<LPTHREAD_START_ROUTINE> *m_cqTaskQueue;

		VOID ListenQueue();

		static DWORD GetNumberOfProcessors();
	};
}
