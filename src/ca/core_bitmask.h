#pragma once

#include "ca_base.h"

namespace ca
{
	namespace core
	{
		// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
		CA_INLINE u32 bitmask_popcount(u32 mask)
		{
			mask = mask - ((mask >> 1) & 0x55555555);
			mask = (mask & 0x33333333) + ((mask >> 2) & 0x33333333);
			return ((mask + (mask >> 4) & 0xf0f0f0f) * 0x1010101) >> 24;
		}

		CA_INLINE u32 bitmask_popcount(i32 mask)
		{
			mask = mask - ((mask >> 1) & 0x55555555);
			mask = (mask & 0x33333333) + ((mask >> 2) & 0x33333333);
			return ((mask + (mask >> 4) & 0xf0f0f0f) * 0x1010101) >> 24;
		}
	}
}