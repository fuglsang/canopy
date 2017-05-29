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
					for (u32 k = 0; k != N; k++)
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

		template <typename T>
		inline T determinant(mat_t<T, 2> const & M)
		{
			// | a11 a12 |
			// | a21 a22 |
			T add = (M.a11 * M.a22);
			T sub = (M.a12 * M.a21);
			return add - sub;
		}

		template <typename T>
		inline T determinant(mat_t<T, 3> const & M)
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
		inline void transpose(mat_t<T, N> & M)
		{
			for (u32 i = 0; i != N; i++)
			{
				for (u32 j = i + 1; j != N; j++)
				{
					swap(M.row[i].e[j], M.row[j].e[i]);
				}
			}
		}

		template <typename T, u32 N>
		inline mat_t<T, N> transpose_copy_of(mat_t<T, N> const & M)
		{
			mat_t<T, N> copy = M;
			transpose(copy);
			return copy;
		}

		template <typename T, u32 N>
		inline void invert(mat_t<T, N> & M)
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
			M_adjugate = transpose_copy_of(M_cofactors);

			// calc determinant
			for (u32 j = 0; j != N; j++)
			{
				det_val += det_sgn * M.row[0].e[j] * M_minors[0].e[j];
				det_sgn *= T(-1);
			}

			// calc inverse
			CA_ASSERT_MSG(det_val != T(0), "singular matrix not invertible");
			M = rcp(det_val) * M_adjugate;
		}

		template <typename T, u32 N>
		inline mat_t<T, N> invert_copy_of(mat_t<T, N> const & M)
		{
			mat_t<T, N> copy = M;
			invert(copy);
			return copy;
		}

		template <typename T, u32 N>
		inline void set_zero(mat_t<T, N> & M)
		{
			for (i32 i = 0; i != N * N; i++)
			{
				M.e[i] = T(0);
			}
		}

		template <typename T, u32 N>
		inline void set_identity(mat_t<T, N> & M)
		{
			set_zero(M);
			for (i32 i = 0; i != N; i++)
			{
				M.row[i].e[i] = T(1);
			}
		}

		template <typename T>
		inline void set_rotation_by_angle(mat_t<T, 2> & M, T theta)
		{
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 2>(M.row[0]) = { c, -s };
			subvector<0, 2>(M.row[1]) = { s, c };
		}

		template <typename T>
		inline void set_rotation_by_angle_x(mat_t<T, 3> & M, T theta)
		{
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { T(0), T(0), T(0) };
			subvector<0, 3>(M.row[1]) = { T(0), c, -s };
			subvector<0, 3>(M.row[2]) = { T(0), s, c };
		}

		template <typename T>
		inline void set_rotation_by_angle_y(mat_t<T, 3> & M, T theta)
		{
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { c, T(0), s };
			subvector<0, 3>(M.row[1]) = { T(0), T(0), T(0) };
			subvector<0, 3>(M.row[2]) = { -s, T(0), c };
		}

		template <typename T>
		inline void set_rotation_by_angle_z(mat_t<T, 3> & M, T theta)
		{
			T const c = cos(theta);
			T const s = sin(theta);

			subvector<0, 3>(M.row[0]) = { c, -s, T(0) };
			subvector<0, 3>(M.row[1]) = { s, c, T(0) };
			subvector<0, 3>(M.row[2]) = { T(0), T(0), T(0) };
		}

		template <typename T>
		inline void set_rotation_by_axis_angle(mat_t<T, 3> & M, vec_t<T, 3> const & axis, T theta)
		{
			T const c = cos(theta);
			T const s = sin(theta);

			T const k = T(1) - c;
			T const k_xy = k * axis.x * axis.y;
			T const k_xz = k * axis.x * axis.z;
			T const k_yz = k * axis.y * axis.z;

			vec_t<T, 3> const s_axis = s * axis;

			subvector<0, 3>(M.row[0]) = { k * axis.x * axis.x + c, k_xy - s_axis.z, k_xz + s_axis.y };
			subvector<0, 3>(M.row[1]) = { k_xy + s_axis.z, k * axis.y * axis.y + c, k_yz - s_axis.x };
			subvector<0, 3>(M.row[2]) = { k_xz - s_axis.y, k_yz + s_axis.x, k * axis.z * axis.z + c };
		}

		template <typename T>
		inline void set_rotation_by_quaternion(mat_t<T, 3> & M, quat_t<T> const & q)
		{
			vec_t<T, 3> const v = q.xyz * T(2);
			vec_t<T, 3> const w = q.w * v;

			T const xx = v.x * q.xyz.x;
			T const xy = v.x * q.xyz.y;
			T const xz = v.x * q.xyz.z;
			T const yy = v.y * q.xyz.y;
			T const yz = v.y * q.xyz.z;
			T const zz = v.z * q.xyz.z;

			subvector<0, 3>(M.row[0]) = { T(1) - yy - zz, xy - w.z, xz + w.y };
			subvector<0, 3>(M.row[1]) = { xy + w.z, T(1) - xx - zz, yz - w.x };
			subvector<0, 3>(M.row[2]) = { xz - w.y, yz + w.x, T(1) - xx - yy };
		}

		template <typename T>
		inline void set_rotation_by_look_direction(mat_t<T, 3> & M, vec_t<T, 3> const & forward, vec_t<T, 3> const & up)
		{
			vec_t<T, 3> const axis_z = -forward;
			vec_t<T, 3> const axis_x = cross(up, axis_z);;
			vec_t<T, 3> const axis_y = cross(axis_z, axis_x);

			M.row[0] = axis_x;
			M.row[1] = axis_y;
			M.row[2] = axis_z;
			
			transpose(M);
		}

		template <typename T>
		inline void set_parallel_projection(mat_t<T, 4> & M, T left, T right, T bottom, T top, T near, T far)
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

			M.e[0] = x; M.e[1] = 0; M.e[2] = 0; M.e[4] = a;
			M.e[4] = 0; M.e[5] = y; M.e[6] = 0; M.e[7] = b;
			M.e[8] = 0; M.e[9] = 0; M.e[10] = z; M.e[11] = c;
			M.e[12] = 0; M.e[13] = 0; M.e[14] = 0; M.e[15] = d;
		}

		template <typename T>
		inline void set_perspective_projection(mat_t<T, 4> & M, T left, T right, T bottom, T top, T near, T far)
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

			M.e[0] = x; M.e[1] = 0; M.e[2] = a; M.e[3] = 0;
			M.e[4] = 0; M.e[5] = y; M.e[6] = b; M.e[7] = 0;
			M.e[8] = 0; M.e[9] = 0; M.e[10] = c; M.e[11] = d;
			M.e[12] = 0; M.e[13] = 0; M.e[14] = e; M.e[15] = 0;
		}

		template <typename T>
		inline void set_perspective_projection(mat_t<T, 4> & M, T vertical_fov, T aspect_ratio, T near, T far)
		{
			T const one_extent_y = tan(vertical_fov * rcp(T(2)));
			T const one_extent_x = aspect_ratio * one_extent_y;

			T const near_extent_y = near * one_extent_y;
			T const near_extent_x = near * one_extent_x;

			set_perspective_projection(M, -near_extent_x, near_extent_x, -near_extent_y, near_extent_y, near, far);
		}
	}
}