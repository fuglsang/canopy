#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum shaderstage
		{
			SHADERSTAGE_COMPUTE,
			SHADERSTAGE_FRAGMENT,
			SHADERSTAGE_GEOMETRY,
			SHADERSTAGE_VERTEX,
			NUM_SHADERSTAGES,
		};

		struct shader_t
		{
			void * handle;
			device_t * device;
			shaderstage stage;
		};

		void create_shader(shader_t * shader, device_t * device, shaderstage stage, char const * glsl_source, size_t glsl_source_size);
		void destroy_shader(shader_t * shader);
	}
}