#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		template <typename T>
		struct cubic_t
		{
			//
			//  .---t
			//  |
			//  :    p0--p1--p2--p3
			//
			union
			{
				T p[4];
				struct { T p0, p1, p2, p3; };
			};
		};

		typedef cubic_t<fvec2_t> fcubic2_t;
		typedef cubic_t<fvec3_t> fcubic3_t;
		typedef cubic_t<fvec4_t> fcubic4_t;

		template <typename T>
		struct bicubic_t
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
				T p[16];
				struct
				{
					T p00, p01, p02, p03;
					T p10, p11, p12, p13;
					T p20, p21, p22, p23;
					T p30, p31, p32, p33;
				};
				cubic_t<T> g[4];
				struct { cubic_t<T> g0, g1, g2, g3; };
			};
		};

		typedef bicubic_t<fvec2_t> fbicubic2_t;
		typedef bicubic_t<fvec3_t> fbicubic3_t;
		typedef bicubic_t<fvec4_t> fbicubic4_t;
	}
}