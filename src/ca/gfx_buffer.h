#pragma once

#include "ca/gfx_device.h"

namespace ca
{
	namespace gfx
	{
		enum buffertype
		{
			BUFFERTYPE_INDEX,
			BUFFERTYPE_STORAGE,
			BUFFERTYPE_UNIFORM,
			BUFFERTYPE_VERTEX,
			NUM_BUFFERTYPES,
		};

		enum buffermemorytype
		{
			BUFFERMEMORYTYPE_MAPPABLE,
			BUFFERMEMORYTYPE_MAPPABLE_COHERENT,
			BUFFERMEMORYTYPE_STAGED,
			NUM_BUFFERMEMORYTYPES,
		};

		struct buffer_t
		{
			void * handle;
			device_t * device;
			buffertype type;
			buffermemorytype memory_type;
			size_t size;
		};

		void create_buffer(buffer_t * buffer, device_t * device, buffertype type, buffermemorytype memory_type, size_t size);
		void destroy_buffer(buffer_t * buffer);

		void * buffer_map(buffer_t * buffer, size_t offset, size_t size);
		void buffer_flush_range(buffer_t * buffer, size_t offset, size_t size);
		void buffer_invalidate_range(buffer_t * buffer, size_t offset, size_t size);
		void buffer_unmap(buffer_t * buffer);
	}
}