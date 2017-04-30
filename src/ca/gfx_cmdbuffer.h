#pragma once

#include "ca/gfx_device.h"
#include "ca/math_vec.h"

namespace ca
{
	namespace gfx
	{
		enum cmdbuffertype
		{
			CMDBUFFERTYPE_GRAPHICS,
			CMDBUFFERTYPE_TRANSFER,
			CMDBUFFERTYPE_COMPUTE,
			NUM_CMDBUFFERTYPES,
		};

		struct cmdbuffer_t
		{
			void * handle;
			device_t * device;
			cmdbuffertype type;
		};

		void create_cmdbuffer(cmdbuffer_t * cmdbuffer, device_t * device, cmdbuffertype type);
		void destroy_cmdbuffer(cmdbuffer_t * cmdbuffer);

		void cmdbuffer_begin(cmdbuffer_t * cmdbuffer);
		void cmdbuffer_draw(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 vertex_count);
		void cmdbuffer_draw_indexed(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 index_start, u32 index_count);
		void cmdbuffer_end(cmdbuffer_t * cmdbuffer);
	}
}