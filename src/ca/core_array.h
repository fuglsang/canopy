#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace core
	{
		template <typename T, u32 N>
		struct array_t
		{
			T data[N];

			inline T & operator [] (u32 index)
			{
				CA_ASSERT_MSG(index >= 0 && index < N, "array out of bounds: %i vs [0..%d]", index, N - 1);
				return data[index];
			}

			template <typename T, u32 N>
			inline T const & operator [] (u32 index) const
			{
				CA_ASSERT_MSG(index >= 0 && index < N, "array out of bounds: %i vs [0..%d]", index, N - 1);
				return data[index];
			}
		};
	}
}