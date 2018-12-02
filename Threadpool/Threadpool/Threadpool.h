#pragma once

#include "TaskArgumentPair.h"
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

		VOID AddTask(TaskArgumentPair *lpTaskArgument);
		VOID Wait();
		VOID Wait(DWORD dwTimeout);
	protected:
		DWORD m_dwThreadCount;
		HANDLE *m_lpThreads;

		volatile LONG m_bIsRunning;

		CRITICAL_SECTION m_csQueueCriticalSection;
		CONDITION_VARIABLE m_cvQueueConditionVariable;
		std::queue<TaskArgumentPair *> *m_qTaskQueue;

		static DWORD WINAPI TaskListenerThreadRoutine(Threadpool *lpInstance);
	};
}
