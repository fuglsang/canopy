#include "ca/input_cursor.h"

namespace ca
{
	namespace input
	{
		void cursor_clear(cursor_t * cursor)
		{
			cursor->tracking = false;
			cursor->position = { -1, -1 };
		}

		void cursor_register_position(cursor_t * cursor, math::ivec2_t const & position)
		{
			cursor->position = position;
		}
	}
}