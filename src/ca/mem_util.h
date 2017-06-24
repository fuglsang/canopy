#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace mem
	{
		//TODO split into mem_align.h and mem_ptr.h

		inline bool align_ispow2(size_t alignment)
		{
			return !(alignment & (alignment - 1));
		}

		inline uintptr_t align_up(uintptr_t address, size_t alignment)
		{
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
			return ((address + (alignment - 1)) & ~(alignment - 1));
		}

		inline uintptr_t align_down(uintptr_t address, size_t alignment)
		{
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
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

		template <typename T1, typename T2>
		inline size_t ptr_offsetof(T1 T2::*member)
		{
			T2 x;
			size_t a0 = reinterpret_cast<size_t>(&x);
			size_t a1 = reinterpret_cast<size_t>(&(x.*member));
			return (a1 - a0);
		}
	}
}