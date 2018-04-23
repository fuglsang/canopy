#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		// note: row-major

		template <typename T, u32 N>
		struct mat_t
		{
			union
			{
				T e[N * N];
				vec_t<T, N> row[N];
			};
		};

		template <typename T>
		struct mat_t<T, 2>
		{
			union
			{
				T e[2 * 2];
				struct
				{
					T a11, a12;
					T a21, a22;
				};
				vec2_t<T> row[2];
			};
		};

		template <typename T>
		struct mat_t<T, 3>
		{
			union
			{
				T e[3 * 3];
				struct
				{
					T a11, a12, a13;
					T a21, a22, a23;
					T a31, a32, a33;
				};
				vec3_t<T> row[3];
			};
		};

		template <typename T>
		struct mat_t<T, 4>
		{
			union
			{
				T e[4 * 4];
				struct
				{
					T a11, a12, a13, a14;
					T a21, a22, a23, a24;
					T a31, a32, a33, a34;
					T a41, a42, a43, a44;
				};
				vec4_t<T> row[4];
			};
		};

		template <typename T> using mat2_t = mat_t<T, 2>;
		template <typename T> using mat3_t = mat_t<T, 3>;
		template <typename T> using mat4_t = mat_t<T, 4>;

		template <u32 N> using bmat_t = mat_t<bool, N>;
		template <u32 N> using fmat_t = mat_t<f32, N>;
		template <u32 N> using dmat_t = mat_t<f64, N>;

		typedef bmat_t<2> bmat2_t;
		typedef bmat_t<3> bmat3_t;
		typedef bmat_t<4> bmat4_t;

		typedef fmat_t<2> fmat2_t;
		typedef fmat_t<3> fmat3_t;
		typedef fmat_t<4> fmat4_t;

		typedef dmat_t<2> dmat2_t;
		typedef dmat_t<3> dmat3_t;
		typedef dmat_t<4> dmat4_t;
	}
}