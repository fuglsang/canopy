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

		struct cmdbuffer_t;
		struct fence_t;
		struct semaphore_t;

		void create_device(device_t * device, mem::heaparena_t * arena);
		void destroy_device(device_t * device);

		void device_submit(device_t * device, cmdbuffer_t * cmdbuffer, semaphore_t * wait_semaphore, semaphore_t * signal_semaphore, fence_t * signal_fence);
	}
}