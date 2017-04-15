#pragma once

#include "ca/types.h"
#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		// note: row-major

		template <typename T, u32 N>
		struct mat_t
		{
			T e[N * N];
			vec_t<T, N> row[N];
		};

		template <typename T>
		struct mat_t<T, 2>
		{
			union
			{
				T e[2 * 2];
				T a11, a12;
				T a21, a22;
				vec_t<T, 2> row[2];
			};
		};

		template <typename T>
		struct mat_t<T, 3>
		{
			union
			{
				T e[3 * 3];
				T a11, a12, a13;
				T a21, a22, a23;
				T a31, a32, a33;
				vec_t<T, 3> row[3];
			};
		};

		template <typename T>
		struct mat_t<T, 4>
		{
			union
			{
				T e[4 * 4];
				T a11, a12, a13, a14;
				T a21, a22, a23, a24;
				T a31, a32, a33, a34;
				T a41, a42, a43, a44;
				vec_t<T, 4> row[4];
			};
		};

		typedef mat_t<f32, 2> mat2_t;
		typedef mat_t<f32, 3> mat3_t;
		typedef mat_t<f32, 4> mat4_t;

		#include "ca/math_mat.inl"
	}
}