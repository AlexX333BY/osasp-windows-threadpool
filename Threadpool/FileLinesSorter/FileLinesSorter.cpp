#include <stdio.h>
#include <Windows.h>

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

	HANDLE hFile = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Error opening file %s\n", argv[1]);
		return -1;
	}

	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hFileMapping == NULL)
	{
		CloseHandle(hFile);
		printf("Error mapping file\n");
		return -1;
	}

	LPVOID lpFileView = MapViewOfFileEx(hFileMapping, FILE_MAP_READ, 0, 0, 0, NULL);
	if (lpFileView == NULL)
	{
		CloseHandle(hFileMapping);
		CloseHandle(hFile);
		printf("Error viewing file\n");
		return -1;
	}

	//

	UnmapViewOfFile(lpFileView);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
}
