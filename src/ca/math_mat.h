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

		template <u32 N> using fmat_t = mat_t<f32, N>;
		template <u32 N> using dmat_t = mat_t<f64, N>;

		typedef fmat_t<2> fmat2_t;
		typedef fmat_t<3> fmat3_t;
		typedef fmat_t<4> fmat4_t;

		typedef dmat_t<2> dmat2_t;
		typedef dmat_t<3> dmat3_t;
		typedef dmat_t<4> dmat4_t;

		//--------------------
		// operator overloads

		//TODO list them

		//-------------------
		// library functions

		template <typename T>
		inline T determinant(mat2_t<T> const & M);

		template <typename T>
		inline T determinant(mat3_t<T> const & M);

		template <typename T, u32 N>
		inline T determinant(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline mat_t<T, N> inverse(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline mat_t<T, N> transpose(mat_t<T, N> const & M);

		template <typename T, u32 N>
		inline mat_t<T, N> & transpose_in_place(mat_t<T, N> & M);

		template <typename T, u32 N>
		inline mat_t<T, N - 1> submatrix(mat_t<T, N> const & M, u32 remove_i, u32 remove_j);

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_column(mat_t<T, N> & M, vec_t<T, C> const & column);

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_diagonal(mat_t<T, N> & M, vec_t<T, C> const & diagonal);

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_diagonal(mat_t<T, N> & M, T diagonal_value);

		template <u32 K0, typename T, u32 N>
		inline void submatrix_set_lower_identity(mat_t<T, N> & M);

		template <u32 I0, u32 J0, u32 CI, u32 CJ, typename T, u32 N>
		inline void submatrix_set_rect(mat_t<T, N> & M, T value);

		//-------------------
		// utility functions

		template <typename T, u32 N>
		inline void set_identity(mat_t<T, N> & M);

		template <typename T>
		inline void set_look_at(mat4_t<T> & M, vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up);

		template <typename T>
		inline void set_parallel_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline void set_parallel_projection(mat4_t<T> & M, T vertical_size, T aspect_ratio, T near, T far);

		template <typename T>
		inline void set_perspective_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline void set_perspective_projection(mat4_t<T> & M, T vertical_fov, T aspect_ratio, T near, T far);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_x(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_y(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_angle_z(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_axis_angle(mat_t<T, N> & M, vec3_t<T> const & axis, T theta);

		template <typename T, u32 N>
		inline void set_rotation_by_forward_up(mat_t<T, N> & M, vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T, u32 N>
		inline void set_rotation_by_quaternion(mat_t<T, N> & M, quat_t<T> const & q);

		template <typename T, u32 N, u32 C>
		inline void set_scaling(mat_t<T, N> & M, vec_t<T, C> const & s);

		template <typename T, u32 N, u32 C>
		inline void set_translation(mat_t<T, N> & M, vec_t<T, C> const & t);

		// - - - -

		template <typename T>
		inline mat2_t<T> mat2_identity();

		template <typename T>
		inline mat2_t<T> mat2_rotation(T theta);

		template <typename T>
		inline mat2_t<T> mat2_scaling(vec2_t<T> const & s);

		// - - - -

		template <typename T>
		inline mat3_t<T> mat3_identity();

		template <typename T>
		inline mat3_t<T> mat3_rotation(T theta);

		template <typename T>
		inline mat3_t<T> mat3_rotation(quat_t<T> const & q);

		template <typename T>
		inline mat3_t<T> mat3_rotation(vec3_t<T> const & axis, T theta);

		template <typename T>
		inline mat3_t<T> mat3_rotation(vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T>
		inline mat3_t<T> mat3_rotation_x(T theta);

		template <typename T>
		inline mat3_t<T> mat3_rotation_y(T theta);

		template <typename T>
		inline mat3_t<T> mat3_rotation_z(T theta);

		template <typename T>
		inline mat3_t<T> mat3_scaling(vec2_t<T> const & s);

		template <typename T>
		inline mat3_t<T> mat3_scaling(vec3_t<T> const & s);

		template <typename T>
		inline mat3_t<T> mat3_translation(vec2_t<T> const & t);

		// - - - -

		template <typename T>
		inline mat4_t<T> mat4_identity();

		template <typename T>
		inline mat4_t<T> mat4_look_at(vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up);

		template <typename T>
		inline mat4_t<T> mat4_parallel(T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline mat4_t<T> mat4_parallel(T vertical_size, T aspect_ratio, T near, T far);

		template <typename T>
		inline mat4_t<T> mat4_perspective(T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		inline mat4_t<T> mat4_perspective(T vertical_fov, T aspect_ratio, T near, T far);

		template <typename T>
		inline mat4_t<T> mat4_rotation(quat_t<T> const & q);

		template <typename T>
		inline mat4_t<T> mat4_rotation(vec3_t<T> const & axis, T theta);

		template <typename T>
		inline mat4_t<T> mat4_rotation(vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T>
		inline mat4_t<T> mat4_rotation_x(T theta);

		template <typename T>
		inline mat4_t<T> mat4_rotation_y(T theta);

		template <typename T>
		inline mat4_t<T> mat4_rotation_z(T theta);

		template <typename T>
		inline mat4_t<T> mat4_scaling(vec3_t<T> s);

		template <typename T>
		inline mat4_t<T> mat4_translation(vec3_t<T> t);
	}
}

#include "ca/math_mat.inl"