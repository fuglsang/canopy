#pragma once

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <typename T>
		CA_INLINE T abs(T const & s)
		{
			if (s < 0)
				return -s;
			else
				return s;
		}

		template <typename T>
		CA_INLINE T frac(T const & s)
		{
			return (s - floor(s));
		}

		template <typename T>
		CA_INLINE T rcp(T const & s)
		{
			return (1 / s);
		}

		template <typename T>
		CA_INLINE T round(T const & s)
		{
			return floor(s + 0.5f);
		}

		template <typename T>
		CA_INLINE T rsqrt(T const & s)
		{
			return rcp(sqrt(s));
		}

		template <typename T>
		CA_INLINE T saturate(T const & s)
		{
			return clamp(s, 0, 1);
		}

		//--------
		// binary

		template <typename T>
		CA_INLINE T min(T const & a, T const & b)
		{
			if (a < b)
				return a;
			else
				return b;
		}

		template <typename T>
		CA_INLINE T max(T const & a, T const & b)
		{
			if (a > b)
				return a;
			else
				return b;
		}

		//---------
		// ternary

		template <typename T>
		CA_INLINE T clamp(T const & s, T const & a, T const & b)
		{
			if (s < a)
				return a;
			else if (s > b)
				return b;
			else
				return s;
		}

		template <typename T>
		CA_INLINE T lerp(T const & a, T const & b, T const & s)
		{
			return (1 - s) * a + s * b;
		}
	}
}