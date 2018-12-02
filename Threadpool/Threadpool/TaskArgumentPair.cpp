#include "TaskArgumentPair.h"
#include <stdexcept> 

namespace Threadpool
{
	TaskArgumentPair::TaskArgumentPair(LPTHREAD_START_ROUTINE lpTask, LPVOID lpArgument)
		: m_lpTask(lpTask), m_lpArgument(lpArgument)
	{
		if (lpTask == NULL)
		{
			throw new std::invalid_argument("lpTask");
		}
	}

	LPTHREAD_START_ROUTINE TaskArgumentPair::GetTask()
	{
		return m_lpTask;
	}

	LPVOID TaskArgumentPair::GetArgument()
	{
		return m_lpArgument;
	}
}
