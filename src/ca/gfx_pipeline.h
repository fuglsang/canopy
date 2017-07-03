#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_shader.h"
#include "ca/gfx_framebuffer.h"
#include "ca/gfx_shaderdecl.h"
#include "ca/gfx_vertexdecl.h"

namespace ca
{
	namespace gfx
	{
		enum pipelinetype
		{
			PIPELINETYPE_COMPUTE,
			PIPELINETYPE_GRAPHICS,
			NUM_PIPELINETYPES
		};

		struct pipeline_t
		{
			void * handle;
			device_t * device;
			pipelinetype type;
		};

		void create_pipeline(pipeline_t * pipeline, framebuffer_t * framebuffer, shader_t * stages, u32 stage_count, shaderdecl_t * xdecl, vertexdecl_t * vdecl);
		void destroy_pipeline(pipeline_t * pipeline);
	}
}