#pragma once

#include "ca/types.h"
#include "ca/math_vec.h"
#include "ca/math_quat.h"

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
				vec_t<T, 2> row[2];
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
				vec_t<T, 3> row[3];
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
				vec_t<T, 4> row[4];
			};
		};

		typedef mat_t<f32, 2> fmat2_t;
		typedef mat_t<f32, 3> fmat3_t;
		typedef mat_t<f32, 4> fmat4_t;

		typedef mat_t<f64, 2> dmat2_t;
		typedef mat_t<f64, 3> dmat3_t;
		typedef mat_t<f64, 4> dmat4_t;

		//--------------------
		// operator overloads

		//TODO list them

		//-------------------
		// library functions
		
		template <typename T, u32 N>
		inline mat_t<T, N - 1> submatrix(mat_t<T, N> const & M, u32 remove_i, u32 remove_j);

		template <typename T>
		inline T determinant(mat_t<T, 2> const & M);

		template <typename T>
		inline T determinant(mat_t<T, 3> const & M);

		template <typename T, u32 N>
		inline T determinant(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline void transpose(mat_t<T, N> & M);

		template <typename T, u32 N>
		inline mat_t<T, N> transpose_copy_of(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline void invert(mat_t<T, N> & M);

		template <typename T, u32 N>
		inline mat_t<T, N> invert_copy_of(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline void set_zero(mat_t<T, N> & M);

		template <typename T, u32 N>
		inline void set_identity(mat_t<T, N> & M);

		//TODO
		// make_affine_translation
		// make_affine_rotation
		// make_affine_scaling

		template <typename T>
		inline void set_rotation_by_angle(mat_t<T, 2> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_x(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_y(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_z(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_axis_angle(mat_t<T, N> & M, vec_t<T, 3> const & axis, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_quaternion(mat_t<T, N> & M, quat_t<T> const & q);

		template <typename T, u32 N>
		inline void set_rotation_by_look_direction(mat_t<T, N> & M, vec_t<T, 3> const & forward, vec_t<T, 3> const & up);

		template <typename T, u32 N>
		inline void set_translation(mat_t<T, N> & M, vec_t<T, N - 1> const & v);

		template <typename T>
		inline void set_parallel_projection(mat_t<T, 4> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline void set_perspective_projection(mat_t<T, 4> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline void set_perspective_projection(mat_t<T, 4> & M, T vertical_fov, T aspect_ratio, T near, T far);
	}
}

#include "ca/math_mat.inl"