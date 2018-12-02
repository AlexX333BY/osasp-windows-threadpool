#pragma once

#include <Windows.h>

namespace Threadpool
{
	typedef LPVOID(*THREADTASK)(LPVOID lpArgument);

	class Threadpool
	{
	public:
		Threadpool();
		Threadpool(DWORD dwThreadCount);
		~Threadpool();

		BOOL AddTask(THREADTASK ttTask);
		BOOL Wait();
		BOOL Wait(DWORD dwTimeout);
	};
}
