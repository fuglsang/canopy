#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace input
	{
		struct pointer_t
		{
			bool tracking;
			math::ivec2_t position;
		};

		void pointer_clear(pointer_t * pointer);
	}
}