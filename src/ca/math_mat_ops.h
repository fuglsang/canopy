#pragma once

#include "ca/math_mat.h"
#include "ca/math_vec_ops.h"

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <typename T>
		CA_INLINE T determinant(mat2_t<T> const & M);

		template <typename T>
		CA_INLINE T determinant(mat3_t<T> const & M);

		template <typename T, u32 N>
		CA_INLINE T determinant(mat_t<T, N> const & M);

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> inverse(mat_t<T, N> const & M);

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> transpose(mat_t<T, N> const & M);

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> & transpose_in_place(mat_t<T, N> & M);

		template <typename T, u32 N>//TODO constexpr
		CA_INLINE mat_t<T, N - 1> submatrix(mat_t<T, N> const & M, u32 remove_i, u32 remove_j);

		//--------
		// binary

		template <typename T, u32 N>
		CA_INLINE mat_t<T, N> mul(mat_t<T, N> const & A, mat_t<T, N> const & B);

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> mul(mat_t<T, N> const & A, vec_t<T, N> const & b);

		//----------------
		// built-in unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(mat_t<T, N> const & M);
		CA_XDECL_MATH_VEC_OPS_BUILTIN_UNARY
		#undef X

		//-----------------
		// built-in binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(mat_t<T, N> const & A, mat_t<T, N> const & B);
		CA_XDECL_MATH_VEC_OPS_BUILTIN_BINARY
		#undef X

		//-------------------
		// built-in compound

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto& operator op(mat_t<T, N> & A, mat_t<T, N> const & B);
		CA_XDECL_MATH_VEC_OPS_BUILTIN_COMPOUND
		#undef X
	}
}

#include "ca/math_mat_ops.inl"