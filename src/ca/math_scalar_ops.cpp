#include "ca_base.h"
#include "ca/math_scalar_ops.h"

#include <cmath>

namespace ca
{
	namespace math
	{
		#define X(op)\
		template <> f32 op<f32> (f32 const & s) { return std::op(s); }\
		template <> f64 op<f64> (f64 const & s) { return std::op(s); }
		CA_XDECL_MATH_SCALAR_OPS_UNARY_EXTERN
		#undef X

		#define X(op)\
		template <> f32 op<f32> (f32 const & a, f32 const & b) { return std::op(a, b); }\
		template <> f64 op<f64> (f64 const & a, f64 const & b) { return std::op(a, b); }
		CA_XDECL_MATH_SCALAR_OPS_BINARY_EXTERN
		#undef X
		
		#define X(op)\
		template <> f32 op<f32> (f32 const & a, f32 const & b, f32 const & c) { return std::op(a, b, c); }\
		template <> f64 op<f64> (f64 const & a, f64 const & b, f64 const & c) { return std::op(a, b, c); }
		CA_XDECL_MATH_SCALAR_OPS_TERNARY_EXTERN
		#undef X
		
		static void __test_scalar_ops()
		{
			f32 const a = 1;
			f32 const b = 1;
			f32 const c = 1;

			#define X(op) op(a);
			CA_XDECL_MATH_SCALAR_OPS_UNARY_EXTERN
			#undef X
			
			#define X(op) op(a, b);
			CA_XDECL_MATH_SCALAR_OPS_BINARY_EXTERN
			#undef X
			
			#define X(op) op(a, b, c);
			CA_XDECL_MATH_SCALAR_OPS_TERNARY_EXTERN
			#undef X
		}
	}
}