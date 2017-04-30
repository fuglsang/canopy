#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_shader.h"

namespace ca
{
	namespace gfx
	{
		struct pipeline_t
		{
			void * handle;
			device_t * device;
			shader_t * stages[NUM_SHADERTYPES];
		};

		void create_pipeline(pipeline_t * pipeline, device_t * device);
		void destroy_pipeline(pipeline_t * pipeline);

		void pipeline_bind_stage(pipeline_t * pipeline, shadertype stage, shader_t * shader);
		void pipeline_unbind_stage(pipeline_t * pipeline, shadertype stage);
	}
}