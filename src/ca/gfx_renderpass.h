#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_texture.h"
#include "ca/core_array.h"

namespace ca
{
	namespace gfx
	{
		struct renderpass_t
		{
			void * handle;
			device_t * device;
		};

		enum renderloadop
		{
			RENDERLOADOP_CLEAR,
			RENDERLOADOP_DISCARD,
			RENDERLOADOP_LOAD,
			NUM_RENDERLOADOPS,
		};

		enum renderstoreop
		{
			RENDERSTOREOP_DISCARD,
			RENDERSTOREOP_STORE,
			NUM_RENDERSTOREOPS,
		};

		struct renderattachment_t
		{
			texture_t * texture;
			renderloadop loadop;
			renderstoreop storeop;
			f32 clear_color[4];
			f32 clear_depth;
			u32 clear_stencil;
		};

		void create_renderpass(renderpass_t * renderpass, device_t * device, renderattachment_t * attachments, u32 attachment_count);
		void destroy_renderpass(renderpass_t * renderpass);
	}
}