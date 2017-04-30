#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		struct semaphore_t
		{
			void * handle;
			device_t * device;
		};

		void create_semaphore(semaphore_t * semaphore, device_t * device);
		void destroy_semaphore(semaphore_t * semaphore);
	}
}