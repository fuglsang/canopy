#include "ca/input_pointer.h"

namespace ca
{
	namespace input
	{
		void pointer_clear(pointer_t * pointer)
		{
			pointer->tracking = false;
			pointer->position = { -1, -1 };
		}
	}
}