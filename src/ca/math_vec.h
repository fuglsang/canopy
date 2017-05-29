#pragma once

#include "ca/types.h"

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

		template<typename T>
		struct vec_t<T, 2>
		{
			union
			{
				T e[2];
				struct { T x, y; };
			};
		};

		template<typename T>
		struct vec_t<T, 3>
		{
			union
			{
				T e[3];
				struct { T x, y, z; };
			};
		};

		template<typename T>
		struct vec_t<T, 4>
		{
			union
			{
				T e[4];
				struct { T x, y, z, w; };
			};
		};

		typedef vec_t<f32, 2> fvec2_t;
		typedef vec_t<f32, 3> fvec3_t;
		typedef vec_t<f32, 4> fvec4_t;

		typedef vec_t<f64, 2> dvec2_t;
		typedef vec_t<f64, 3> dvec3_t;
		typedef vec_t<f64, 4> dvec4_t;

		typedef vec_t<i32, 2> ivec2_t;
		typedef vec_t<i32, 3> ivec3_t;
		typedef vec_t<i32, 4> ivec4_t;

		typedef vec_t<u32, 2> uvec2_t;
		typedef vec_t<u32, 3> uvec3_t;
		typedef vec_t<u32, 4> uvec4_t;

		//--------------------
		// operator overloads

		//TODO list them

		//-------------------
		// library functions

		template <typename T>
		inline T cross(vec_t<T, 2> const & a, vec_t<T, 2> const & b);

		template <typename T>
		inline vec_t<T, 3> cross(vec_t<T, 3> const & a, vec_t<T, 3> const & b);

		template <typename T, u32 N>
		inline T dot(vec_t<T, N> const & a, vec_t<T, N> const & b);

		template <typename T, u32 N>
		inline void homogenize(vec_t<T, N> & v);

		template <typename T, u32 N>
		inline vec_t<T, N> homogenize_copy_of(vec_t<T, N> const & v);

		template <typename T, u32 N>
		inline T norm(vec_t<T, N> const & v);

		template <typename T, u32 N>
		inline void normalize(vec_t<T, N> & v);

		template <typename T, u32 N>
		inline vec_t<T, N> normalize_copy_of(vec_t<T, N> const & v);

		template <u32 I, u32 C, typename T, u32 N>
		inline vec_t<T, C> & subvector(vec_t<T, N> & v);
	}
}

#include "ca/math_vec.inl"