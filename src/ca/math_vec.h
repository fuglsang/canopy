#pragma once

#include "ca/types.h"
#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		template <typename T, u32 N>
		struct vec_t
		{
			T e[N];
		};

		template<typename T>
		struct vec_t<T, 2>
		{
			union
			{
				T e[2];
				T x, y;
			};
		};

		template<typename T>
		struct vec_t<T, 3>
		{
			union
			{
				T e[3];
				T x, y, z;
			};
		};

		template<typename T>
		struct vec_t<T, 4>
		{
			union
			{
				T e[4];
				T x, y, z, w;
			};
		};

		typedef vec_t<f32, 2> vec2_t;
		typedef vec_t<f32, 3> vec3_t;
		typedef vec_t<f32, 4> vec4_t;

		typedef vec_t<i32, 2> vec2i_t;
		typedef vec_t<i32, 3> vec3i_t;
		typedef vec_t<i32, 4> vec4i_t;

		#include "ca/math_vec.inl"
	}
}