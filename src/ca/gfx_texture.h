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

		enum textureformat
		{
			// depth
			TEXTUREFORMAT_DEPTH16,
			TEXTUREFORMAT_DEPTH32,

			// depth-stencil
			TEXTUREFORMAT_DEPTH24_STENCIL8,

			// rgba
			TEXTUREFORMAT_R8,
			TEXTUREFORMAT_RG8,
			TEXTUREFORMAT_RGB8,
			TEXTUREFORMAT_RGBA8,
			
			// rgba float
			TEXTUREFORMAT_R16F,
			TEXTUREFORMAT_RG16F,

			NUM_TEXTUREFORMATS,
		};

		struct texture_t
		{
			void * handle;
			device_t * device;
			texturetype type;
			textureformat format;
			u32 dim_x;
			u32 dim_y;
		};

		void create_texture(texture_t * texture, device_t * device);
		void destroy_texture(texture_t * texture);
	}
}