#pragma once

#include "ca/gfx_uniformpool.h"
#include "ca/gfx_uniformlayout.h"

namespace ca
{
	namespace gfx
	{
		struct uniformset_t
		{
			void * handle;
			uniformpool_t * uniformpool;
			uniformlayout_t * uniformlayout;
		};

		void create_uniformset(uniformset_t * uniformset, uniformpool_t * uniformpool, uniformlayout_t * uniformlayout);
		void destroy_uniformset(uniformset_t * uniformset);

		void uniformset_update_index(uniformset_t * uniformset, u32 uniformindex, buffer_t * buffer);
	}
}