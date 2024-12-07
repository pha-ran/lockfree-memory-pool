#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "lockfree_memory_pool.h"
#include <process.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

#define THREAD_COUNT	10
#define ALLOC_COUNT		1000

HANDLE handles[THREAD_COUNT];
bool start = 0;
lockfree_memory_pool<unsigned long long> mp;

unsigned __stdcall worker_func(void* index)
{
	int worker_id = GetCurrentThreadId();
	unsigned long long* ptr[ALLOC_COUNT];

	wprintf(L"[WORKER] %d\n", worker_id);

	for (int i = 0; i < ALLOC_COUNT; ++i)
	{
		ptr[i] = mp.oalloc();
		*ptr[i] = 0x1122334455667788;
	}

	while (!start);
	
	for (int i = 0; i < ALLOC_COUNT; ++i)
		mp.ofree(ptr[i]);

	for (;;)
	{
		for (int i = 0; i < ALLOC_COUNT; ++i)
			ptr[i] = mp.oalloc();

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
		{
			unsigned long long* data = ptr[i];
			*data += 1;
			if (*data != 0x1122334455667789) __debugbreak();
		}

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
		{
			unsigned long long* data = ptr[i];
			*data -= 1;
			if (*data != 0x1122334455667788) __debugbreak();
		}

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
			mp.ofree(ptr[i]);

		Sleep(0);
	}

	return 0;
}

int wmain(void) noexcept
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	timeBeginPeriod(1);
	for (int i = 0; i < THREAD_COUNT; ++i)
		handles[i] = (HANDLE)_beginthreadex(nullptr, 0, worker_func, nullptr, 0, nullptr);
	system("pause");
	start = true;

	DWORD wait = WaitForMultipleObjects(THREAD_COUNT, handles, TRUE, INFINITE);
	if (wait != WAIT_OBJECT_0) __debugbreak();
	for (int i = 0; i < THREAD_COUNT; ++i)
		CloseHandle(handles[i]);
	timeEndPeriod(1);

	return 0;
}
