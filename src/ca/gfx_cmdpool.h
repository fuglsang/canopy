#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		struct cmdpool_t
		{
			void * handle;
			device_t * device;
		};

		void create_cmdpool(cmdpool_t * cmdpool, device_t * device);
		void destroy_cmdpool(cmdpool_t * cmdpool);

		void cmdpool_reset(cmdpool_t * cmdpool);
	}
}