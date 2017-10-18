#pragma once

#include "ca/types.h"

namespace ca
{
	namespace input
	{
		struct key_t
		{
			u8 pressed : 1;
			u8 down : 2;
			u8 up : 2;
		};

		void key_clear(key_t * key);
		void key_clear(key_t * keys, u32 const key_count);
		void key_register_down(key_t * key);
		void key_register_up(key_t * key);
		void key_rollover(key_t * key);
		void key_rollover(key_t * keys, u32 const key_count);
	}
}