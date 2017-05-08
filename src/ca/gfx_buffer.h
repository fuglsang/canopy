#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum buffertype
		{
			BUFFERTYPE_INDEX,
			BUFFERTYPE_VERTEX,
			BUFFERTYPE_STORAGE,
			BUFFERTYPE_UNIFORM,
			NUM_BUFFERTYPES,
		};

		struct buffer_t
		{
			void * handle;
			device_t * device;
			buffertype type;
		};

		void create_buffer(buffer_t * buffer, device_t * device, buffertype type, size_t size);
		void destroy_buffer(buffer_t * buffer);
	}
}