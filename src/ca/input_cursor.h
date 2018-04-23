#pragma once

#include "ca/math_vec.h"

namespace ca
{
	namespace input
	{
		struct cursor_t
		{
			bool tracking;
			math::ivec2_t position;
		};

		void cursor_clear(cursor_t * cursor);
		void cursor_register_position(cursor_t * cursor, math::ivec2_t const & position);
	}
}