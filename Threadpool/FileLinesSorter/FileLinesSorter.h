#pragma once

#include <Windows.h>

namespace FileLinesSorter
{
	VOID Sort(LPSTR *lpsLines, DWORD dwLinesCount, DWORD dwThreadsCount);
}
