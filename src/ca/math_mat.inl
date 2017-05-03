//--------------------------
// operator overloads below

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

CA_DEFINE_MATRIX_OP_SCALAR(*)
CA_DEFINE_MATRIX_OP_SCALAR(/)

CA_DEFINE_SCALAR_OP_MATRIX(*)
CA_DEFINE_SCALAR_OP_MATRIX(/)

#undef CA_DEFINE_MATRIX_OP_MATRIX
#undef CA_DEFINE_MATRIX_OP_SCALAR
#undef CA_DEFINE_SCALAR_OP_MATRIX

//-------------------------
// utility functions below

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
	//TODO
}

template <typename T, u32 N>
inline mat_t<T, N> invert_copy_of(mat_t<T, N> const & M)
{
	mat_t<T, N> copy = M;
	invert(copy);
	return copy;
}