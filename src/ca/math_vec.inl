//--------------------------
// operator overloads below

#define CA_DEFINE_VECTOR_OP_VECTOR(OP)											\
template <typename T, u32 N>													\
inline vec_t<T, N> operator OP (vec_t<T, N> const & a, vec_t<T, N> const & b)	\
{																				\
	vec_t<T, N> v = a;															\
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

CA_DEFINE_VECTOR_OP_VECTOR(+)
CA_DEFINE_VECTOR_OP_VECTOR(-)
CA_DEFINE_VECTOR_OP_VECTOR(*)
CA_DEFINE_VECTOR_OP_VECTOR(/)

CA_DEFINE_VECTOR_OP_SCALAR(*)
CA_DEFINE_VECTOR_OP_SCALAR(/)

CA_DEFINE_SCALAR_OP_VECTOR(*)
CA_DEFINE_SCALAR_OP_VECTOR(/)

#undef CA_DEFINE_VECTOR_OP_VECTOR
#undef CA_DEFINE_VECTOR_OP_SCALAR
#undef CA_DEFINE_SCALAR_OP_VECTOR

//-------------------------
// utility functions below

template <typename T>
inline T cross(vec_t<T, 2> const & a, vec_t<T, 2> const & b)
{
	return (a.x * b.y - a.y * b.x);
}

template <typename T>
inline vec_t<T, 3> cross(vec_t<T, 3> const & a, vec_t<T, 3> const & b)
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
inline T norm(vec_t<T, N> const & v)
{
	return sqrt(dot(v, v));
}