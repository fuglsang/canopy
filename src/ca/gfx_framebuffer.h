#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_texture.h"
#include "ca/core_array.h"

namespace ca
{
	namespace gfx
	{
		struct framebuffer_t
		{
			void * handle;
			device_t * device;
			u32 width;
			u32 height;
		};

		enum rendertargetloadop
		{
			RENDERTARGETLOADOP_CLEAR,
			RENDERTARGETLOADOP_DISCARD,
			RENDERTARGETLOADOP_LOAD,
			NUM_RENDERTARGETLOADOPS,
		};

		enum rendertargetstoreop
		{
			RENDERTARGETSTOREOP_DISCARD,
			RENDERTARGETSTOREOP_STORE,
			NUM_RENDERTARGETSTOREOPS,
		};

		struct rendertarget_t
		{
			texture_t * texture;
			rendertargetloadop loadop;
			rendertargetstoreop storeop;
			f32 clear_color[4];
			f32 clear_depth;
			u32 clear_stencil;
		};

		void create_framebuffer(framebuffer_t * framebuffer, device_t * device, rendertarget_t * attachments, u32 attachment_count);
		void destroy_framebuffer(framebuffer_t * framebuffer);
	}
}