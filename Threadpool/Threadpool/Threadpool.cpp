#include "Threadpool.h"
#include <stdexcept> 

namespace Threadpool
{
	Threadpool::Threadpool() : Threadpool::Threadpool(GetNumberOfProcessors())
	{ }

	Threadpool::Threadpool(DWORD dwThreadCount) 
		: m_dwThreadCount(dwThreadCount), m_bIsRunning(TRUE)
	{
		if (dwThreadCount == 0)
		{
			throw new std::out_of_range("dwThreadCount");
		}

		m_lpThreads = new HANDLE[dwThreadCount];
		for (DWORD dwThread = 0; dwThread < dwThreadCount; ++dwThread)
		{
			do
			{
				m_lpThreads[dwThread] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TaskListenerThreadRoutine, this, 0, NULL);
			} while (m_lpThreads[dwThread] == NULL);
		}
		m_qTaskQueue = new std::queue<LPTHREAD_START_ROUTINE>();

		InitializeCriticalSection(&m_csQueueCriticalSection);
		InitializeConditionVariable(&m_cvQueueConditionVariable);
	}

	Threadpool::~Threadpool()
	{
		
	}

	VOID Threadpool::AddTask(LPTHREAD_START_ROUTINE lpTask)
	{
		if (lpTask != NULL)
		{
			EnterCriticalSection(&m_csQueueCriticalSection);
			m_qTaskQueue->push(lpTask);
			WakeConditionVariable(&m_cvQueueConditionVariable);
			LeaveCriticalSection(&m_csQueueCriticalSection);
		}
		else
		{
			throw new std::out_of_range("lpTask");
		}
	}

	VOID Threadpool::TaskListenerThreadRoutine(Threadpool *lpInstance)
	{
		LPTHREAD_START_ROUTINE lpTask;

		do
		{
			EnterCriticalSection(&lpInstance->m_csQueueCriticalSection);
			while (lpInstance->m_qTaskQueue->empty())
			{
				SleepConditionVariableCS(&lpInstance->m_cvQueueConditionVariable, &lpInstance->m_csQueueCriticalSection, INFINITE);
			}
			lpTask = lpInstance->m_qTaskQueue->front();
			lpInstance->m_qTaskQueue->pop();
			LeaveCriticalSection(&lpInstance->m_csQueueCriticalSection);

			if (lpTask != NULL)
			{
				lpTask(NULL);
			}
		} while (lpTask != NULL);
	}

	DWORD Threadpool::GetNumberOfProcessors()
	{
		SYSTEM_INFO siInfo;
		GetSystemInfo(&siInfo);
		return siInfo.dwNumberOfProcessors;
	}
}
