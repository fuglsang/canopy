#include "ca/math_util.h"

namespace ca
{
	namespace math
	{
		//-------------------
		// library functions

		template <typename T, u32 N>
		inline T curvature_t0(beziercurve_t<vec_t<T, N>> const & curve)
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
		inline T curvature(beziercurve_t<vec_t<T, N>> const & curve, S t)
		{
			beziercurve_t<vec_t<T, N>> curve_t0;
			beziercurve_t<vec_t<T, N>> curve_t1;

			split(curve, t, curve_t0, &curve_t1);

			return curvature_t0(curve_t1);
		}

		template <typename T, typename S>
		inline void sample(beziercurve_t<T> const & curve, S t, T * x, T * v, T * a)
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
		inline void sample(beziercurve_t<T> const & curve, S t, T * x, T * v)
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
		inline void sample(beziercurve_t<T> const & curve, S t, T * x)
		{
			S tt = t * t;
			S ttt = t * tt;
			S tt3 = 3.0f * tt;

			S r = 1.0f - t;
			S rr = r * r;
			S rrr = rr * r;
			S rr3 = 3.0f * rr;

			*x = curve.p0 * (rrr) + curve.p1 * (rr3 * t) + curve.p2 * (tt3 * r) + curve.p3 * (ttt);
		}

		template <typename T, typename S>
		inline void sample(bezierpatch_t<T> const & patch, vec2_t<S> const & st, T * x)
		{
			beziercurve_t<T> curve_t;
			sample(patch.g0, st.y, &curve_t.p0);
			sample(patch.g1, st.y, &curve_t.p1);
			sample(patch.g2, st.y, &curve_t.p2);
			sample(patch.g3, st.y, &curve_t.p3);
			sample(curve_t, st.x, x);
		}

		template <typename T>
		inline void sample_lattice(bezierpatch_t<T> const & patch, T * points, uvec2_t const & point_count)
		{
			CA_ASSERT(point_count.x >= 2);
			CA_ASSERT(point_count.y >= 2);
			f32 step_s = rcp(static_cast<f32>(point_count.x - 1));
			f32 step_t = rcp(static_cast<f32>(point_count.y - 1));
			for (u32 i = 0; i != point_count.y; i++)
			{
				beziercurve_t<T> curve_t;
				sample(patch.g0, i * step_t, &curve_t.p0);
				sample(patch.g1, i * step_t, &curve_t.p1);
				sample(patch.g2, i * step_t, &curve_t.p2);
				sample(patch.g3, i * step_t, &curve_t.p3);

				points[point_count.x * i] = curve_t.p0;
				points[point_count.x * (i + 1) - 1] = curve_t.p3;
				for (u32 j = 1, n = point_count.x - 1; j != n; j++)
				{
					sample(curve_t, j * step_s, &points[point_count.x * i + j]);
				}
			}
		}

		template <typename T>
		inline void sample_linestrip(beziercurve_t<T> const & curve, T * points, u32 point_count)
		{
			CA_ASSERT(point_count >= 2);
			f32 step_t = rcp(static_cast<f32>(point_count - 1));
			points[0] = curve.p0;
			points[point_count - 1] = curve.p3;
			for (u32 i = 1, n = point_count - 1; i != n; i++)
			{
				sample(curve, i * step_t, &points[i]);
			}
		}

		template <typename T, typename S>
		inline void split(beziercurve_t<T> const & curve, S t, beziercurve_t<T> * curve_t0, beziercurve_t<T> * curve_t1)
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
		inline void split(bezierpatch_t<T> const & patch, vec2_t<S> const & st, bezierpatch_t<T> * patch_s0t0, bezierpatch_t<T> * patch_s0t1, bezierpatch_t<T> * patch_s1t0, bezierpatch_t<T> * patch_s1t1)
		{
			bezierpatch_t<T> patch_s0;
			bezierpatch_t<T> patch_s1;
			split_s(patch, st.x, &patch_s0, &patch_s1);
			split_t(patch_s0, st.y, patch_s0t0, patch_s0t1);
			split_t(patch_s1, st.y, patch_s1t0, patch_s1t1);
		}

		template <typename T, typename S>
		inline void split_s(bezierpatch_t<T> const & patch, S s, bezierpatch_t<T> * patch_s0, bezierpatch_t<T> * patch_s1)
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
				beziercurve_t<T> h;
				h.p0 = patch.g0.p[i];
				h.p1 = patch.g1.p[i];
				h.p2 = patch.g2.p[i];
				h.p3 = patch.g3.p[i];

				beziercurve_t<T> h0;
				beziercurve_t<T> h1;
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
		void split_t(bezierpatch_t<T> const & patch, S t, bezierpatch_t<T> * patch_t0, bezierpatch_t<T> * patch_t1)
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
		inline void subdivide(beziercurve_t<T> const & curve, beziercurve_t<T> * curves, u32 curve_count)
		{
			CA_ASSERT(curve_count >= 1);
			beziercurve_t<T> g[2];
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
	}
}