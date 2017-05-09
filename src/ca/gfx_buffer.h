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
			size_t size;
		};

		void create_buffer(buffer_t * buffer, device_t * device, buffertype type, size_t size);
		void destroy_buffer(buffer_t * buffer);

		void * buffer_map(buffer_t * buffer, size_t offset, size_t size);
		void buffer_mapped_flush(buffer_t * buffer, size_t offset, size_t size);
		void buffer_mapped_invalidate(buffer_t * buffer, size_t offset, size_t size);
		void buffer_unmap(buffer_t * buffer);
	}
}