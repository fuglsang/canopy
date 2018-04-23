#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		//-------------------
		// library functions

		template <typename T, u32 N>
		CA_INLINE T curvature_at_p0(bezier_t<vec_t<T, N>> const & curve)
		{
			// http://cagd.cs.byu.edu/~557/text/ch2.pdf
			vec_t<T, N> v1 = curve.p1 - curve.p0;
			vec_t<T, N> v2 = curve.p2 - curve.p0;

			T a = norm(v1);
			vec_t<T, N> u = v1 / a;
			vec_t<T, N> q = p0 + u * dot(u, v2);

			T h = norm(p2 - q);
			T k = ((N - 1.0f) / N) * (h / (a * a));
			return k;
		}

		template <typename T, u32 N, typename S>
		CA_INLINE T curvature(bezier_t<vec_t<T, N>> const & curve, S t)
		{
			bezier_t<vec_t<T, N>> curve_t0;
			bezier_t<vec_t<T, N>> curve_t1;

			split(curve, t, curve_t0, &curve_t1);

			return curvature_at_p0(curve_t1);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezier_t<T> const & curve, S t, T * x, T * v, T * a)
		{
			S tt = t * t;
			S ttt = t * tt;
			S tt3 = 3.0f * tt;

			S r = 1.0f - t;
			S rr = r * r;
			S rrr = rr * r;
			S rr3 = 3.0f * rr;

			*x = curve.p0 * (rrr) + curve.p1 * (rr3 * t) + curve.p2 * (tt3 * r) + curve.p3 * (ttt);
			*v = curve.p0 * (-rr3) + curve.p1 * (rr3 - 6.0f * t * r) + curve.p2 * (6.0f * t * r - tt3) + curve.p3 * (tt3);
			*a = curve.p0 * (-6.0f * r) + curve.p1 * (6.0f * (3.0f * t - 2.0f)) + curve.p2 * (-6.0f * (3.0f * t - 1.0f)) + curve.p3 * (6.0f * t);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezier_t<T> const & curve, S t, T * x, T * v)
		{
			S tt = t * t;
			S ttt = t * tt;
			S tt3 = 3.0f * tt;

			S r = 1.0f - t;
			S rr = r * r;
			S rrr = rr * r;
			S rr3 = 3.0f * rr;

			*x = curve.p0 * (rrr) + curve.p1 * (rr3 * t) + curve.p2 * (tt3 * r) + curve.p3 * (ttt);
			*v = curve.p0 * (-rr3) + curve.p1 * (rr3 - 6.0f * t * r) + curve.p2 * (6.0f * t * r - tt3) + curve.p3 * (tt3);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezier_t<T> const & curve, S t, T * x)
		{
			S tt = t * t;
			S ttt = t * tt;
			S tt3 = 3 * tt;

			S r = 1.0f - t;
			S rr = r * r;
			S rrr = rr * r;
			S rr3 = 3.0f * rr;

			*x = curve.p0 * (rrr) + curve.p1 * (rr3 * t) + curve.p2 * (tt3 * r) + curve.p3 * (ttt);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezierpatch_t<T> const & patch, vec2_t<S> const & st, T * x)
		{
			bezier_t<T> curve_t;
			eval(patch.g0, st.y, &curve_t.p0);
			eval(patch.g1, st.y, &curve_t.p1);
			eval(patch.g2, st.y, &curve_t.p2);
			eval(patch.g3, st.y, &curve_t.p3);
			eval(curve_t, st.x, x);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezierpatch_t<T> const & patch, vec2_t<S> const & st, T * x, T * vs, T * vt)
		{
			bezier_t<T> curve_s;
			eval({ patch.p00, patch.p10, patch.p20, patch.p30 }, st.x, &curve_s.p0);
			eval({ patch.p01, patch.p11, patch.p21, patch.p31 }, st.x, &curve_s.p1);
			eval({ patch.p02, patch.p12, patch.p22, patch.p32 }, st.x, &curve_s.p2);
			eval({ patch.p03, patch.p13, patch.p23, patch.p33 }, st.x, &curve_s.p3);

			bezier_t<T> curve_t;
			eval(patch.g0, st.y, &curve_t.p0);
			eval(patch.g1, st.y, &curve_t.p1);
			eval(patch.g2, st.y, &curve_t.p2);
			eval(patch.g3, st.y, &curve_t.p3);

			eval(curve_s, st.y, x, vt);
			eval(curve_t, st.x, x, vs);
		}

		template <typename T, typename S>
		CA_INLINE void eval(bezierpatch_t<T> const & patch, vec2_t<S> const & st, T * x, T * n)
		{
			T vs;
			T vt;

			eval(patch, st, x, &vs, &vt);
			
			*n = normalize(cross(vs, vt));
		}

		template <typename T>
		CA_INLINE void sample(bezier_t<T> const & curve, T * points, u32 point_count)
		{
			CA_ASSERT(point_count >= 2);
			f32 step_t = rcp(static_cast<f32>(point_count - 1));
			points[0] = curve.p0;
			points[point_count - 1] = curve.p3;
			for (u32 i = 1, n = point_count - 1; i != n; i++)
			{
				eval(curve, i * step_t, &points[i]);
			}
		}

		template <typename T>
		CA_INLINE void sample(bezierpatch_t<T> const & patch, T * points, uvec2_t const & point_count_st)
		{
			CA_ASSERT(point_count_st.x >= 2);
			CA_ASSERT(point_count_st.y >= 2);
			f32 step_s = rcp(static_cast<f32>(point_count_st.x - 1));
			f32 step_t = rcp(static_cast<f32>(point_count_st.y - 1));
			for (u32 i = 0; i != point_count_st.y; i++)
			{
				bezier_t<T> curve_t;
				eval(patch.g0, i * step_t, &curve_t.p0);
				eval(patch.g1, i * step_t, &curve_t.p1);
				eval(patch.g2, i * step_t, &curve_t.p2);
				eval(patch.g3, i * step_t, &curve_t.p3);

				points[point_count_st.x * i] = curve_t.p0;
				points[point_count_st.x * (i + 1) - 1] = curve_t.p3;
				for (u32 j = 1, n = point_count_st.x - 1; j != n; j++)
				{
					eval(curve_t, j * step_s, &points[point_count_st.x * i + j]);
				}
			}
		}

		template <typename T, typename S>
		CA_INLINE void split(bezier_t<T> const & curve, S t, bezier_t<T> * curve_t0, bezier_t<T> * curve_t1)
		{
			T a = lerp(curve.p0, curve.p1, t);
			T b = lerp(curve.p1, curve.p2, t);
			T c = lerp(curve.p2, curve.p3, t);
			T d = lerp(a, b, t);
			T e = lerp(b, c, t);
			T p = lerp(d, e, t);

			curve_t0->p0 = curve.p0;
			curve_t0->p1 = a;
			curve_t0->p2 = d;
			curve_t0->p3 = p;

			curve_t1->p0 = p;
			curve_t1->p1 = e;
			curve_t1->p2 = c;
			curve_t1->p3 = curve.p3;
		}

		template <typename T, typename S>
		CA_INLINE void split(bezierpatch_t<T> const & patch, vec2_t<S> const & st, bezierpatch_t<T> * patch_s0t0, bezierpatch_t<T> * patch_s0t1, bezierpatch_t<T> * patch_s1t0, bezierpatch_t<T> * patch_s1t1)
		{
			bezierpatch_t<T> patch_s0;
			bezierpatch_t<T> patch_s1;
			split_s(patch, st.x, &patch_s0, &patch_s1);
			split_t(patch_s0, st.y, patch_s0t0, patch_s0t1);
			split_t(patch_s1, st.y, patch_s1t0, patch_s1t1);
		}

		template <typename T, typename S>
		CA_INLINE void split_s(bezierpatch_t<T> const & patch, S s, bezierpatch_t<T> * patch_s0, bezierpatch_t<T> * patch_s1)
		{
			//
			//  .---t
			//  |
			//  s    p00--p01--p02--p03   g0
			//        |    |    |    |
			//       p10--p11--p12--p13   g1
			//        |    |    |    |
			//       p20--p21--p22--p23   g2
			//        |    |    |    |
			//       p30--p31--p32--p33   g3
			//
			//        h    h    h    h
			//
			for (u32 i = 0; i != 4; i++)
			{
				bezier_t<T> h;
				h.p0 = patch.g0.p[i];
				h.p1 = patch.g1.p[i];
				h.p2 = patch.g2.p[i];
				h.p3 = patch.g3.p[i];

				bezier_t<T> h0;
				bezier_t<T> h1;
				split(h, s, &h0, &h1);

				patch_s0->g0.p[i] = h0.p0;
				patch_s0->g1.p[i] = h0.p1;
				patch_s0->g2.p[i] = h0.p2;
				patch_s0->g3.p[i] = h0.p3;

				patch_s1->g0.p[i] = h1.p0;
				patch_s1->g1.p[i] = h1.p1;
				patch_s1->g2.p[i] = h1.p2;
				patch_s1->g3.p[i] = h1.p3;
			}
		}

		template <typename T, typename S>
		CA_INLINE void split_t(bezierpatch_t<T> const & patch, S t, bezierpatch_t<T> * patch_t0, bezierpatch_t<T> * patch_t1)
		{
			//
			//  .---t
			//  |
			//  s    p00--p01--p02--p03   g0
			//        |    |    |    |
			//       p10--p11--p12--p13   g1
			//        |    |    |    |
			//       p20--p21--p22--p23   g2
			//        |    |    |    |
			//       p30--p31--p32--p33   g3
			//
			for (u32 i = 0; i != 4; i++)
			{
				split(patch.g[i], t, &patch_t0->g[i], &patch_t1->g[i]);
			}
		}

		template <typename T>
		CA_INLINE void subdivide(bezier_t<T> const & curve, bezier_t<T> * curves, u32 curve_count)
		{
			CA_ASSERT(curve_count >= 1);
			bezier_t<T> g[2];
			g[0] = curve;
			
			for (u32 i = 0, n = curve_count - 1; i != n; i++)
			{
				split(g[i & 1], rcp(static_cast<f32>(curve_count - i)), &curves[i], &g[(i + 1) & 1]);
			}

			curves[curve_count - 1] = g[(curve_count - 1) & 1];

			//TODO consider this
			//bezier_t<T> remainder = curve;
			//for (u32 i = 0, n = segment_count - 1; i != n; i++)
			//{
			//	bezier_split_left_pivot_right(remainder, rcp(static_cast<f32>(segment_count - i)), &segments[i]);
			//}
			//segments[segment_count - 1] = remainder;
		}

		template <typename T>
		CA_INLINE void subdivide(bezierpatch_t<T> const & patch, bezierpatch_t<T> * patches, uvec2_t const & patch_count)
		{
			CA_FATAL("TODO");
		}
	}
}