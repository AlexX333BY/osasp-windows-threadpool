#pragma once

#include <Windows.h>
#include <queue>

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
		HANDLE *m_lpThreads;

		volatile BOOL m_bIsRunning;

		CRITICAL_SECTION m_csQueueCriticalSection;
		CONDITION_VARIABLE m_cvQueueConditionVariable;
		std::queue<LPTHREAD_START_ROUTINE> *m_qTaskQueue;

		static VOID TaskListenerThreadRoutine(Threadpool *lpInstance);

		static DWORD GetNumberOfProcessors();
	};
}
