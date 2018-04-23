#pragma once

#include "ca_base.h"

namespace ca
{
	namespace math
	{
		template <typename T>
		CA_INLINE void swap(T & a, T & b)
		{
			T c = a;
			a = b;
			b = c;
		}
	}
}