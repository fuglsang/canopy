#pragma once

#include "ca_base.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace core
	{
		template <typename T, u32 N>
		struct array_t
		{
			T e[N];

			CA_INLINE T & operator [] (u32 index)
			{
				CA_ASSERT_MSG(index >= 0 && index < N, "index out of bounds: %d vs [0..%d]", index, N - 1);
				return e[index];
			}

			template <typename T, u32 N>
			CA_INLINE T const & operator [] (u32 index) const
			{
				CA_ASSERT_MSG(index >= 0 && index < N, "index out of bounds: %d vs [0..%d]", index, N - 1);
				return e[index];
			}
		};
	}
}