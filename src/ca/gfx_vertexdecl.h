#pragma once

#include "ca/types.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace math
	{
		template <typename T, u32 N>
		struct vec_t;

		template <typename T, u32 N>
		struct mat_t;

		template <typename T>
		struct quat_t;
	}

	namespace gfx
	{
		enum vertexattribtype
		{
			VERTEXATTRIBTYPE_F32,
			VERTEXATTRIBTYPE_I32,
			VERTEXATTRIBTYPE_U32,

			VERTEXATTRIBTYPE_F64,
			VERTEXATTRIBTYPE_I64,
			VERTEXATTRIBTYPE_U64,
			
			NUM_VERTEXATTRIBTYPES,
		};

		template <typename T>
		struct resolve_vertexattribtype_t
		{
			static u32 const type = NUM_VERTEXATTRIBTYPES;
			static u32 const size = 0;
		};

		template <>
		struct resolve_vertexattribtype_t<f32>
		{
			static u32 const type = VERTEXATTRIBTYPE_F32;
			static u32 const size = 1;
		};

		template <>
		struct resolve_vertexattribtype_t<i32>
		{
			static u32 const type = VERTEXATTRIBTYPE_I32;
			static u32 const size = 1;
		};

		template <>
		struct resolve_vertexattribtype_t<u32>
		{
			static u32 const type = VERTEXATTRIBTYPE_U32;
			static u32 const size = 1;
		};

		template <>
		struct resolve_vertexattribtype_t<f64>
		{
			static u32 const type = VERTEXATTRIBTYPE_F64;
			static u32 const size = 1;
		};

		template <>
		struct resolve_vertexattribtype_t<i64>
		{
			static u32 const type = VERTEXATTRIBTYPE_I64;
			static u32 const size = 1;
		};

		template <>
		struct resolve_vertexattribtype_t<u64>
		{
			static u32 const type = VERTEXATTRIBTYPE_U64;
			static u32 const size = 1;
		};

		template <typename T, u32 N>
		struct resolve_vertexattribtype_t<math::vec_t<T, N>>
		{
			static u32 const type = resolve_vertexattribtype_t<T>::type;
			static u32 const size = N;
		};

		template <typename T, u32 N>
		struct resolve_vertexattribtype_t<math::mat_t<T, N>>
		{
			static u32 const type = resolve_vertexattribtype_t<T>::type;
			static u32 const size = N;
		};

		template <typename T>
		struct resolve_vertexattribtype_t<math::quat_t<T>>
		{
			static u32 const type = resolve_vertexattribtype_t<T>::type;
			static u32 const size = 4;
		};

		struct vertexdecl_t
		{
			static u32 const MAX_BUFFERS = 4;
			static u32 const MAX_ATTRIBS = 32;

			struct bufferdecl_t
			{
				u32 binding;
				size_t stride;
			};

			struct attribdecl_t
			{
				u32 buffer_index;
				u32 location;
				size_t offset;
				vertexattribtype component_type;
				u32 component_count;
			};

			bufferdecl_t buffers[MAX_BUFFERS] = {};
			attribdecl_t attribs[MAX_ATTRIBS] = {};

			u32 buffer_count = 0;
			u32 attrib_count = 0;
			u32 location_mask = 0;
		};

		inline void declare_vertexbuffer(vertexdecl_t * vertexdecl, u32 binding, size_t stride)
		{
			CA_ASSERT(binding < vertexdecl_t::MAX_BUFFERS);
			
			vertexdecl_t::bufferdecl_t * bufferdecl = &vertexdecl->buffers[vertexdecl->buffer_count++];
			bufferdecl->binding = binding;
			bufferdecl->stride = stride;
		}

		inline void declare_vertexattrib(vertexdecl_t * vertexdecl, u32 location, size_t offset, vertexattribtype component_type, u32 component_count)
		{
			CA_ASSERT(vertexdecl->buffer_count > 0);
			CA_ASSERT(location < vertexdecl_t::MAX_ATTRIBS);

			u32 location_count = 0;
			switch (component_type)
			{
			case VERTEXATTRIBTYPE_F32:
			case VERTEXATTRIBTYPE_I32:
			case VERTEXATTRIBTYPE_U32:
				location_count = mem::align_up(4 * component_count, 16) / 16;
				break;

			case VERTEXATTRIBTYPE_F64:
			case VERTEXATTRIBTYPE_I64:
			case VERTEXATTRIBTYPE_U64:
				location_count = mem::align_up(8 * component_count, 16) / 16;
				break;
			}

			u32 location_mask = vertexdecl->location_mask;
			CA_LOG("trying location %u, component_count %u, location_count %u", location, component_count, location_count);
			for (u32 i = location; i != location_count; i++)
			{
				CA_ASSERT_MSG((location_mask & (1 << i)) == 0, "attribute location overlap");
				location_mask |= (1 << i);
			}

			vertexdecl_t::attribdecl_t * attribdecl = &vertexdecl->attribs[vertexdecl->attrib_count++];
			attribdecl->buffer_index = vertexdecl->buffer_count - 1;
			attribdecl->location = location;
			attribdecl->offset = offset;
			attribdecl->component_type = component_type;
			attribdecl->component_count = component_count;
			vertexdecl->location_mask = location_mask;
		}

		template <typename T1, typename T2>
		inline void declare_vertexattrib(vertexdecl_t * vertexdecl, u32 location, T1 T2::*member)
		{
			size_t offset = mem::ptr_offsetof(member);
			u32 component_type = resolve_vertexattribtype_t<T1>::type;
			u32 component_count = resolve_vertexattribtype_t<T1>::size;
			declare_vertexattrib(vertexdecl, location, offset, (vertexattribtype)component_type, component_count);
		}
	}
}