#pragma once

#include "ca/input_key.h"

namespace ca
{
	namespace input
	{
		enum keycode
		{
			// 0--9
			KEY_0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,

			// a--z
			KEY_A,
			KEY_B,
			KEY_C,
			KEY_D,
			KEY_E,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_I,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_M,
			KEY_N,
			KEY_O,
			KEY_P,
			KEY_Q,
			KEY_R,
			KEY_S,
			KEY_T,
			KEY_U,
			KEY_V,
			KEY_W,
			KEY_X,
			KEY_Y,
			KEY_Z,

			// f1--f12
			KEY_F1,
			KEY_F2,
			KEY_F3,
			KEY_F4,
			KEY_F5,
			KEY_F6,
			KEY_F7,
			KEY_F8,
			KEY_F9,
			KEY_F10,
			KEY_F11,
			KEY_F12,

			// arrow
			KEY_LEFT,
			KEY_UP,
			KEY_RIGHT,
			KEY_DOWN,

			// modifier
			KEY_ALT,
			KEY_CTRL,
			KEY_SHIFT,

			// operator
			KEY_BACKSPACE,
			KEY_DELETE,
			KEY_ENTER,
			KEY_ESCAPE,
			KEY_SPACE,
			KEY_TAB,

			// mouse
			KEY_MOUSE_LEFT,
			KEY_MOUSE_MIDDLE,
			KEY_MOUSE_RIGHT,

			// unknown
			KEY_UNKNOWN,

			// ...
			NUM_KEYS,
		};

		//struct keyboard_t
		//{
		//	key_t keys[NUM_KEYS];
		//};
	}
}