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

		enum textureusage
		{
			NUM_TEXTUREUSAGES,
		};

		struct texture_t
		{
			void * handle;
			device_t * device;
			//texturetype type;
			//u32 width;
			//u32 height;
		};

		void create_texture(texture_t * texture, device_t * device);
		void destroy_texture(texture_t * texture);
	}
}