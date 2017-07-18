#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		//--------------------
		// operator overloads

		template <typename T, u32 N>
		inline mat_t<T, N> operator* (mat_t<T, N> const & A, mat_t<T, N> const & B)
		{
			mat_t<T, N> M;
			for (u32 i = 0; i != N; i++)
			{
				for (u32 j = 0; j != N; j++)
				{
					M.row[i].e[j] = A.row[i].e[0] * B.row[0].e[j];
					for (u32 k = 1; k != N; k++)
					{
						M.row[i].e[j] += A.row[i].e[k] * B.row[k].e[j];
					}
				}
			}
			return M;
		}

		template <typename T, u32 N>
		inline vec_t<T, N> operator* (mat_t<T, N> const & A, vec_t<T, N> const & b)
		{
			vec_t<T, N> v;
			for (u32 i = 0; i != N; i++)
			{
				v.e[i] = dot(A.row[i], b);
			}
			return v;
		}

		#define CA_DEFINE_MATRIX_OP_MATRIX(OP)											\
		template <typename T, u32 N>													\
		inline mat_t<T, N> operator OP (mat_t<T, N> const & A, mat_t<T, N> const & B)	\
		{																				\
			mat_t<T, N> M;																\
			for (u32 i = 0; i != N * N; i++)											\
			{																			\
				M.e[i] = A.e[i] OP B.e[i];												\
			}																			\
			return M;																	\
		}																				\
																						\
		template <typename T, u32 N>													\
		inline mat_t<T, N> & operator OP##= (mat_t<T, N> & A, mat_t<T, N> const & B)	\
		{																				\
			return (A = (A OP B));														\
		}

		#define CA_DEFINE_MATRIX_OP_SCALAR(OP)											\
		template <typename T, u32 N>													\
		inline mat_t<T, N> operator OP (mat_t<T, N> const & A, T const b)				\
		{																				\
			mat_t<T, N> M;																\
			for (u32 i = 0; i != N * N; i++)											\
			{																			\
				M.e[i] = A.e[i] OP b;													\
			}																			\
			return M;																	\
		}																				\
																						\
		template <typename T, u32 N>													\
		inline mat_t<T, N> & operator OP##= (mat_t<T, N> & A, T const b)				\
		{																				\
			return (A = (A OP b));														\
		}

		#define CA_DEFINE_SCALAR_OP_MATRIX(OP)											\
		template <typename T, u32 N>													\
		inline mat_t<T, N> operator OP (T const a, mat_t<T, N> const & B)				\
		{																				\
			mat_t<T, N> M;																\
			for (u32 i = 0; i != N * N; i++)											\
			{																			\
				M.e[i] = a OP B.e[i];													\
			}																			\
			return M;																	\
		}

		CA_DEFINE_MATRIX_OP_MATRIX(+)
		CA_DEFINE_MATRIX_OP_MATRIX(-)

		template <typename T, u32 N>
		inline mat_t<T, N> operator- (mat_t<T, N> const & A)
		{
			mat_t<T, N> M;
			for (u32 i = 0; i != N * N; i++)
			{
				M.e[i] = -A.e[i];
			}
			return M;
		}

		CA_DEFINE_MATRIX_OP_SCALAR(*)
		CA_DEFINE_MATRIX_OP_SCALAR(/)

		CA_DEFINE_SCALAR_OP_MATRIX(*)
		CA_DEFINE_SCALAR_OP_MATRIX(/)

		#undef CA_DEFINE_MATRIX_OP_MATRIX
		#undef CA_DEFINE_MATRIX_OP_SCALAR
		#undef CA_DEFINE_SCALAR_OP_MATRIX

		//-------------------
		// library functions

		template <typename T>
		inline T determinant(mat2_t<T> const & M)
		{
			// | a11 a12 |
			// | a21 a22 |
			T add = (M.a11 * M.a22);
			T sub = (M.a12 * M.a21);
			return add - sub;
		}

		template <typename T>
		inline T determinant(mat3_t<T> const & M)
		{
			T add = (M.a11 * M.a22 * M.a33) + (M.a12 * M.a23 * M.a31) + (M.a13 * M.a21 * M.a32);
			T sub = (M.a13 * M.a22 * M.a31) + (M.a12 * M.a21 * M.a33) + (M.a11 * M.a23 * M.a32);
			return add - sub;
		}

		template <typename T, u32 N>
		inline T determinant(mat_t<T, N> const & M)
		{
			T val = T(0);
			T sgn = T(1);
			for (u32 j = 0; j != N; j++)
			{
				val += sgn * M.row[0].e[i] * determinant(submatrix(M, 0, j));
				sgn *= T(-1);
			}
			return val;
		}

		template <typename T, u32 N>
		inline mat_t<T, N> inverse(mat_t<T, N> const & M)
		{
			mat_t<T, N> M_minors;
			mat_t<T, N> M_cofactors;
			mat_t<T, N> M_adjugate;
			T det_val = T(0);
			T det_sgn = T(1);

			// calc minors
			for (u32 i = 0; i != N; i++)
			{
				for (u32 j = 0; j != N; j++)
				{
					M_minors.row[i].e[j] = determinant(submatrix(M, i, j));
				}
			}

			// calc cofactors
			M_cofactors = M_minors;
			for (u32 i = 0; i != N; i++)
			{
				for (u32 j = 1 - (i % 2); j < N; j += 2)
				{
					M_cofactors.row[i].e[j] = -M_cofactors.row[i].e[j];//TODO verify
				}
			}

			// calc adjugate
			M_adjugate = transpose(M_cofactors);

			// calc determinant
			for (u32 j = 0; j != N; j++)
			{
				det_val += det_sgn * M.row[0].e[j] * M_minors.row[0].e[j];
				det_sgn *= T(-1);
			}

			// calc inverse
			CA_ASSERT_MSG(det_val != T(0), "singular matrix not invertible");
			return rcp(det_val) * M_adjugate;
		}

		template <typename T, u32 N>
		inline mat_t<T, N> transpose(mat_t<T, N> const & M)
		{
			mat_t<T, N> M_transpose = M;
			return transpose_in_place(M_transpose);
		}

		template <typename T, u32 N>
		inline mat_t<T, N> & transpose_in_place(mat_t<T, N> & M)
		{
			for (u32 i = 0; i != N; i++)
			{
				for (u32 j = i + 1; j != N; j++)
				{
					swap(M.row[i].e[j], M.row[j].e[i]);
				}
			}
			return M;
		}

		template <typename T, u32 N>
		inline mat_t<T, N - 1> submatrix(mat_t<T, N> const & M, u32 remove_i, u32 remove_j)
		{
			mat_t<T, N - 1> M_sub;
			for (u32 i = 0; i != remove_i; i++)
			{
				for (u32 j = 0; j != remove_j; j++)
				{
					M_sub.row[i].e[j] = M.row[i].e[j];
				}
				for (u32 j = remove_j + 1; j != N; j++)
				{
					M_sub.row[i].e[j - 1] = M.row[i].e[j];
				}
			}
			for (u32 i = remove_i + 1; i != N; i++)
			{
				for (u32 j = 0; j != remove_j; j++)
				{
					M_sub.row[i - 1].e[j] = M.row[i].e[j];
				}
				for (u32 j = remove_j + 1; j != N; j++)
				{
					M_sub.row[i - 1].e[j - 1] = M.row[i].e[j];
				}
			}
			return M_sub;
		}
		
		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_column(mat_t<T, N> & M, vec_t<T, C> const & column)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + 1 <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0] = column.e[k];
			}
		}

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_diagonal(mat_t<T, N> & M, T diagonal_value)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + C <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0 + k] = diagonal_value;
			}
		}

		template <u32 I0, u32 J0, u32 C, typename T, u32 N>
		inline void submatrix_set_diagonal(mat_t<T, N> & M, vec_t<T, C> const & diagonal)
		{
			static_assert(I0 + C <= N, "out of bounds");
			static_assert(J0 + C <= N, "out of bounds");
			for (u32 k = 0; k != C; k++)
			{
				M.row[I0 + k].e[J0 + k] = diagonal.e[k];
			}
		}

		template <u32 K, typename T, u32 N>
		inline void submatrix_set_lower_identity(mat_t<T, N> & M)
		{
			submatrix_set_rect<0, K, K, N - K>(M, T(0));
			submatrix_set_rect<K, 0, N - K, N>(M, T(0));
			submatrix_set_diagonal<K, K, N - K>(M, T(1));
		}

		template <u32 I0, u32 J0, u32 CI, u32 CJ, typename T, u32 N>
		inline void submatrix_set_rect(mat_t<T, N> & M, T value)
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

		//-------------------
		// utility functions

		template <typename T, u32 N>
		inline void set_identity(mat_t<T, N> & M)
		{
			submatrix_set_rect<0, 0, N, N>(M, T(0));
			submatrix_set_diagonal<0, 0, N>(M, T(1));
		}

		template <typename T>
		inline void set_look_at(mat4_t<T> & M, vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up)
		{
			mat4_t<T> M_r;
			mat4_t<T> M_t;
			
			set_rotation_by_forward_up(M_r, normalize(p1 - p0), up);
			set_translation(M_t, p0);

			M = M_t * M_r;
		}

		template <typename T>
		inline void set_parallel_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far)
		{
			// see article by Song Ho Ahn
			// http://www.songho.ca/opengl/gl_projectionmatrix.html#ortho

			T const x = T(2) / (right - left);
			T const y = T(2) / (top - bottom);
			T const z = -T(2) / (far - near);
			T const a = -(right + left) / (right - left);
			T const b = -(top + bottom) / (top - bottom);
			T const c = -(far + near) / (far - near);
			T const d = T(1);

			M.row[0] = { x, 0, 0, a };
			M.row[1] = { 0, y, 0, b };
			M.row[2] = { 0, 0, z, c };
			M.row[3] = { 0, 0, 0, d };
		}

		template <typename T>
		inline void set_parallel_projection(mat4_t<T> & M, T vertical_size, T aspect_ratio, T near, T far)
		{
			T const near_extent_y = rcp(T(2)) * vertical_size;
			T const near_extent_x = aspect_ratio * extent_y;

			set_parallel_projection(M, -near_extent_x, near_extent_x, -near_extent_y, near_extent_y, near, far);
		}

		template <typename T>
		inline void set_perspective_projection(mat4_t<T> & M, T left, T right, T bottom, T top, T near, T far)
		{
			// see article by Song Ho Ahn
			// http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective

			T const x = (T(2) * near) / (right - left);
			T const y = (T(2) * near) / (top - bottom);
			T const a = (right + left) / (right - left);
			T const b = (top + bottom) / (top - bottom);
			T const c = -(far + near) / (far - near);
			T const d = -(T(2) * far * near) / (far - near);
			T const e = -T(1);

			M.row[0] = { x, 0, a, 0 };
			M.row[1] = { 0, y, b, 0 };
			M.row[2] = { 0, 0, c, d };
			M.row[3] = { 0, 0, e, 0 };
		}

		template <typename T>
		inline void set_perspective_projection(mat4_t<T> & M, T vertical_fov, T aspect_ratio, T near, T far)
		{
			T const one_extent_y = tan(vertical_fov * rcp(T(2)));
			T const one_extent_x = aspect_ratio * one_extent_y;

			T const near_extent_y = near * one_extent_y;
			T const near_extent_x = near * one_extent_x;

			set_perspective_projection(M, -near_extent_x, near_extent_x, -near_extent_y, near_extent_y, near, far);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_angle_x(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { T(1), T(0), T(0) };
			subvector<0, 3>(M.row[1]) = { T(0), c, -s };
			subvector<0, 3>(M.row[2]) = { T(0), s, c };
			submatrix_set_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_angle_y(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { c, T(0), s };
			subvector<0, 3>(M.row[1]) = { T(0), T(1), T(0) };
			subvector<0, 3>(M.row[2]) = { -s, T(0), c };
			
			submatrix_set_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_angle_z(mat_t<T, N> & M, T theta)
		{
			static_assert(N >= 2, "require N >= 2");
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 2>(M.row[0]) = { c, -s };
			subvector<0, 2>(M.row[1]) = { s, c };
			
			submatrix_set_lower_identity<2>(M);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_axis_angle(mat_t<T, N> & M, vec3_t<T> const & axis, T theta)
		{
			static_assert(N >= 3, "require N >= 3");
			T const c = cos(theta);
			T const s = sin(theta);

			T const k = T(1) - c;
			T const k_xy = k * axis.x * axis.y;
			T const k_xz = k * axis.x * axis.z;
			T const k_yz = k * axis.y * axis.z;
			
			vec3_t<T> const s_axis = s * axis;

			subvector<0, 3>(M.row[0]) = { k * axis.x * axis.x + c, k_xy - s_axis.z, k_xz + s_axis.y };
			subvector<0, 3>(M.row[1]) = { k_xy + s_axis.z, k * axis.y * axis.y + c, k_yz - s_axis.x };
			subvector<0, 3>(M.row[2]) = { k_xz - s_axis.y, k_yz + s_axis.x, k * axis.z * axis.z + c };
			
			submatrix_set_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_forward_up(mat_t<T, N> & M, vec3_t<T> const & forward, vec3_t<T> const & up)
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
			
			submatrix_set_lower_identity<3>(M);
		}

		template <typename T, u32 N>
		inline void set_rotation_by_quaternion(mat_t<T, N> & M, quat_t<T> const & q)
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

			subvector<0, 3>(M.row[0]) = { T(1) - yy - zz, xy - w.z, xz + w.y };
			subvector<0, 3>(M.row[1]) = { xy + w.z, T(1) - xx - zz, yz - w.x };
			subvector<0, 3>(M.row[2]) = { xz - w.y, yz + w.x, T(1) - xx - yy };
			
			submatrix_set_lower_identity<3>(M);
		}

		template <typename T, u32 N, u32 C>
		inline void set_scaling(mat_t<T, N> & M, vec_t<T, C> const & s)
		{
			static_assert(N >= C, "require N >= dim s");
			submatrix_set_rect<0, 0, N, N>(M, T(0));
			submatrix_set_diagonal<0, 0>(M, s);
			submatrix_set_diagonal<C, C, N - C>(M, T(1));
		}

		template <typename T, u32 N, u32 C>
		inline void set_translation(mat_t<T, N> & M, vec_t<T, C> const & t)
		{
			static_assert(N > C, "require N > dim t");
			submatrix_set_rect<0, 0, N, N - 1>(M, T(0));
			submatrix_set_diagonal<0, 0, N>(M, T(1));
			submatrix_set_column<0, N - 1>(M, t);
		}

		// - - - -

		template <typename T>
		inline mat2_t<T> mat2_identity()
		{
			mat2_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		inline mat2_t<T> mat2_rotation(T theta)
		{
			mat2_t<T> M;
			set_rotation_by_angle_z(M, theta);
			return M;
		}

		template <typename T>
		inline mat2_t<T> mat2_scaling(vec2_t<T> const & s)
		{
			mat2_t<T> M;
			set_scaling(M, s);
			return M;
		}

		// - - - -

		template <typename T>
		inline mat3_t<T> mat3_identity()
		{
			mat3_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_z(M);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation(quat_t<T> const & q)
		{
			mat3_t<T> M;
			set_rotation_by_quaternion(M);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation(vec3_t<T> const & axis, T theta)
		{
			mat3_t<T> M;
			set_rotation_by_axis_angle(M, axis, angle);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation(vec3_t<T> const & forward, vec3_t<T> const & up)
		{
			mat3_t<T> M;
			set_rotation_by_forward_up(M, forward, up);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation_x(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_x(M, angle);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation_y(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_y(M, angle);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_rotation_z(T theta)
		{
			mat3_t<T> M;
			set_rotation_by_angle_z(M, angle);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_scaling(vec2_t<T> const & s)
		{
			mat3_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_scaling(vec3_t<T> const & s)
		{
			mat3_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		inline mat3_t<T> mat3_translation(vec2_t<T> const & t)
		{
			mat3_t<T> M;
			set_translation(M, t);
			return M;
		}

		// - - - -

		template <typename T>
		inline mat4_t<T> mat4_identity()
		{
			mat4_t<T> M;
			set_identity(M);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_look_at(vec3_t<T> const & p0, vec3_t<T> const & p1, vec3_t<T> const & up)
		{
			mat4_t<T> M;
			set_look_at(M, p0, p1, up);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_parallel(T left, T right, T bottom, T top, T near, T far)
		{
			mat4_t<T> M;
			set_parallel_projection(M, left, right, bottom, top, near, far);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_parallel(T vertical_size, T aspect_ratio, T near, T far)
		{
			mat4_t<T> M;
			set_parallel_projection(M, vertical_size, aspect_ratio, near, far);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_perspective(T left, T right, T bottom, T top, T near, T far)
		{
			mat4_t<T> M;
			set_perspective_projection(M, left, right, bottom, top, near, far);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_perspective(T vertical_fov, T aspect_ratio, T near, T far)
		{
			mat4_t<T> M;
			set_perspective_projection(M, vertical_fov, aspect_ratio, near, far);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation(quat_t<T> const & q)
		{
			mat4_t<T> M;
			set_rotation_by_quaternion(M, q);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation(vec3_t<T> const & axis, T theta)
		{
			mat4_t<T> M;
			set_rotation_by_axis_angle(M, axis, theta);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation(vec3_t<T> const & forward, vec3_t<T> const & up)
		{
			mat4_t<T> M;
			set_rotation_by_forward_up(M, axis, theta);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation_x(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_x(M, theta);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation_y(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_y(M, theta);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_rotation_z(T theta)
		{
			mat4_t<T> M;
			set_rotation_by_angle_z(M, theta);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_scaling(vec3_t<T> s)
		{
			mat4_t<T> M;
			set_scaling(M, s);
			return M;
		}

		template <typename T>
		inline mat4_t<T> mat4_translation(vec3_t<T> t)
		{
			mat4_t<T> M;
			set_translation(M, t);
			return M;
		}
	}
}