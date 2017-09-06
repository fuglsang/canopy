#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace math
	{
		template <typename T>
		struct ray_t
		{
			T origin;
			T direction;
		};

		typedef ray_t<fvec2_t> fray2_t;
		typedef ray_t<fvec3_t> fray3_t;
		typedef ray_t<fvec4_t> fray4_t;
	}
}