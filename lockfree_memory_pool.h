#pragma once

#include <windows.h>

template <typename T, bool reuse = true>
class lockfree_memory_pool final {};

template <typename T>
class lockfree_memory_pool<T, true> final
{
public:
	static constexpr unsigned long long _address_max = 0x00007ffffffeffff;

public:
	inline lockfree_memory_pool(void) noexcept
		: _top(0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		if (_address_max ^ (unsigned long long)si.lpMaximumApplicationAddress) __debugbreak();
	}

	inline lockfree_memory_pool(unsigned int count) noexcept
		: _top(0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		if (_address_max ^ (unsigned long long)si.lpMaximumApplicationAddress) __debugbreak();

		// todo
	}

	inline ~lockfree_memory_pool(void) noexcept
	{
		// todo
	}

	lockfree_memory_pool(lockfree_memory_pool&) = delete;
	lockfree_memory_pool(lockfree_memory_pool&&) = delete;

	lockfree_memory_pool& operator=(lockfree_memory_pool&) = delete;
	lockfree_memory_pool& operator=(lockfree_memory_pool&&) = delete;

public:
	inline T* oalloc(void) noexcept
	{
		// todo
	}

	inline void ofree(T* ptr) noexcept
	{
		// todo
	}

private:
	struct NODE
	{
		NODE* _next;
		T _data;
	};

private:
	unsigned long long _top;

};
