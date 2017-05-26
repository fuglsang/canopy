#pragma once

#include "ca/math_vec.h"
#include "ca/gfx_cmdpool.h"
#include "ca/gfx_renderpass.h"
#include "ca/gfx_texture.h"

namespace ca
{
	namespace gfx
	{
		struct cmdbuffer_t
		{
			void * handle;
			cmdpool_t * cmdpool;
		};

		void create_cmdbuffer(cmdbuffer_t * cmdbuffer, cmdpool_t * cmdpool);
		void destroy_cmdbuffer(cmdbuffer_t * cmdbuffer);

		void cmdbuffer_reset(cmdbuffer_t * cmdbuffer);
		void cmdbuffer_begin(cmdbuffer_t * cmdbuffer);
		void cmdbuffer_begin_renderpass(cmdbuffer_t * cmdbuffer, renderpass_t * renderpass);		
		void cmdbuffer_clear_color(cmdbuffer_t * cmdbuffer, texture_t * texture, math::vec4_t const & color);
		void cmdbuffer_bind_indexbuffer(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset, size_t stride);
		void cmdbuffer_bind_vertexbuffer(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset);
		void cmdbuffer_draw(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 vertex_count);
		void cmdbuffer_draw_indexed(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 index_start, u32 index_count);		
		void cmdbuffer_end_renderpass(cmdbuffer_t * cmdbuffer);
		void cmdbuffer_end(cmdbuffer_t * cmdbuffer);
	}
}