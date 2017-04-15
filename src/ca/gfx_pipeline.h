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
			shader_t stages[NUM_SHADERTYPES];
		};

		void createpipeline(device_t * device, pipeline_t * pipeline);
		void destroypipeline(device_t * device, pipeline_t * pipeline);

		void bindpipelinestage(device_t * device, pipeline_t * pipeline, shadertype stage, shader_t * shader);
		void unbindpipelinestage(device_t * device, pipeline_t * pipeline, shadertype stage);
	}
}