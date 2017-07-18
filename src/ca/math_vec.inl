#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		//--------------------
		// operator overloads

		#define CA_DEFINE_VECTOR_OP_VECTOR(OP)											\
		template <typename T, u32 N>													\
		inline vec_t<T, N> operator OP (vec_t<T, N> const & a, vec_t<T, N> const & b)	\
		{																				\
			vec_t<T, N> v;																\
			for (u32 i = 0; i != N; i++)												\
			{																			\
				v.e[i] = a.e[i] OP b.e[i];												\
			}																			\
			return v;																	\
		}																				\
																						\
		template <typename T, u32 N>													\
		inline vec_t<T, N> & operator OP##= (vec_t<T, N> & a, vec_t<T, N> const & b)	\
		{																				\
			return (a = (a OP b));														\
		}

		#define CA_DEFINE_VECTOR_OP_SCALAR(OP)											\
		template <typename T, u32 N>													\
		inline vec_t<T, N> operator OP (vec_t<T, N> const & a, T const b)				\
		{																				\
			vec_t<T, N> v;																\
			for (u32 i = 0; i != N; i++)												\
			{																			\
				v.e[i] = a.e[i] OP b;													\
			}																			\
			return v;																	\
		}																				\
																						\
		template <typename T, u32 N>													\
		inline vec_t<T, N> & operator OP##= (vec_t<T, N> & a, T const b)				\
		{																				\
			return (a = (a OP b));														\
		}

		#define CA_DEFINE_SCALAR_OP_VECTOR(OP)											\
		template <typename T, u32 N>													\
		inline vec_t<T, N> operator OP (T const a, vec_t<T, N> const & b)				\
		{																				\
			vec_t<T, N> v;																\
			for (u32 i = 0; i != N; i++)												\
			{																			\
				v.e[i] = a OP b.e[i];													\
			}																			\
			return v;																	\
		}

		CA_DEFINE_VECTOR_OP_VECTOR(*)
		CA_DEFINE_VECTOR_OP_VECTOR(/)
		CA_DEFINE_VECTOR_OP_VECTOR(+)
		CA_DEFINE_VECTOR_OP_VECTOR(-)

		template <typename T, u32 N>
		inline vec_t<T, N> operator- (vec_t<T, N> const & a)
		{
			vec_t<T, N> v;
			for (u32 i = 0; i != N; i++)
			{
				v.e[i] = -a.e[i];
			}
			return v;
		}

		CA_DEFINE_VECTOR_OP_SCALAR(*)
		CA_DEFINE_VECTOR_OP_SCALAR(/)

		CA_DEFINE_SCALAR_OP_VECTOR(*)
		CA_DEFINE_SCALAR_OP_VECTOR(/)

		#undef CA_DEFINE_VECTOR_OP_VECTOR
		#undef CA_DEFINE_VECTOR_OP_SCALAR
		#undef CA_DEFINE_SCALAR_OP_VECTOR

		//-------------------
		// library functions

		template <typename T>
		inline T cross(vec2_t<T> const & a, vec2_t<T> const & b)
		{
			return (a.x * b.y - a.y * b.x);
		}

		template <typename T>
		inline vec3_t<T> cross(vec3_t<T> const & a, vec3_t<T> const & b)
		{
			return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		}

		template <typename T, u32 N>
		inline T dot(vec_t<T, N> const & a, vec_t<T, N> const & b)
		{
			T dot = T(0);
			for (u32 i = 0; i != N; i++)
			{
				dot += a.e[i] * b.e[i];
			}
			return dot;
		}

		template <typename T, u32 N>
		inline vec_t<T, N> homogenize(vec_t<T, N> const & v)
		{
			return (v / v.e[N - 1]);
		}

		template <typename T, u32 N>
		inline vec_t<T, N> & homogenize_in_place(vec_t<T, N> & v)
		{
			return (v /= v.e[N - 1]);
		}

		template <typename T, u32 N>
		inline T norm(vec_t<T, N> const & v)
		{
			return sqrt(dot(v, v));
		}

		template <typename T, u32 N>
		inline T norm_sq(vec_t<T, N> const & v)
		{
			return dot(v, v);
		}

		template <typename T, u32 N>
		inline vec_t<T, N> normalize(vec_t<T, N> const & v)
		{
			return (v * rcp_norm(v));
		}

		template <typename T, u32 N>
		inline vec_t<T, N> & normalize_in_place(vec_t<T, N> & v)
		{
			return (v *= rcp_norm(v));
		}

		template <typename T, u32 N>
		inline T rcp_norm(vec_t<T, N> const & v)
		{
			return rcp_sqrt(dot(v, v));
		}

		template <u32 I, u32 C, typename T, u32 N>
		inline vec_t<T, C> & subvector(vec_t<T, N> & v)
		{
			static_assert(I + C <= N, "out of bounds");
			return *reinterpret_cast<vec_t<T, C> *>(v.e + I);
		}
	}
}