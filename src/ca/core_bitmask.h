#pragma once

#include "ca/types.h"

namespace ca
{
	namespace core
	{
		// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
		inline u32 bitmask_popcount(u32 mask)
		{
			mask = mask - ((mask >> 1) & 0x55555555);
			mask = (mask & 0x33333333) + ((mask >> 2) & 0x33333333);
			return ((mask + (mask >> 4) & 0xf0f0f0f) * 0x1010101) >> 24;
		}

		inline u32 bitmask_popcount(i32 mask)
		{
			mask = mask - ((mask >> 1) & 0x55555555);
			mask = (mask & 0x33333333) + ((mask >> 2) & 0x33333333);
			return ((mask + (mask >> 4) & 0xf0f0f0f) * 0x1010101) >> 24;
		}
	}
}