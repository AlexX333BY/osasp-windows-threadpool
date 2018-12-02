#include <stdlib.h>
#include <search.h>
#include "..\Threadpool\Threadpool.h"

namespace FileLinesSorter
{
	typedef struct _SORT_ROUTINE_DATA
	{
		LPSTR *lpsStrings;
		DWORD dwCount;
	} SORT_ROUTINE_DATA;

	int _cdecl QsortStringComparator(const void *lpString1, const void *lpString2)
	{
		return strcmp(*((LPSTR *)lpString1), *((LPSTR *)lpString2));
	}

	DWORD WINAPI SortRoutine(SORT_ROUTINE_DATA *srdArgument)
	{
		qsort(srdArgument->lpsStrings, srdArgument->dwCount, sizeof(LPSTR), QsortStringComparator);
		return 0;
	}

	VOID MergeSortLines(LPSTR *lpsLines, DWORD dwLeft, DWORD dwRight)
	{
		if (dwLeft == dwRight)
		{
			return;
		}

		DWORD dwMiddle = (dwRight + dwLeft) / 2;
		MergeSortLines(lpsLines, dwLeft, dwMiddle);
		MergeSortLines(lpsLines, dwMiddle + 1, dwRight);

		DWORD dwBufferLength = dwRight - dwLeft + 1;
		LPSTR *lpsBuffer = (LPSTR *)calloc(dwBufferLength, sizeof(LPSTR));
		DWORD dwLeftPartElement = dwLeft, dwRightPartElement = dwMiddle + 1;
		DWORD dwInsertIndex = 0;

		while ((dwLeftPartElement <= dwMiddle) && (dwRightPartElement <= dwRight))
		{
			if (strcmp(lpsLines[dwLeftPartElement], lpsLines[dwRightPartElement]) < 0)
			{
				lpsBuffer[dwInsertIndex++] = lpsLines[dwLeftPartElement++];
			}
			else
			{
				lpsBuffer[dwInsertIndex++] = lpsLines[dwRightPartElement++];
			}
		}
		while (dwLeftPartElement <= dwMiddle)
		{
			lpsBuffer[dwInsertIndex++] = lpsLines[dwLeftPartElement++];
		}
		while (dwRightPartElement <= dwRight)
		{
			lpsBuffer[dwInsertIndex++] = lpsLines[dwRightPartElement++];
		}

		for (DWORD dwCopyIndex = 0; dwCopyIndex < dwBufferLength; ++dwCopyIndex)
		{
			lpsLines[dwLeft + dwCopyIndex] = lpsBuffer[dwCopyIndex];
		}
	}

	VOID ParallelSort(LPSTR *lpsLines, DWORD dwLinesCount, DWORD dwThreadCount)
	{
		Threadpool::Threadpool *tpThreadpool;
		if (dwThreadCount == 0)
		{
			tpThreadpool = new Threadpool::Threadpool();
			dwThreadCount = tpThreadpool->GetThreadCount();
		}
		else
		{
			tpThreadpool = new Threadpool::Threadpool(dwThreadCount);
		}

		DWORD dwStringsForThread = dwLinesCount / dwThreadCount + (dwLinesCount % dwThreadCount == 0 ? 0 : 1);

		if (dwStringsForThread > 0)
		{
			INT iStringsForCurrentThread;
			Threadpool::TaskArgumentPair **taPairs = (Threadpool::TaskArgumentPair **)calloc(0, sizeof(Threadpool::TaskArgumentPair *)), **taBuffer;
			DWORD dwPairsCount = 0;
			DWORD dwProcessedStrings = 0;
			SORT_ROUTINE_DATA *srdData;
			for (DWORD dwThread = 0; dwThread < dwThreadCount; ++dwThread)
			{
				iStringsForCurrentThread = min(dwStringsForThread, dwLinesCount - dwProcessedStrings);
				if (iStringsForCurrentThread > 0)
				{
					srdData = (SORT_ROUTINE_DATA *)calloc(1, sizeof(SORT_ROUTINE_DATA));
					srdData->dwCount = iStringsForCurrentThread;
					srdData->lpsStrings = lpsLines + dwProcessedStrings;

					do
					{
						taBuffer = (Threadpool::TaskArgumentPair **)realloc(taPairs, (dwPairsCount + 1) * sizeof(Threadpool::TaskArgumentPair *));
					} while (taBuffer == NULL);
					taPairs = taBuffer;
					taPairs[dwPairsCount] = new Threadpool::TaskArgumentPair((LPTHREAD_START_ROUTINE)SortRoutine, srdData);
					tpThreadpool->AddTask(taPairs[dwPairsCount++]);
					dwProcessedStrings += iStringsForCurrentThread;
				}
			}

			tpThreadpool->Wait();

			for (DWORD dwPair = 0; dwPair < dwPairsCount; ++dwPair)
			{
				free(taPairs[dwPair]->GetArgument());
				delete taPairs[dwPair];
			}
			delete[] taPairs;
		}

		delete tpThreadpool;
	}

	VOID Sort(LPSTR *lpsLines, DWORD dwLinesCount, DWORD dwThreadsCount)
	{
		ParallelSort(lpsLines, dwLinesCount, dwThreadsCount);
		MergeSortLines(lpsLines, 0, dwLinesCount - 1);
	}
}
