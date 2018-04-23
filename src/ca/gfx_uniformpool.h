#pragma once

#include "ca_base.h"
#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		struct uniformpool_t
		{
			void * handle;
			device_t * device;
		};

		void create_uniformpool(uniformpool_t * uniformpool, device_t * device, u32 max_uniforms, u32 max_uniformsets);
		void destroy_uniformpool(uniformpool_t * uniformpool);
	}
}