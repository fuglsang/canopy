#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_shader.h"
#include "ca/gfx_framebuffer.h"

namespace ca
{
	namespace gfx
	{
		struct pipeline_t
		{
			void * handle;
			device_t * device;
		};

		void create_pipeline(pipeline_t * pipeline, framebuffer_t * framebuffer);
		void destroy_pipeline(pipeline_t * pipeline);
	}
}