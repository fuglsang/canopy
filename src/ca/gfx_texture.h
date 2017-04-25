#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum texturetype
		{
			TEXTURETYPE_2D,
			TEXTURETYPE_3D,
			NUM_TEXTURETYPES,
		};

		struct texture_t
		{
			void * handle;
			u32 width;
			u32 height;
			texturetype type;
		};

		void create_texture(texture_t * texture, device_t * device, u32 width, u32 height);
		void destroy_texture(texture_t * texture, device_t * device);
	}
}