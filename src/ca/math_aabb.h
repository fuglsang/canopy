#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		template <typename T>
		struct aabb_t
		{
			T min;
			T max;
		};

		typedef aabb_t<fvec2_t> faabb2_t;
		typedef aabb_t<fvec3_t> faabb3_t;
		typedef aabb_t<fvec4_t> faabb4_t;

		template <typename T>
		inline bool aabb_contains(aabb_t<T> * aabb, T const & value)
		{
			return all(value >= aabb->min) && all(value <= aabb->max);
		}

		template <typename T>
		inline void aabb_include(aabb_t<T> * aabb, T const & value)
		{
			aabb->min = min(aabb->min, value);
			aabb->max = max(aabb->max, value);
		}
	}
}