#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <Windows.h>
#include "..\Threadpool\Threadpool.h"

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

LPSTR *GetFileLines(LPSTR lpsFilename, LPDWORD lpdwLinesCount)
{
	HANDLE hFile = CreateFile(lpsFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	LARGE_INTEGER liFileSize;
	if (!GetFileSizeEx(hFile, &liFileSize))
	{
		CloseHandle(hFile);
		return NULL;
	}

	DWORD dwReadCount;
	LPSTR lpsFile = (LPSTR)calloc(liFileSize.QuadPart + 1, 1);
	BOOL bReadResult = ReadFile(hFile, lpsFile, (DWORD)liFileSize.QuadPart, &dwReadCount, NULL);
	CloseHandle(hFile);
	if (!bReadResult)
	{
		free(lpsFile);
		return NULL;
	}

	LPCSTR lpsDelimiters = "\r\n";
	LPSTR *lpsResult = (LPSTR *)calloc(0, sizeof(LPSTR)), *lpsBuffer;
	LPSTR lpsContext;
	DWORD dwLinesCount = 0;
	LPSTR lpsToken = strtok_s(lpsFile, lpsDelimiters, &lpsContext);

	while (lpsToken != NULL)
	{
		do
		{
			lpsBuffer = (LPSTR *)realloc(lpsResult, (dwLinesCount + 1) * sizeof(LPSTR));
		} while (lpsBuffer == NULL);
		lpsResult = lpsBuffer;
		lpsResult[dwLinesCount++] = lpsToken;
		lpsToken = strtok_s(NULL, lpsDelimiters, &lpsContext);
	}

	*lpdwLinesCount = dwLinesCount;
	return lpsResult;
}

int main(int argc, char **argv)
{
	if (argc == 0)
	{
		printf("usage: %s filename [threads]\n", argv[0]);
		return -1;
	}

	INT dwThreadCount = 0;
	if (argc >= 3)
	{
		dwThreadCount = strtol(argv[2], NULL, 10);
		if (dwThreadCount <= 0)
		{
			printf("Illegal threads count\n");
			return -1;
		}
	}

	DWORD dwLinesCount;
	LPSTR *lpsLines = GetFileLines(argv[1], &dwLinesCount);

	ParallelSort(lpsLines, dwLinesCount, dwThreadCount);
	MergeSortLines(lpsLines, 0, dwLinesCount - 1);

	for (DWORD dwLine = 0; dwLine < dwLinesCount; ++dwLine)
	{
		printf("%s\n", lpsLines[dwLine]);
	}
	system("pause");
}
