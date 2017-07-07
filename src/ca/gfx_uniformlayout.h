#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_shaderdecl.h"

namespace ca
{
	namespace gfx
	{
		struct uniformlayout_t
		{
			void * handle;
			device_t * device;
		};

		void create_uniformlayout(uniformlayout_t * uniformlayout, device_t * device, shaderdecl_t * shaderdecl);
		void destroy_uniformlayout(uniformlayout_t * uniformlayout);
	}
}