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
			shadertype type;
		};

		void createshader(device_t * device, shader_t * shader);
		void destroyshader(device_t * device, shader_t * shader);
	}
}