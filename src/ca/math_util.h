#pragma once

#include "ca/types.h"

#include <math.h>

namespace ca
{
	namespace math
	{
		//TODO move definitions to .inl

		template <typename T>
		inline T abs(T const & s)
		{
			if (s < T(0))
				return -s;
			else
				return s;
		}

		inline f32 acos(f32 s)
		{
			return ::acosf(s);
		}

		inline f64 acos(f64 d)
		{
			return ::acos(d);
		}

		inline f32 asin(f32 s)
		{
			return ::asinf(s);
		}

		inline f64 asin(f64 d)
		{
			return ::asin(d);
		}

		inline f32 atan(f32 s)
		{
			return ::atanf(s);
		}

		inline f64 atan(f64 d)
		{
			return ::atan(d);
		}

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

		inline f32 tan(f32 s)
		{
			return ::tanf(s);
		}

		inline f64 tan(f64 d)
		{
			return ::tan(d);
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
		inline T rcp(T const & a)
		{
			return (T(1) / a);
		}

		template <typename T>
		inline T rcp_sqrt(T const & a)
		{
			return rcp(sqrt(a));
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
		
		template <typename T>
		inline T saturate(T const & a)
		{
			return clamp(a, T(0), T(1));
		}

		template <typename T, typename S>
		inline T lerp(T const & a0, T const & a1, S const & t)
		{
			return (S(1) - t) * a0 + t * a1;
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