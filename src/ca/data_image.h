#pragma once

#include "ca/types.h"
#include "ca/data_buffer.h"

namespace ca
{
	namespace data
	{
		enum imageformat
		{
			IMAGEFORMAT_R8,
			IMAGEFORMAT_R8G8,
			IMAGEFORMAT_R8G8B8,
			IMAGEFORMAT_R8G8B8A8,
			NUM_IMAGEFORMATS,
		};

		struct image_t
		{
			buffer_t * buffer;
			u32 width;
			u32 height;
			imageformat format;
		};

		void create_image(image_t * image, u32 width, u32 height, imageformat format);
		void destroy_image(image_t * image);
	}
}