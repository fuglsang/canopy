template <typename T, u32 N>
void bezier_eval(bezier_t<T, N> const * bezier, f32 t, vec_t<T, N> * x, vec_t<T, N> * v, vec_t<T, N> * a)
{
	T tt = t * t;
	T ttt = t * tt;
	T tt3 = 3.0f * tt;

	T r = 1.0f - t;
	T rr = r * r;
	T rrr = rr * r;
	T rr3 = 3.0f * rr;

	*x = bezier->p0 * (rrr)+bezier->p1 * (rr3 * t) + bezier->p2 * (tt3 * r) + bezier->p3 * (ttt);
	*v = bezier->p0 * (-rr3) + bezier->p1 * (rr3 - 6.0f * t * r) + bezier->p2 * (6.0f * t * r - tt3) + bezier->p3 * (tt3);
	*a = bezier->p0 * (-6.0f * r) + bezier->p1 * (6.0f * (3.0f * t - 2.0f)) + bezier->p2 * (-6.0f * (3.0f * t - 1.0f)) + bezier->p3 * (6.0f * t);
}

template <typename T, u32 N>
void bezier_eval(bezier_t<T, N> const * bezier, f32 t, vec_t<T, N> * x, vec_t<T, N> * v)
{
	T tt = t * t;
	T ttt = t * tt;
	T tt3 = 3.0f * tt;

	T r = 1.0f - t;
	T rr = r * r;
	T rrr = rr * r;
	T rr3 = 3.0f * rr;

	*x = bezier->p0 * (rrr)+bezier->p1 * (rr3 * t) + bezier->p2 * (tt3 * r) + bezier->p3 * (ttt);
	*v = bezier->p0 * (-rr3) + bezier->p1 * (rr3 - 6.0f * t * r) + bezier->p2 * (6.0f * t * r - tt3) + bezier->p3 * (tt3);
}

template <typename T, u32 N>
void bezier_eval(bezier_t<T, N> const * bezier, f32 t, vec_t<T, N> * x)
{
	T tt = t * t;
	T ttt = t * tt;
	T tt3 = 3.0f * tt;

	T r = 1.0f - t;
	T rr = r * r;
	T rrr = rr * r;
	T rr3 = 3.0f * rr;

	*x = bezier->p0 * (rrr)+bezier->p1 * (rr3 * t) + bezier->p2 * (tt3 * r) + bezier->p3 * (ttt);
}

template <typename T, u32 N>
void bezier_split(bezier_t<T, N> const * bezier, f32 t, bezier_t<T, N> * split_left, bezier_t<T, N> * split_right)
{
	vec_t<T, N> a = lerp(bezier->p0, bezier->p1, t);
	vec_t<T, N> b = lerp(bezier->p1, bezier->p2, t);
	vec_t<T, N> c = lerp(bezier->p2, bezier->p3, t);
	vec_t<T, N> d = lerp(a, b, t);
	vec_t<T, N> e = lerp(b, c, t);
	vec_t<T, N> p = lerp(d, e, t);

	split_left->p0 = bezier->p0;
	split_left->p1 = a;
	split_left->p2 = d;
	split_left->p3 = p;

	split_right->p0 = p;
	split_right->p1 = e;
	split_right->p2 = c;
	split_right->p3 = bezier->p3;
}

template <typename T, u32 N>
T bezier_curvature_p0(bezier_t<T, N> const * bezier)
{
	// http://cagd.cs.byu.edu/~557/text/ch2.pdf
	vec_t<T, N> v1 = bezier->p1 - bezier->p0;
	vec_t<T, N> v2 = bezier->p2 - bezier->p0;

	T a = norm(v1);
	vec_t<T, N> u = v1 / a;
	vec_t<T, N> q = p0 + u * dot(u, v2);

	T h = norm(p2 - q);
	T k = ((N - 1.0f) / N) * (h / (a * a));
	return k;
}

template <typename T, u32 N>
T bezier_curvature(bezier_t<T, N> * const bezier, f32 t)
{
	bezier_t<T, N> split_left;
	bezier_t<T, N> split_right;

	bezier_split(bezier, t, &split_left, &split_right);

	return bezier_curvature_p0(&split_right);
}