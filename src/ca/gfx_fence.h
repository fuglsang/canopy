#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		struct fence_t
		{
			void * handle;
			device_t * device;
		};

		void create_fence(fence_t * fence, device_t * device, bool signaled);
		void destroy_fence(fence_t * fence);

		void fence_peek_signaled(fence_t * fence, bool * signaled);
		void fence_reset_signaled(fence_t * fence);
		void fence_wait_signaled(fence_t * fence);
		void fence_wait_reset_signaled(fence_t * fence);
	}
}