#pragma once

#include "ca/types.h"

#include <math.h>

namespace ca
{
	namespace math
	{
		inline f32 cos(f32 s)
		{
			return ::cosf(s);
		}

		inline f64 cos(f64 d)
		{
			return ::cos(d);
		}

		template <typename T, typename S>
		inline T lerp(T const & a, T const & b, S s)
		{
			return (S(1) - s) * a + s * b;
		}

		template <typename T>
		inline T rcp(T const & a)
		{
			return (T(1) / a);
		}

		inline f32 sin(f32 s)
		{
			return ::sinf(s);
		}

		inline f64 sin(f64 d)
		{
			return ::sin(d);
		}

		inline f32 sqrt(f32 s)
		{
			return ::sqrtf(s);
		}

		inline f64 sqrt(f64 d)
		{
			return ::sqrt(d);
		}

		template <typename T>
		inline void swap(T & a, T & b)
		{
			T c = a;
			a = b;
			b = c;
		}
	}
}