#pragma once

#include <windows.h>
#include <new>

template <typename T, bool reuse = true>
class lockfree_memory_pool final {};

template <typename T>
class lockfree_memory_pool<T, true> final
{
	using pointer_size = unsigned long long;

public:
	static constexpr pointer_size _user_address_max		= 0x00007ffffffeffff;
	static constexpr pointer_size _user_address_mask	= 0x00007fffffffffff;

	static constexpr pointer_size _top_counter_mask		= 0xffff800000000000;
	static constexpr pointer_size _increment_counter	= 0x0000800000000000;

public:
	inline lockfree_memory_pool(void) noexcept
		: _top(0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		if ((pointer_size)si.lpMaximumApplicationAddress ^ _user_address_max) __debugbreak();
	}

	inline lockfree_memory_pool(unsigned int count) noexcept
		: _top(0)
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		if ((pointer_size)si.lpMaximumApplicationAddress ^ _user_address_max) __debugbreak();

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
		for (;;)
		{
			pointer_size local_top = _top;
			NODE* node = (NODE*)(local_top & _user_address_mask);
			if (node == nullptr)
				return (T*)((char*)(::new(std::nothrow) NODE) + offsetof(NODE, _data));

			pointer_size next_top = (pointer_size)node->_next + (local_top & _top_counter_mask) + _increment_counter;
			pointer_size prev_top = InterlockedCompareExchange(&_top, next_top, local_top);
			if (prev_top == local_top)
				return (T*)((char*)node + offsetof(NODE, _data));
		}
	}

	inline void ofree(T* ptr) noexcept
	{
		NODE* node = (NODE*)((char*)ptr - offsetof(NODE, _data));

		for (;;)
		{
			pointer_size local_top = _top;
			node->_next = (NODE*)(local_top & _user_address_mask);
			pointer_size next_top = (pointer_size)node + (local_top & _top_counter_mask) + _increment_counter;
			pointer_size prev_top = InterlockedCompareExchange(&_top, next_top, local_top);
			if (prev_top == local_top) return;
		}
	}

private:
	struct NODE
	{
		NODE* _next;
		T _data;
	};

private:
	pointer_size _top;

};
