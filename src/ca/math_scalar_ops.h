#pragma once

#include "ca_base.h"

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <typename T> T cos(T const & s);
		template <typename T> T sin(T const & s);
		template <typename T> T tan(T const & s);
		template <typename T> T acos(T const & s);
		template <typename T> T asin(T const & s);
		template <typename T> T atan(T const & s);
		template <typename T> T exp(T const & s);
		template <typename T> T log(T const & s);
		template <typename T> T sqrt(T const & s);
		template <typename T> T ceil(T const & s);
		template <typename T> T floor(T const & s);
		#define CA_XDECL_MATH_SCALAR_OPS_UNARY_EXTERN\
			X(cos)\
			X(sin)\
			X(tan)\
			X(acos)\
			X(asin)\
			X(atan)\
			X(exp)\
			X(log)\
			X(sqrt)\
			X(ceil)\
			X(floor)\

		template <typename T> CA_INLINE T abs(T const & s);
		template <typename T> CA_INLINE T frac(T const & s);
		template <typename T> CA_INLINE T rcp(T const & s);
		template <typename T> CA_INLINE T round(T const & s);
		template <typename T> CA_INLINE T rsqrt(T const & s);
		template <typename T> CA_INLINE T saturate(T const & s);
		#define CA_XDECL_MATH_SCALAR_OPS_UNARY_INLINE\
			X(abs)\
			X(frac)\
			X(rcp)\
			X(round)\
			X(rsqrt)\
			X(saturate)\

		#define CA_XDECL_MATH_SCALAR_OPS_UNARY\
			CA_XDECL_MATH_SCALAR_OPS_UNARY_EXTERN\
			CA_XDECL_MATH_SCALAR_OPS_UNARY_INLINE\

		//--------
		// binary

		template <typename T> T pow(T const & a, T const & b);
		#define CA_XDECL_MATH_SCALAR_OPS_BINARY_EXTERN\
			X(pow)\

		template <typename T> CA_INLINE T min(T const & a, T const & b);
		template <typename T> CA_INLINE T max(T const & a, T const & b);
		#define CA_XDECL_MATH_SCALAR_OPS_BINARY_INLINE\
			X(min)\
			X(max)\
		
		#define CA_XDECL_MATH_SCALAR_OPS_BINARY\
			CA_XDECL_MATH_SCALAR_OPS_BINARY_EXTERN\
			CA_XDECL_MATH_SCALAR_OPS_BINARY_INLINE\

		//---------
		// ternary

		#define CA_XDECL_MATH_SCALAR_OPS_TERNARY_EXTERN\

		template <typename T> CA_INLINE T clamp(T const & s, T const & a, T const & b);
		template <typename T> CA_INLINE T lerp(T const & a, T const & b, T const & s);
		#define CA_XDECL_MATH_SCALAR_OPS_TERNARY_INLINE\
			X(clamp)\
			X(lerp)\

		#define CA_XDECL_MATH_SCALAR_OPS_TERNARY\
			CA_XDECL_MATH_SCALAR_OPS_TERNARY_EXTERN\
			CA_XDECL_MATH_SCALAR_OPS_TERNARY_INLINE\

		// empty line consumes last backslash
	}
}

#include "ca/math_scalar_ops.inl"