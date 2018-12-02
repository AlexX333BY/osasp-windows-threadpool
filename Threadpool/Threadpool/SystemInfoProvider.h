#pragma once

#include <Windows.h>

namespace Threadpool
{
	class SystemInfoProvider
	{
	public:
		static DWORD GetNumberOfProcessors();
	};
}
