#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace gfx
	{
		struct device_t
		{
			void * handle;
			mem::arena_t * arena;
		};

		void create_device(mem::arena_t * arena, device_t * device);
		void destroy_device(mem::arena_t * arena, device_t * device);
	}
}