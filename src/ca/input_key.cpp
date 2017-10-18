#include "ca/core_log.h"
#include "ca/input_key.h"

namespace ca
{
	namespace input
	{
		void key_clear(key_t * key)
		{
			key->pressed = 0;
			key->down = 0;
			key->up = 0;
		}

		void key_clear(key_t * keys, u32 const key_count)
		{
			for (u32 i = 0; i != key_count; i++)
			{
				key_clear(keys + i);
			}
		}

		void key_register_down(key_t * key)
		{
			if (key->pressed == 0)
			{
				key->pressed = 1;
				key->down |= 2;
			}
		}

		void key_register_up(key_t * key)
		{
			if (key->pressed != 0)
			{
				key->pressed = 0;
				key->up |= 2;
			}
		}

		void key_rollover(key_t * key)
		{
			key->down >>= 1;
			key->up >>= 1;
		}

		void key_rollover(key_t * keys, u32 const key_count)
		{
			for (u32 i = 0; i != key_count; i++)
			{
				key_rollover(keys + i);
			}
		}
	}
}