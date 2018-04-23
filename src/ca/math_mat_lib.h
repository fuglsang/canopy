#pragma once

#include "ca/math_mat.h"

namespace ca
{
	namespace math
	{
		//---------
		// utility

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_column(mat_t<T, N> & M, vec_t<T, C> const & column);

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_diagonal(mat_t<T, N> & M, vec_t<T, C> const & diagonal);

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_diagonal(mat_t<T, N> & M, T diagonal_value);

		template <u32 K0, typename T, u32 N>
		CA_INLINE void set_submatrix_lower_identity(mat_t<T, N> & M);

		template <u32 I0, u32 J0, u32 CI, u32 CJ, typename T, u32 N>
		CA_INLINE void set_submatrix_rect(mat_t<T, N> & M, T value);

		//---------
		// library

		template <typename T, u32 N>
		CA_INLINE void set_identity(mat_t<T, N> & M);

		template <typename T>
		CA_INLINE void set_look_at(mat4_t<T> & M, vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up);

		template <typename T>
		CA_INLINE void set_parallel_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		CA_INLINE void set_parallel_projection(mat4_t<T> & M, T vertical_size, T aspect_ratio, T near, T far);

		template <typename T>
		CA_INLINE void set_perspective_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		CA_INLINE void set_perspective_projection(mat4_t<T> & M, T vertical_fov, T aspect_ratio, T near, T far);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_x(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_y(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_z(mat_t<T, N> & M, T theta);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_axis_angle(mat_t<T, N> & M, vec3_t<T> const & axis, T theta);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_forward_up(mat_t<T, N> & M, vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_quaternion(mat_t<T, N> & M, quat_t<T> const & q);

		template <typename T, u32 N, u32 C>
		CA_INLINE void set_scaling(mat_t<T, N> & M, vec_t<T, C> const & s);

		template <typename T, u32 N, u32 C>
		CA_INLINE void set_translation(mat_t<T, N> & M, vec_t<T, C> const & t);

		// - - - -

		template <typename T>
		CA_INLINE mat2_t<T> mat2_identity();

		template <typename T>
		CA_INLINE mat2_t<T> mat2_rotation(T theta);

		template <typename T>
		CA_INLINE mat2_t<T> mat2_scaling(vec2_t<T> const & s);

		// - - - -

		template <typename T>
		CA_INLINE mat3_t<T> mat3_identity();

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(T theta);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(quat_t<T> const & q);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(vec3_t<T> const & axis, T theta);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_x(T theta);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_y(T theta);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_z(T theta);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_scaling(vec2_t<T> const & s);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_scaling(vec3_t<T> const & s);

		template <typename T>
		CA_INLINE mat3_t<T> mat3_translation(vec2_t<T> const & t);

		// - - - -

		template <typename T>
		CA_INLINE mat4_t<T> mat4_identity();

		template <typename T>
		CA_INLINE mat4_t<T> mat4_look_at(vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_parallel(T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_parallel(T vertical_size, T aspect_ratio, T near, T far);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_perspective(T left, T right, T bottom, T top, T near, T far);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_perspective(T vertical_fov, T aspect_ratio, T near, T far);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(quat_t<T> const & q);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(vec3_t<T> const & axis, T theta);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(vec3_t<T> const & forward, vec3_t<T> const & up);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_x(T theta);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_y(T theta);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_z(T theta);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_scaling(vec3_t<T> s);

		template <typename T>
		CA_INLINE mat4_t<T> mat4_translation(vec3_t<T> t);
	}
}

#include "ca/math_mat_lib.inl"