#include "ca/input_keystate.h"

#include <string.h>

namespace ca
{
	namespace input
	{
		void keystate_reset(keystate_t * keystate)
		{
			memset(keystate, 0, sizeof(keystate_t));
		}

		void keystate_register_key_down(keystate_t * keystate, keycode key)
		{
			if (keystate->key_pressed[key] == 0)
			{
				keystate->key_pressed[key] = 1;
				keystate->key_down[key] |= (1 << 1);
			}
		}

		void keystate_register_key_up(keystate_t * keystate, keycode key)
		{
			if (keystate->key_pressed[key] != 0)
			{
				keystate->key_pressed[key] = 0;
				keystate->key_up[key] |= (1 << 1);
			}
		}

		void keystate_rollover(keystate_t * keystate)
		{
			for (u32 i = 0; i != NUM_KEYS; i++)
			{
				keystate->key_down[i] >>= 1;
				keystate->key_up[i] >>= 1;
			}
		}
	}
}