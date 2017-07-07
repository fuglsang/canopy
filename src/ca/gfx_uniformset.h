#pragma once

#include "ca/gfx_shaderdecl.h"
#include "ca/gfx_uniformpool.h"

namespace ca
{
	namespace gfx
	{
		struct uniformset_t
		{
			void * handle;
			uniformpool_t * uniformpool;
		};

		void create_uniformset(uniformset_t * uniformset, uniformpool_t * uniformpool, shaderdecl_t * shaderdecl);
		void destroy_uniformset(uniformset_t * uniformset);
	}
}