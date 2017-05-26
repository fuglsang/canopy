#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum shadertype
		{
			SHADERTYPE_VERTEX,
			SHADERTYPE_GEOMETRY,
			SHADERTYPE_FRAGMENT,
			NUM_SHADERTYPES,
		};

		struct shader_t
		{
			void * handle;
			device_t * device;
			shadertype type;
		};

		void create_shader(shader_t * shader, device_t * device, char const * glsl_source, size_t glsl_source_size);
		void destroy_shader(shader_t * shader);
	}
}