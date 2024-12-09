#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "lockfree_memory_pool.h"
#include <process.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib, "winmm.lib")

#define THREAD_COUNT	4
#define ALLOC_COUNT		1000

class test
{
public:
	test(void) noexcept
		: _value(0x1122334455667788)
	{
		_ptr = new char;
		wprintf(L"test\n");
	}

	~test(void) noexcept
	{
		delete _ptr;
		wprintf(L"~test\n");
	}

public:
	unsigned long long _value;
	char* _ptr;

};

HANDLE handles[THREAD_COUNT];
bool start = 0;
lockfree_memory_pool<test, false> mp((unsigned int)(THREAD_COUNT * ALLOC_COUNT));

unsigned __stdcall worker_func(void*)
{
	int worker_id = GetCurrentThreadId();
	test* ptr[ALLOC_COUNT];

	wprintf(L"[WORKER] %d\n", worker_id);

	while (!start);

	for (;;)
	{
		for (int i = 0; i < ALLOC_COUNT; ++i)
			ptr[i] = mp.oalloc();

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
		{
			test* data = ptr[i];
			if (InterlockedIncrement(&data->_value) != 0x1122334455667789) __debugbreak();
			*(data->_ptr) = 0xaa;
		}

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
		{
			test* data = ptr[i];
			if (InterlockedDecrement(&data->_value) != 0x1122334455667788) __debugbreak();
		}

		Sleep(0);

		for (int i = 0; i < ALLOC_COUNT; ++i)
			mp.ofree(ptr[i]);

		Sleep(0);

		if (!start) break;
	}

	Sleep(3000);

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

	for (;;)
	{
		char c = _getch();

		if (c == 'q')
		{
			start = 0;
			break;
		}
	}

	DWORD wait = WaitForMultipleObjects(THREAD_COUNT, handles, TRUE, INFINITE);
	if (wait != WAIT_OBJECT_0) __debugbreak();
	for (int i = 0; i < THREAD_COUNT; ++i)
#pragma warning(suppress:6001)
#pragma warning(suppress:28183)
		CloseHandle(handles[i]);
	timeEndPeriod(1);

	return 0;
}
