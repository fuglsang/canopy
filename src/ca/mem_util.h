#pragma once

#include "ca_base.h"
#include "ca/core_assert.h"

#include <stdint.h>

namespace ca
{
	namespace mem
	{
		//TODO split into mem_align.h and mem_ptr.h ?

		CA_INLINE bool align_ispow2(size_t alignment)
		{
			return !(alignment & (alignment - 1));
		}

		CA_INLINE uintptr_t align_up(uintptr_t address, size_t alignment)
		{
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
			return ((address + (alignment - 1)) & ~(alignment - 1));
		}

		CA_INLINE uintptr_t align_down(uintptr_t address, size_t alignment)
		{
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
			return (address & ~(alignment - 1));
		}

		template <typename T>
		CA_INLINE T * align_up(T * block, size_t alignment)
		{
			return reinterpret_cast<T *>(align_up(reinterpret_cast<uintptr_t>(block), alignment));
		}

		template <typename T>
		CA_INLINE T * align_down(T * block, size_t alignment)
		{
			return reinterpret_cast<T *>(align_down(reinterpret_cast<uintptr_t>(block), alignment));
		}

		template <typename T>
		CA_INLINE T * ptr_add(T * block, ptrdiff_t offset)
		{
			intptr_t a = reinterpret_cast<intptr_t>(block);
			intptr_t aa = a + offset;
			return reinterpret_cast<T *>(aa);
		}

		template <typename T>
		CA_INLINE ptrdiff_t ptr_diff(T * block0, T * block1)
		{
			intptr_t a0 = reinterpret_cast<intptr_t>(block0);
			intptr_t a1 = reinterpret_cast<intptr_t>(block1);
			return (a1 - a0);
		}

		template <typename T>
		CA_INLINE bool ptr_equals(T * block0, T * block1)
		{
			uintptr_t a0 = reinterpret_cast<uintptr_t>(block0);
			uintptr_t a1 = reinterpret_cast<uintptr_t>(block1);
			return (a1 == a0);
		}

		template <typename T, typename C>
		CA_INLINE size_t ptr_offsetof(T C::*member)
		{
			C x;
			size_t a0 = reinterpret_cast<size_t>(&x);
			size_t a1 = reinterpret_cast<size_t>(&(x.*member));
			return (a1 - a0);
		}
	}
}