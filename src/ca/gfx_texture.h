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

		enum texturelayout		// usage?

		{
/*
VK_IMAGE_LAYOUT_UNDEFINED = 0,
VK_IMAGE_LAYOUT_GENERAL = 1,
VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL = 2,
VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL = 3,
VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL = 4,
VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL = 5,
VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL = 6,
VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL = 7,
VK_IMAGE_LAYOUT_PREINITIALIZED = 8,
VK_IMAGE_LAYOUT_PRESENT_SRC_KHR = 1000001002,
*/

			TEXTURELAYOUT_UNDEFINED,
			TEXTURELAYOUT_PRESENT,
			TEXTURELAYOUT_SAMPLE_COLOR,
			TEXTURELAYOUT_TRANSFER_DST,
			TEXTURELAYOUT_TRANSFER_SRC,
			NUM_TEXTURELAYOUTS,
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