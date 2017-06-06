#pragma once

#include "ca/types.h"

namespace ca
{
	namespace mem
	{
		inline uintptr_t align_up(uintptr_t address, size_t alignment)
		{
			return ((address + (alignment - 1)) & ~(alignment - 1));
		}

		inline uintptr_t align_down(uintptr_t address, size_t alignment)
		{
			return (address & ~(alignment - 1));
		}

		template <typename T>
		inline T * align_up(T * block, size_t alignment)
		{
			return reinterpret_cast<T *>(align_up(reinterpret_cast<uintptr_t>(block), alignment));
		}

		template <typename T>
		inline T * align_down(T * block, size_t alignment)
		{
			return reinterpret_cast<T *>(align_down(reinterpret_cast<uintptr_t>(block), alignment));
		}

		template <typename T>
		inline T * ptr_add(T * block, ptrdiff_t offset)
		{
			intptr_t a = reinterpret_cast<intptr_t>(block);
			intptr_t aa = a + offset;
			return reinterpret_cast<T *>(aa);
		}

		template <typename T>
		inline ptrdiff_t ptr_diff(T * block0, T * block1)
		{
			intptr_t a0 = reinterpret_cast<intptr_t>(block0);
			intptr_t a1 = reinterpret_cast<intptr_t>(block1);
			return (a1 - a0);
		}

		template <typename T>
		inline bool ptr_equals(T * block0, T * block1)
		{
			uintptr_t a0 = reinterpret_cast<uintptr_t>(block0);
			uintptr_t a1 = reinterpret_cast<uintptr_t>(block1);
			return (a1 == a0);
		}
	}
}