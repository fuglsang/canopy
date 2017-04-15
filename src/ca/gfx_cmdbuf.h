#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		struct cmdbuf_t
		{
			void * handle;
		};

		void create_cmdbuf(device_t * device, cmdbuf_t * cmdbuf);
		void destroy_cmdbuf(device_t * device, cmdbuf_t * cmdbuf);
	}
}