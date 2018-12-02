#include <stdio.h>
#include <Windows.h>

int main(int argc, char **argv)
{
	if (argc == 0)
	{
		printf("usage: %s filename [threads]", argv[0]);
		return -1;
	}

	DWORD dwThreadCount;
	if (argc >= 2)
	{
		dwThreadCount = strtoul(argv[2], NULL, 10);
		if (dwThreadCount == 0)
		{
			printf("Illegal threads count");
			return -1;
		}
	}
}
