#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace gfx
	{
		enum uniformtype
		{
			UNIFORMTYPE_CONSTANT_BUFFER,
			UNIFORMTYPE_SAMPLER,
			UNIFORMTYPE_STORAGE_BUFFER,
			UNIFORMTYPE_TEXTURE,
			END_UNIFORMTYPE,
		};

		struct uniformsetdecl_t
		{
			static u32 const MAX_UNIFORMS = 32;

			struct bindingdecl_t
			{
				u32 binding;
				uniformtype type;
				shaderstage stage;
			};

			bindingdecl_t properties[MAX_UNIFORMS];

			u32 property_count = 0;
			u32 binding_mask = 0;
		};

		inline void uniformsetdecl_binding(uniformsetdecl_t * uniformsetdecl, u32 binding, uniformtype type, shaderstage stage)
		{
			CA_ASSERT(binding < uniformsetdecl_t::MAX_UNIFORMS);

			u32 binding_mask = uniformsetdecl->binding_mask;
			u32 binding_bit = 1 << binding;

			CA_ASSERT_MSG((binding_mask & binding_bit) == 0, "shader property binding overlap");
			binding_mask |= binding_bit;

			uniformsetdecl_t::bindingdecl_t * uniformdecl = &uniformsetdecl->properties[uniformsetdecl->property_count++];
			uniformdecl->binding = binding;
			uniformdecl->type = type;
			uniformdecl->stage = stage;
			uniformsetdecl->binding_mask = binding_mask;
		}
	}
}