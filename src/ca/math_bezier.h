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

		template <typename T, u32 N>
		struct bezier_t
		{
			union
			{
				vec_t<T, N> p[4];
				struct { vec_t<T, N> p0, p1, p2, p3; };
			};
		};

		typedef bezier_t<f32, 2> bezier2_t;
		typedef bezier_t<f32, 3> bezier3_t;
		typedef bezier_t<f32, 4> bezier4_t;

		#include "ca/math_bezier.inl"
	}
}