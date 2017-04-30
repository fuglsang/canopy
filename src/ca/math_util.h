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
		inline T max(T const & a, T const & b)
		{
			if (a > b)
				return a;
			else
				return b;
		}

		template <typename T>
		inline T min(T const & a, T const & b)
		{
			if (a < b)
				return a;
			else
				return b;
		}

		template <typename T>
		inline T clamp(T const & a, T const & a0, T const & a1)
		{
			if (a < a0)
				return a0;
			else if (a > a1)
				return a1;
			else
				return a;
		}

		template <typename T, typename S>
		inline T lerp(T const & a0, T const & a1, S t)
		{
			return (S(1) - t) * a0 + t * a1;
		}

		template <typename T>
		inline T rcp(T const & a)
		{
			return (T(1) / a);
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