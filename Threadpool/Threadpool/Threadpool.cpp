#include "Threadpool.h"
#include <stdexcept> 

namespace Threadpool
{
	Threadpool::Threadpool() : Threadpool::Threadpool(GetNumberOfProcessors())
	{ }

	Threadpool::Threadpool(DWORD dwThreadCount) : m_dwThreadCount(dwThreadCount)
	{
		if (dwThreadCount == 0)
		{
			throw new std::out_of_range("dwThreadCount");
		}

		m_hThreads = new HANDLE[dwThreadCount];
		m_cqTaskQueue = new concurrency::concurrent_queue<LPTHREAD_START_ROUTINE>();
	}

	Threadpool::~Threadpool()
	{
		
	}

	VOID Threadpool::AddTask(LPTHREAD_START_ROUTINE lpTask)
	{
		if (lpTask != NULL)
		{
			m_cqTaskQueue->push(lpTask);
		}
		else
		{
			throw new std::out_of_range("lpTask");
		}
	}

	DWORD Threadpool::GetNumberOfProcessors()
	{
		SYSTEM_INFO siInfo;
		GetSystemInfo(&siInfo);
		return siInfo.dwNumberOfProcessors;
	}
}
