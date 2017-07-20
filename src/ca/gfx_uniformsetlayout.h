#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_uniformsetdecl.h"

namespace ca
{
	namespace gfx
	{
		struct uniformsetlayout_t
		{
			void * handle;
			device_t * device;
		};

		void create_uniformsetlayout(uniformsetlayout_t * uniformsetlayout, device_t * device, uniformsetdecl_t * uniformsetdecl);
		void destroy_uniformsetlayout(uniformsetlayout_t * uniformsetlayout);
	}
}