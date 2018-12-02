#include <stdio.h>
#include <Windows.h>

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

	DWORD dwThreadCount;
	if (argc >= 3)
	{
		dwThreadCount = strtoul(argv[2], NULL, 10);
		if (dwThreadCount == 0)
		{
			printf("Illegal threads count\n");
			return -1;
		}
	}

	DWORD dwLinesCount;
	LPSTR *lpsLines = GetFileLines(argv[1], &dwLinesCount);
}
