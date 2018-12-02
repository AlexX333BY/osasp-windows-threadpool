#pragma once

#include <Windows.h>

namespace Threadpool
{
	class TaskArgumentPair
	{
	public:
		TaskArgumentPair(LPTHREAD_START_ROUTINE lpTask, LPVOID lpArgument);
		LPTHREAD_START_ROUTINE GetTask();
		LPVOID GetArgument();
	protected:
		CONST LPTHREAD_START_ROUTINE m_lpTask;
		CONST LPVOID m_lpArgument;
	};
}
