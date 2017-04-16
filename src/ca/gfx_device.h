#pragma once

#include "ca/mem_heaparena.h"

namespace ca
{
	namespace gfx
	{
		struct device_t
		{
			void * handle;
			mem::heaparena_t * arena;
		};

		void create_device(device_t * device, mem::heaparena_t * arena);
		void destroy_device(device_t * device);
	}
}