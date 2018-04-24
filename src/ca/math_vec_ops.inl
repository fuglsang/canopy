#pragma once

namespace ca
{
	namespace math
	{
		//-------
		// unary

		template <u32 N>
		CA_INLINE bool all(bvec_t<N> const & v)
		{
			bool b = true;
			for (u32 i = 0; i != N; i++)
			{
				b &= v.e[i];
			}
			return b;
		}

		template <u32 N>
		CA_INLINE bool any(bvec_t<N> const & v)
		{
			bool b = false;
			for (u32 i = 0; i != N; i++)
			{
				b |= v.e[i];
			}
			return b;
		}

		template <typename T, u32 N>
		CA_INLINE T component_min(vec_t<T, N> & v)
		{
			T s = v.e[0];
			for (u32 i = 1; i != N; i++)
			{
				s = min(s, v.e[i]);
			}
			return s;
		}

		template <typename T, u32 N>
		CA_INLINE T component_max(vec_t<T, N> & v)
		{
			T s = v.e[0];
			for (u32 i = 1; i != N; i++)
			{
				s = max(s, v.e[i]);
			}
			return s;
		}

		template <typename T, u32 N>
		CA_INLINE T component_sum(vec_t<T, N> & v)
		{
			T s = 0;
			for (u32 i = 0; i != N; i++)
			{
				s += v.e[i];
			}
			return s;
		}

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> homogenize(vec_t<T, N> const & v)
		{
			return (v / v.e[N - 1]);
		}

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> & homogenize_in_place(vec_t<T, N> & v)
		{
			return (v /= v.e[N - 1]);
		}

		template <typename T, u32 N>
		CA_INLINE T norm(vec_t<T, N> const & v)
		{
			return sqrt(dot(v, v));
		}

		template <typename T, u32 N>
		CA_INLINE T norm_sq(vec_t<T, N> const & v)
		{
			return dot(v, v);
		}

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> normalize(vec_t<T, N> const & v)
		{
			return (v * rcp_norm(v));
		}

		template <typename T, u32 N>
		CA_INLINE vec_t<T, N> & normalize_in_place(vec_t<T, N> & v)
		{
			return (v *= rcp_norm(v));
		}

		template <typename T, u32 N>
		CA_INLINE T rcp_norm(vec_t<T, N> const & v)
		{
			return rsqrt(dot(v, v));
		}

		template <u32 I, u32 C, typename T, u32 N>
		CA_INLINE vec_t<T, C> & subvector(vec_t<T, N> & v)
		{
			static_assert(I + C <= N, "out of bounds");
			return *reinterpret_cast<vec_t<T, C> *>(v.e + I);
		}

		//--------
		// binary

		template <typename T>
		CA_INLINE T cross(vec2_t<T> const & a, vec2_t<T> const & b)
		{
			return (a.x * b.y - a.y * b.x);
		}

		template <typename T>
		CA_INLINE vec3_t<T> cross(vec3_t<T> const & a, vec3_t<T> const & b)
		{
			return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		}

		template <typename T, u32 N>
		CA_INLINE T distance(vec_t<T, N> const & a, vec_t<T, N> const & b)
		{
			return norm(a - b);
		}

		template <typename T, u32 N>
		CA_INLINE T dot(vec_t<T, N> const & a, vec_t<T, N> const & b)
		{
			T dot = 0;
			for (u32 i = 0; i != N; i++)
			{
				dot += a.e[i] * b.e[i];
			}
			return dot;
		}

		//---------
		// ternary

		template <typename T, typename S>
		CA_INLINE T lerp(T const & a, T const & b, S const & s)
		{
			return (1 - s) * a + s * b;
		}

		//----------------------
		// component-wise unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & v)\
		{\
			vec_t<decltype(op(T())), N> result;\
			for (u32 i = 0; i != N; i++)\
			{\
				result.e[i] = op(v.e[i]);\
			}\
			return result;\
		}
		CA_XDECL_MATH_SCALAR_OPS_UNARY
		#undef X

		//-----------------------
		// component-wise binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & a, vec_t<T, N> const & b)\
		{\
			vec_t<decltype(op(T(), T())), N> result;\
			for (u32 i = 0; i != N; i++)\
			{\
				result.e[i] = op(a.e[i], b.e[i]);\
			}\
			return result;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BINARY
		#undef X

		//------------------------
		// component-wise ternary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto op(vec_t<T, N> const & a, vec_t<T, N> const & b, vec_t<T, N> const & c)\
		{\
			vec_t<decltype(op(T(), T(), T())), N> result;\
			for (u32 i = 0; i != N; i++)\
			{\
				result.e[i] = op(a.e[i], b.e[i], c.e[i]);\
			}\
			return result;\
		}
		CA_XDECL_MATH_SCALAR_OPS_TERNARY
		#undef X

		//-------------------------------
		// component-wise built-in unary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(vec_t<T, N> const & v)\
		{\
			vec_t<decltype(op T()), N> v_out;\
			for (u32 i = 0; i != N; i++)\
			{\
				v_out.e[i] = op v.e[i];\
			}\
			return v_out;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_UNARY
		#undef X

		//--------------------------------
		// component-wise built-in binary

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(vec_t<T, N> const & a, vec_t<T, N> const & b)\
		{\
			vec_t<decltype(T() op T()), N> v;\
			for (u32 i = 0; i != N; i++)\
			{\
				v.e[i] = a.e[i] op b.e[i];\
			}\
			return v;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(vec_t<T, N> const & a, T const & b)\
		{\
			vec_t<decltype(T() op T()), N> v;\
			for (u32 i = 0; i != N; i++)\
			{\
				v.e[i] = a.e[i] op b;\
			}\
			return v;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto operator op(T const & a, vec_t<T, N> const & b)\
		{\
			vec_t<decltype(T() op T()), N> v;\
			for (u32 i = 0; i != N; i++)\
			{\
				v.e[i] = a op b.e[i];\
			}\
			return v;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_BINARY
		#undef X

		//----------------------------------
		// component-wise built-in compound

		#define X(op)\
		template <typename T, u32 N>\
		CA_INLINE auto& operator op(vec_t<T, N> & a, vec_t<T, N> const & b)\
		{\
			for (u32 i = 0; i != N; i++)\
			{\
				a.e[i] op b.e[i];\
			}\
			return a;\
		}\
		template <typename T, u32 N>\
		CA_INLINE auto& operator op(vec_t<T, N> & a, T const & b)\
		{\
			for (u32 i = 0; i != N; i++)\
			{\
				a.e[i] op b;\
			}\
			return a;\
		}
		CA_XDECL_MATH_SCALAR_OPS_BUILTIN_COMPOUND
		#undef X
	}
}