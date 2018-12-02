#include "Threadpool.h"
#include "SystemInfoProvider.h"
#include <stdexcept> 

namespace Threadpool
{
	Threadpool::Threadpool() : Threadpool::Threadpool(SystemInfoProvider::GetNumberOfProcessors())
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
		m_qTaskQueue = new std::queue<TaskArgumentPair *>();

		InitializeCriticalSection(&m_csQueueCriticalSection);
		InitializeConditionVariable(&m_cvQueueConditionVariable);
	}

	Threadpool::~Threadpool()
	{
		if (m_bIsRunning)
		{
			Wait();
		}

		for (DWORD dwThread = 0; dwThread < m_dwThreadCount; ++dwThread)
		{
			CloseHandle(m_lpThreads[dwThread]);
		}
		delete[] m_lpThreads;
		delete m_qTaskQueue;
		DeleteCriticalSection(&m_csQueueCriticalSection);
	}

	VOID Threadpool::AddTask(TaskArgumentPair * lpTaskArgument)
	{
		if (lpTaskArgument != NULL)
		{
			EnterCriticalSection(&m_csQueueCriticalSection);
			m_qTaskQueue->push(lpTaskArgument);
			WakeConditionVariable(&m_cvQueueConditionVariable);
			LeaveCriticalSection(&m_csQueueCriticalSection);
		}
		else
		{
			throw new std::out_of_range("lpTask");
		}
	}

	VOID Threadpool::Wait()
	{
		Wait(INFINITE);
	}

	VOID Threadpool::Wait(DWORD dwTimeout)
	{
		if (m_bIsRunning)
		{
			m_bIsRunning = FALSE;
			EnterCriticalSection(&m_csQueueCriticalSection);
			for (DWORD dwThread = 0; dwThread < m_dwThreadCount; ++dwThread)
			{
				m_qTaskQueue->push(NULL);
			}
			WakeAllConditionVariable(&m_cvQueueConditionVariable);
			LeaveCriticalSection(&m_csQueueCriticalSection);
			WaitForMultipleObjects(m_dwThreadCount, m_lpThreads, TRUE, dwTimeout);
		}
	}

	VOID Threadpool::TaskListenerThreadRoutine(Threadpool *lpInstance)
	{
		TaskArgumentPair *lpTaskArgument;

		do
		{
			EnterCriticalSection(&lpInstance->m_csQueueCriticalSection);
			while (lpInstance->m_qTaskQueue->empty())
			{
				SleepConditionVariableCS(&lpInstance->m_cvQueueConditionVariable, &lpInstance->m_csQueueCriticalSection, INFINITE);
			}
			lpTaskArgument = lpInstance->m_qTaskQueue->front();
			lpInstance->m_qTaskQueue->pop();
			LeaveCriticalSection(&lpInstance->m_csQueueCriticalSection);

			if (lpTaskArgument != NULL)
			{
				lpTaskArgument->GetTask()(lpTaskArgument->GetArgument());
			}
		} while (lpTaskArgument != NULL);
	}
}
