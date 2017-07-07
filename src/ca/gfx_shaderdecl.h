#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace gfx
	{
		enum shaderprop
		{
			SHADERPROP_SAMPLER,
			SHADERPROP_STORAGE_BUFFER,
			SHADERPROP_TEXTURE,
			SHADERPROP_UNIFORM_BUFFER,
			NUM_SHADERPROPS,
		};

		struct shaderdecl_t
		{
			static u32 const MAX_PROPERTIES = 32;

			struct propertydecl_t
			{
				u32 binding;
				shaderprop type;
				shaderstage stage;
			};

			propertydecl_t properties[MAX_PROPERTIES];

			u32 property_count = 0;
			u32 binding_mask = 0;
		};

		inline void declare_property(shaderdecl_t * shaderdecl, u32 binding, shaderprop type, shaderstage stage)
		{
			CA_ASSERT(binding < shaderdecl_t::MAX_PROPERTIES);

			u32 binding_mask = shaderdecl->binding_mask;
			u32 binding_bit = 1 << binding;

			CA_ASSERT_MSG((binding_mask & binding_bit) == 0, "shader property binding overlap");
			binding_mask |= binding_bit;

			shaderdecl_t::propertydecl_t * propertydecl = &shaderdecl->properties[shaderdecl->property_count++];
			propertydecl->binding = binding;
			propertydecl->type = type;
			propertydecl->stage = stage;
			shaderdecl->binding_mask = binding_mask;
		}
	}
}