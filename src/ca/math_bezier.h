#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		/* cubic bezier

		C(t) =	p0 (1-t)^3 +
				p1 (t 3 (1-t)^2) +
				p2 (3 t^2 (1-t)) +
				p3 (t^3)

		C'(t) =	p0 (-3 (1-t)^2) +
				p1 (3 (1-t)^2 - 6 t (1-t)) +
				p2 (6 t (1-t) - 3 t^2) +
				p3 (3 t^2)

		C''(t)= p0 (-6 (t-1)) +
				p1 (6 (3 t - 2)) +
				p2 (-6 (3 t - 1)) +
				p3 (6 t)
		
		*/

		template <typename T>
		struct beziercurve_t
		{
			union
			{
				T p[4];
				struct { T p0, p1, p2, p3; };
			};
		};

		template <typename T>
		struct bezierpatch_t
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
			union
			{
				beziercurve_t<T> g[4];
				struct { beziercurve_t<T> g0, g1, g2, g3; };
			};
		};

		typedef beziercurve_t<fvec2_t> fbeziercurve2_t;
		typedef beziercurve_t<fvec3_t> fbeziercurve3_t;
		typedef beziercurve_t<fvec4_t> fbeziercurve4_t;

		typedef bezierpatch_t<fvec2_t> fbezierpatch2_t;
		typedef bezierpatch_t<fvec3_t> fbezierpatch3_t;
		typedef bezierpatch_t<fvec4_t> fbezierpatch4_t;

		//-------------------
		// library functions

		template <typename T, u32 N>
		inline T curvature_t0(beziercurve_t<vec_t<T, N>> const & curve);

		template <typename T, u32 N, typename S>
		inline T curvature(beziercurve_t<vec_t<T, N>> const & curve, S t);

		template <typename T, typename S>
		inline void sample(beziercurve_t<T> const & curve, S t, T * x, T * v, T * a);

		template <typename T, typename S>
		inline void sample(beziercurve_t<T> const & curve, S t, T * x, T * v);

		template <typename T, typename S>
		inline void sample(beziercurve_t<T> const & curve, S t, T * x);

		template <typename T, typename S>
		inline void sample(bezierpatch_t<T> const & patch, vec2_t<S> const & st, T * x);

		template <typename T>
		inline void sample_lattice(bezierpatch_t<T> const & patch, T * points, uvec2_t const & point_count);

		template <typename T>
		inline void sample_linestrip(beziercurve_t<T> const & curve, T * points, u32 point_count);

		template <typename T, typename S>
		inline void split(beziercurve_t<T> const & curve, S t, beziercurve_t<T> * curve_t0, beziercurve_t<T> * curve_t1);

		template <typename T, typename S>
		inline void split(bezierpatch_t<T> const & patch, vec2_t<S> const & st, bezierpatch_t<T> * patch_s0t0, bezierpatch_t<T> * patch_s0t1, bezierpatch_t<T> * patch_s1t0, bezierpatch_t<T> * patch_s1t1);

		template <typename T, typename S>
		inline void split_s(bezierpatch_t<T> const & patch, S s, bezierpatch_t<T> * patch_s0, bezierpatch_t<T> * patch_s1);

		template <typename T, typename S>
		inline void split_t(bezierpatch_t<T> const & patch, S t, bezierpatch_t<T> * patch_t0, bezierpatch_t<T> * patch_t1);

		template <typename T>
		inline void subdivide(beziercurve_t<T> const & curve, beziercurve_t<T> * curves, u32 curve_count);
	}
}

#include "ca/math_bezier.inl"