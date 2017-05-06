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
	return M.a11 * M.a22 - M.a12 * M.a21;
}

//TODO add explicit 3x3 det
//template <typename T>
//inline T determinant(mat_t<T, 3> const & M)
//{
//}

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
	CA_ASSERT_MSG(det_val != T(0), "matrix is singular and not invertible");
	M = rcp(det_val) * M_adjugate;
}

template <typename T, u32 N>
inline mat_t<T, N> invert_copy_of(mat_t<T, N> const & M)
{
	mat_t<T, N> copy = M;
	invert(copy);
	return copy;
}

//TODO
// make_affine_identity
// make_affine_translation
// make_affine_rotation
// make_affine_scaling
// make_lookat
// make_parallel_projection
// make_perspective_projection
