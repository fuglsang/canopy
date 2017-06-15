#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum textureformat
		{
			// depth formats
			TEXTUREFORMAT_DEPTH16,
			TEXTUREFORMAT_DEPTH32F,
			TEXTUREFORMAT_DEPTH24_STENCIL8,

			// color formats
			TEXTUREFORMAT_R8,
			TEXTUREFORMAT_RG8,
			TEXTUREFORMAT_RGB8,
			TEXTUREFORMAT_RGBA8,
			TEXTUREFORMAT_RGB10A2,

			// float formats
			TEXTUREFORMAT_R16F,
			TEXTUREFORMAT_RG16F,

			// ...
			NUM_TEXTUREFORMATS,
		};

		struct texture_t
		{
			void * handle;
			device_t * device;
			textureformat format;
			u32 width;
			u32 height;
		};

		void create_texture(texture_t * texture, device_t * device, textureformat format, u32 width, u32 height);
		void destroy_texture(texture_t * texture);
	}
}