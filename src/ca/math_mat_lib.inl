#pragma once

#include "ca/math_mat_ops.h"
	
namespace ca
{
	namespace math
	{
		//---------
		// utility

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_column(mat_t<T, N> & M, vec_t<T, C> const & column)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + 1 <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0] = column.e[k];
			}
		}

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_diagonal(mat_t<T, N> & M, T diagonal_value)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + C <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0 + k] = diagonal_value;
			}
		}

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		CA_INLINE void set_submatrix_diagonal(mat_t<T, N> & M, vec_t<T, C> const & diagonal)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + C <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0 + k] = diagonal.e[k];
			}
		}

		template <u32 K, typename T, u32 N>
		CA_INLINE void set_submatrix_lower_identity(mat_t<T, N> & M)
		{
			set_submatrix_rect<0, K, K, N - K>(M, T(0));
			set_submatrix_rect<K, 0, N - K, N>(M, T(0));
			set_submatrix_diagonal<K, K, N - K>(M, T(1));
		}

		template <u32 I0, u32 J0, u32 CI, u32 CJ, typename T, u32 N>
		CA_INLINE void set_submatrix_rect(mat_t<T, N> & M, T value)
		{
			static_assert(I0 + CI <= N, "out of bounds");
			static_assert(J0 + CJ <= N, "out of bounds");
			for (u32 i = I0; i != I0 + CI; i++)
			{
				for (u32 j = J0; j != J0 + CJ; j++)
				{
					M.row[i].e[j] = value;
				}
			}
		}

		//---------
		// library

		template <typename T, u32 N>
		CA_INLINE void set_identity(mat_t<T, N> & M)
		{
			set_submatrix_rect<0, 0, N, N>(M, 0);
			set_submatrix_diagonal<0, 0, N>(M, 1);
		}

		template <typename T>
		CA_INLINE void set_look_at(mat4_t<T> & M, vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up)
		{
			mat4_t<T> M_r;
			mat4_t<T> M_t;

			set_rotation_by_forward_up(M_r, normalize(p1 - p0), up);
			set_translation(M_t, p0);

			M = M_t * M_r;
		}

		template <typename T>
		CA_INLINE void set_parallel_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far)
		{
			// see article by Song Ho Ahn
			// http://www.songho.ca/opengl/gl_projectionmatrix.html#ortho

			T const x = 2 / (right - left);
			T const y = 2 / (top - bottom);
			T const z = -2 / (far - near);
			T const a = -(right + left) / (right - left);
			T const b = -(top + bottom) / (top - bottom);
			T const c = -(far + near) / (far - near);
			T const d = 1;

			M.row[0] = { x, 0, 0, a };
			M.row[1] = { 0, y, 0, b };
			M.row[2] = { 0, 0, z, c };
			M.row[3] = { 0, 0, 0, d };
		}

		template <typename T>
		CA_INLINE void set_parallel_projection(mat4_t<T> & M, T vertical_size, T aspect_ratio, T near, T far)
		{
			T const near_extent_y = rcp<T>(2) * vertical_size;
			T const near_extent_x = aspect_ratio * extent_y;

			set_parallel_projection(M, -near_extent_x, near_extent_x, -near_extent_y, near_extent_y, near, far);
		}

		template <typename T>
		CA_INLINE void set_perspective_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far)
		{
			// see article by Song Ho Ahn
			// http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective

			T const x = (2 * near) / (right - left);
			T const y = (2 * near) / (top - bottom);
			T const a = (right + left) / (right - left);
			T const b = (top + bottom) / (top - bottom);
			T const c = -(far + near) / (far - near);
			T const d = -(2 * far * near) / (far - near);
			T const e = -1;

			M.row[0] = { x, 0, a, 0 };
			M.row[1] = { 0, y, b, 0 };
			M.row[2] = { 0, 0, c, d };
			M.row[3] = { 0, 0, e, 0 };
		}

		template <typename T>
		CA_INLINE void set_perspective_projection(mat4_t<T> & M, T vertical_fov, T aspect_ratio, T near, T far)
		{
			T const one_extent_y = tan(vertical_fov * rcp<T>(2));
			T const one_extent_x = aspect_ratio * one_extent_y;

			T const near_extent_y = near * one_extent_y;
			T const near_extent_x = near * one_extent_x;

			set_perspective_projection(M, -near_extent_x, near_extent_x, -near_extent_y, near_extent_y, near, far);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_x(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { 1, 0, 0 };
			subvector<0, 3>(M.row[1]) = { 0, c, -s };
			subvector<0, 3>(M.row[2]) = { 0, s, c };
			set_submatrix_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_y(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { c, 0, s };
			subvector<0, 3>(M.row[1]) = { 0, 1, 0 };
			subvector<0, 3>(M.row[2]) = { -s, 0, c };

			set_submatrix_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_angle_z(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 2, "require N >= 2");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 2>(M.row[0]) = { c, -s };
			subvector<0, 2>(M.row[1]) = { s, c };

			set_submatrix_lower_identity<2>(M);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_axis_angle(mat_t<T, N> & M, vec3_t<T> const & axis, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			T const k = 1 - c;
			T const k_xy = k * axis.x * axis.y;
			T const k_xz = k * axis.x * axis.z;
			T const k_yz = k * axis.y * axis.z;

			vec3_t<T> const s_axis = s * axis;

			subvector<0, 3>(M.row[0]) = { k * axis.x * axis.x + c, k_xy - s_axis.z, k_xz + s_axis.y };
			subvector<0, 3>(M.row[1]) = { k_xy + s_axis.z, k * axis.y * axis.y + c, k_yz - s_axis.x };
			subvector<0, 3>(M.row[2]) = { k_xz - s_axis.y, k_yz + s_axis.x, k * axis.z * axis.z + c };

			set_submatrix_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_forward_up(mat_t<T, N> & M, vec3_t<T> const & forward, vec3_t<T> const & up)
		{
			static_assert(N >= 3, "require N >= 3");
			vec3_t<T> const axis_z = -forward;
			vec3_t<T> const axis_x = cross(up, axis_z);;
			vec3_t<T> const axis_y = cross(axis_z, axis_x);

			//CA_LOG("axis_x = %f,%f,%f", axis_x.x, axis_x.y, axis_x.z);
			//CA_LOG("axis_y = %f,%f,%f", axis_y.x, axis_y.y, axis_y.z);
			//CA_LOG("axis_z = %f,%f,%f", axis_z.x, axis_z.y, axis_z.z);

			subvector<0, 3>(M.row[0]) = { axis_x.e[0], axis_y.e[0], axis_z.e[0] };
			subvector<0, 3>(M.row[1]) = { axis_x.e[1], axis_y.e[1], axis_z.e[1] };
			subvector<0, 3>(M.row[2]) = { axis_x.e[2], axis_y.e[2], axis_z.e[2] };

			set_submatrix_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		CA_INLINE void set_rotation_by_quaternion(mat_t<T, N> & M, quat_t<T> const & q)
		{
			static_assert(N >= 3, "require N >= 3");
			vec3_t<T> const v = q.xyz * T(2);
			vec3_t<T> const w = q.w * v;

			T const xx = v.x * q.xyz.x;
			T const xy = v.x * q.xyz.y;
			T const xz = v.x * q.xyz.z;
			T const yy = v.y * q.xyz.y;
			T const yz = v.y * q.xyz.z;
			T const zz = v.z * q.xyz.z;

			subvector<0, 3>(M.row[0]) = { 1 - yy - zz, xy - w.z, xz + w.y };
			subvector<0, 3>(M.row[1]) = { xy + w.z, 1 - xx - zz, yz - w.x };
			subvector<0, 3>(M.row[2]) = { xz - w.y, yz + w.x, 1 - xx - yy };

			set_submatrix_lower_identity<3>(M);
		}

		template <typename T, u32 N, u32 C>
		CA_INLINE void set_scaling(mat_t<T, N> & M, vec_t<T, C> const & s)
		{
			static_assert(N >= C, "require N >= dim s");
			set_submatrix_rect<0, 0, N, N>(M, T(0));
			set_submatrix_diagonal<0, 0>(M, s);
			set_submatrix_diagonal<C, C, N - C>(M, T(1));
		}

		template <typename T, u32 N, u32 C>
		CA_INLINE void set_translation(mat_t<T, N> & M, vec_t<T, C> const & t)
		{
			static_assert(N > C, "require N > dim t");
			set_submatrix_rect<0, 0, N, N - 1>(M, T(0));
			set_submatrix_diagonal<0, 0, N>(M, T(1));
			set_submatrix_column<0, N - 1>(M, t);
		}

		// - - - -

		template <typename T>
		CA_INLINE mat2_t<T> mat2_identity()
		{
			mat2_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		CA_INLINE mat2_t<T> mat2_rotation(T theta)
		{
			mat2_t<T> M;
			set_rotation_by_angle_z(M, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat2_t<T> mat2_scaling(vec2_t<T> const & s)
		{
			mat2_t<T> M;
			set_scaling(M, s);
			return M;
		}

		// - - - -

		template <typename T>
		CA_INLINE mat3_t<T> mat3_identity()
		{
			mat3_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_z(M, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(quat_t<T> const & q)
		{
			mat3_t<T> M;
			set_rotation_by_quaternion(M, q);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(vec3_t<T> const & axis, T theta)
		{
			mat3_t<T> M;
			set_rotation_by_axis_angle(M, axis, angle);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation(vec3_t<T> const & forward, vec3_t<T> const & up)
		{
			mat3_t<T> M;
			set_rotation_by_forward_up(M, forward, up);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_x(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_x(M, angle);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_y(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_y(M, angle);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_rotation_z(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_z(M, angle);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_scaling(vec2_t<T> const & s)
		{
			mat3_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_scaling(vec3_t<T> const & s)
		{
			mat3_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		CA_INLINE mat3_t<T> mat3_translation(vec2_t<T> const & t)
		{
			mat3_t<T> M;
			set_translation(M, t);
			return M;
		}

		// - - - -

		template <typename T>
		CA_INLINE mat4_t<T> mat4_identity()
		{
			mat4_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_look_at(vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up)
		{
			mat4_t<T> M;
			set_look_at(M, p0, p1, up);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_parallel(T left, T right, T bottom, T top, T near, T far)
		{
			mat4_t<T> M;
			set_parallel_projection(M, left, right, bottom, top, near, far);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_parallel(T vertical_size, T aspect_ratio, T near, T far)
		{
			mat4_t<T> M;
			set_parallel_projection(M, vertical_size, aspect_ratio, near, far);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_perspective(T left, T right, T bottom, T top, T near, T far)
		{
			mat4_t<T> M;
			set_perspective_projection(M, left, right, bottom, top, near, far);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_perspective(T vertical_fov, T aspect_ratio, T near, T far)
		{
			mat4_t<T> M;
			set_perspective_projection(M, vertical_fov, aspect_ratio, near, far);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(quat_t<T> const & q)
		{
			mat4_t<T> M;
			set_rotation_by_quaternion(M, q);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(vec3_t<T> const & axis, T theta)
		{
			mat4_t<T> M;
			set_rotation_by_axis_angle(M, axis, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation(vec3_t<T> const & forward, vec3_t<T> const & up)
		{
			mat4_t<T> M;
			set_rotation_by_forward_up(M, axis, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_x(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_x(M, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_y(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_y(M, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_rotation_z(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_z(M, theta);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_scaling(vec3_t<T> s)
		{
			mat4_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		CA_INLINE mat4_t<T> mat4_translation(vec3_t<T> t)
		{
			mat4_t<T> M;
			set_translation(M, t);
			return M;
		}
	}
}