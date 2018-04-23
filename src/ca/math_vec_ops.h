#pragma once

#include "ca/math_vec.h"
#include "ca/math_scalar_ops.h"

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <u32 N>
		CA_INLINE bool all(bvec_t<N> const & v);

		template <u32 N>
		CA_INLINE bool any(bvec_t<N> const & v);

		template <typename T, u32 N>
		CA_INLINE T component_min(vec_t<T, N> & v);

		template <typename T, u32 N>
		CA_INLINE T component_max(vec_t<T, N> & v);

		template <typename T, u32 N>
		CA_INLINE T component_sum(vec_t<T, N> & v);

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> homogenize(vec_t<T, N> const & v);

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> & homogenize_in_place(vec_t<T, N> & v);

		template <typename T, u32 N>
		CA_INLINE T norm(vec_t<T, N> const & v);

		template <typename T, u32 N>
		CA_INLINE T norm_sq(vec_t<T, N> const & v);

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> normalize(vec_t<T, N> const & v);

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> & normalize_in_place(vec_t<T, N> & v);

		template <typename T, u32 N>
		CA_INLINE T rcp_norm(vec_t<T, N> const & v);

		template <u32 I, u32 C, typename T, u32 N>
		CA_INLINE vec_t<T, C> & subvector(vec_t<T, N> & v);

		//--------
		// binary

		template <typename T>
		CA_INLINE T cross(vec2_t<T> const & a, vec2_t<T> const & b);

		template <typename T>
		CA_INLINE vec3_t<T> cross(vec3_t<T> const & a, vec3_t<T> const & b);

		template <typename T, u32 N>
		CA_INLINE T distance(vec_t<T, N> const & a, vec_t<T, N> const & b);

		template <typename T, u32 N>
		CA_INLINE T dot(vec_t<T, N> const & a, vec_t<T, N> const & b);

		//---------
		// ternary

		template <typename T, typename S>
		CA_INLINE T lerp(T const & a, T const & b, S const & s);

		//----------------
		// built-in unary

		#define CA_XDECL_MATH_VEC_OPS_BUILTIN_UNARY\
			X(-)\

		//-----------------
		// built-in binary

		#define CA_XDECL_MATH_VEC_OPS_BUILTIN_BINARY\
			X(*)\
			X(/)\
			X(+)\
			X(-)\
			X(<)\
			X(>)\
			X(<=)\
			X(>=)\
			X(==)\
			X(!=)\

		//-------------------
		// built-in compound

		#define CA_XDECL_MATH_VEC_OPS_BUILTIN_COMPOUND\
			X(*=)\
			X(/=)\
			X(+=)\
			X(-=)\

		//----------------------
		// component-wise unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & v);
		CA_XDECL_MATH_SCALAR_OPS_UNARY
		#undef X

		//-----------------------
		// component-wise binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & a, vec_t<T, N> const & b);
		CA_XDECL_MATH_SCALAR_OPS_BINARY
		#undef X

		//------------------------
		// component-wise ternary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & a, vec_t<T, N> const & b, vec_t<T, N> const & c);
		CA_XDECL_MATH_SCALAR_OPS_TERNARY
		#undef X
	}
}

#include "ca/math_vec_ops.inl"