#pragma once

#include "ca/core_assert.h"

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <typename T>
		CA_INLINE T determinant(mat2_t<T> const & M)
		{
			// | a11 a12 |
			// | a21 a22 |
			T add = (M.a11 * M.a22);
			T sub = (M.a12 * M.a21);
			return add - sub;
		}

		template <typename T>
		CA_INLINE T determinant(mat3_t<T> const & M)
		{
			T add = (M.a11 * M.a22 * M.a33) + (M.a12 * M.a23 * M.a31) + (M.a13 * M.a21 * M.a32);
			T sub = (M.a13 * M.a22 * M.a31) + (M.a12 * M.a21 * M.a33) + (M.a11 * M.a23 * M.a32);
			return add - sub;
		}

		template <typename T, u32 N>
		CA_INLINE T determinant(mat_t<T, N> const & M)
		{
			T val = 0;
			T sgn = 1;
			for (u32 j = 0; j != N; j++)
			{
				val += sgn * M.row[0].e[i] * determinant(submatrix(M, 0, j));
				sgn *= T(-1);
			}
			return val;
		}

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> inverse(mat_t<T, N> const & M)
		{
			mat_t<T, N> M_minors;
			mat_t<T, N> M_cofactors;
			mat_t<T, N> M_adjugate;
			T det_val = 0;
			T det_sgn = 1;

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
			CA_ASSERT_MSG(det_val != 0, "singular matrix not invertible");
			return rcp(det_val) * M_adjugate;
		}

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> transpose(mat_t<T, N> const & M)
		{
			mat_t<T, N> M_transpose = M;
			return transpose_in_place(M_transpose);
		}

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> & transpose_in_place(mat_t<T, N> & M)
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
		CA_INLINE mat_t<T, N - 1> submatrix(mat_t<T, N> const & M, u32 remove_i, u32 remove_j)
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

		//--------
		// binary

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> mul(mat_t<T, N> const & A, mat_t<T, N> const & B)
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
		CA_INLINE vec_t<T, N> mul(mat_t<T, N> const & A, vec_t<T, N> const & b)
		{
			vec_t<T, N> v;
			for (u32 i = 0; i != N; i++)
			{
				v.e[i] = dot(A.row[i], b);
			}
			return v;
		}

		//----------------------
		// component-wise unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(mat_t<T, N> const & M)\
		{\
			mat_t<decltype(op(T())), N> M_out;\
			for (u32 i = 0; i != N; i++)\
			{\
				M_out.row[i] = op(M.row[i]);\
			}\
			return M_out;\
		}
		CA_XDECL_MATH_SCALAR_OPS_UNARY
		#undef X

		//-----------------------
		// component-wise binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(mat_t<T, N> const & A, mat_t<T, N> const & B)\
		{\
			mat_t<decltype(op(T(), T())), N> M;\
			for (u32 i = 0; i != N; i++)\
			{\
				M.row[i] = op(A.row[i], B.row[i]);\
			}\
			return M;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BINARY
		#undef X

		//------------------------
		// component-wise ternary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(mat_t<T, N> const & A, mat_t<T, N> const & B, mat_t<T, N> const & C)\
		{\
			mat_t<decltype(op(T(), T(), T())), N> M;\
			for (u32 i = 0; i != N; i++)\
			{\
				M.row[i] = op(A.row[i], B.row[i], C.row[i]);\
			}\
			return M;\
		}
		CA_XDECL_MATH_SCALAR_OPS_TERNARY
		#undef X

		//-------------------------------
		// component-wise built-in unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(mat_t<T, N> const & M)\
		{\
			mat_t<decltype(op T()), N> M_out;\
			for (u32 i = 0; i != N; i++)\
			{\
				M_out.row[i] = op M.row[i];\
			}\
			return M_out;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_UNARY
		#undef X

		//--------------------------------
		// component-wise built-in binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(mat_t<T, N> const & A, mat_t<T, N> const & B)\
		{\
			mat_t<decltype(T() op T()), N> M;\
			for (u32 i = 0; i != N; i++)\
			{\
				M.row[i] = A.row[i] op B.row[i];\
			}\
			return M;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(mat_t<T, N> const & A, T const & b)\
		{\
			mat_t<decltype(T() op T()), N> M;\
			for (u32 i = 0; i != N; i++)\
			{\
				M.row[i] = A.row[i] op b;\
			}\
			return M;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(T const & a, mat_t<T, N> const & B)\
		{\
			mat_t<decltype(T() op T()), N> M;\
			for (u32 i = 0; i != N; i++)\
			{\
				M.row[i] = a op B.row[i];\
			}\
			return M;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_BINARY
		#undef X

		//----------------------------------
		// component-wise built-in compound

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto& operator op(mat_t<T, N> & A, mat_t<T, N> const & B)\
		{\
			for (u32 i = 0; i != N; i++)\
			{\
				A.row[i] op B.row[i];\
			}\
			return A;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto& operator op(mat_t<T, N> & A, T const & b)\
		{\
			for (u32 i = 0; i != N; i++)\
			{\
				A.row[i] op b;\
			}\
			return A;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_COMPOUND
		#undef X
	}
}