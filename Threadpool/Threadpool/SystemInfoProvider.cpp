#include "SystemInfoProvider.h"

namespace Threadpool
{
	DWORD SystemInfoProvider::GetNumberOfProcessors()
	{
		SYSTEM_INFO siInfo;
		GetSystemInfo(&siInfo);
		return siInfo.dwNumberOfProcessors;
	}
}
