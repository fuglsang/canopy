#pragma once

#include "ca_base.h"

namespace ca
{
	namespace math
	{
		template <typename T, u32 N>
		struct vec_t
		{
			union
			{
				T e[N];
			};
		};

		template <typename T>
		struct vec_t<T, 2>
		{
			union
			{
				T e[2];
				struct { T x, y; };
			};
		};

		template <typename T>
		struct vec_t<T, 3>
		{
			union
			{
				T e[3];
				struct { T x, y, z; };
			};
		};

		template <typename T>
		struct vec_t<T, 4>
		{
			union
			{
				T e[4];
				struct { T x, y, z, w; };
			};
		};

		template <typename T> using vec2_t = vec_t<T, 2>;
		template <typename T> using vec3_t = vec_t<T, 3>;
		template <typename T> using vec4_t = vec_t<T, 4>;

		template <u32 N> using bvec_t = vec_t<bool, N>;
		template <u32 N> using fvec_t = vec_t<f32, N>;
		template <u32 N> using dvec_t = vec_t<f64, N>;
		template <u32 N> using ivec_t = vec_t<i32, N>;
		template <u32 N> using uvec_t = vec_t<u32, N>;

		typedef bvec_t<2> bvec2_t;
		typedef bvec_t<3> bvec3_t;
		typedef bvec_t<4> bvec4_t;

		typedef fvec_t<2> fvec2_t;
		typedef fvec_t<3> fvec3_t;
		typedef fvec_t<4> fvec4_t;

		typedef dvec_t<2> dvec2_t;
		typedef dvec_t<3> dvec3_t;
		typedef dvec_t<4> dvec4_t;
		
		typedef ivec_t<2> ivec2_t;
		typedef ivec_t<3> ivec3_t;
		typedef ivec_t<4> ivec4_t;

		typedef uvec_t<2> uvec2_t;
		typedef uvec_t<3> uvec3_t;
		typedef uvec_t<4> uvec4_t;
	}
}