#include "ca/core_assert.h"
#include "ca/mem_lineararena.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace mem
	{
		void create_arena(lineararena_t * arena, void * base, size_t size)
		{
			arena->base = base;
			arena->size = size;
			arena->used = 0;
			arena->free = size;
		}

		void * arena_alloc(lineararena_t * arena, size_t size, size_t alignment)
		{
			void * block_base = ptr_add(arena->base, arena->used);
			void * block = align_up(block_base, alignment);

			size_t req_align = ptr_diff(block_base, block);
			size_t req_total = size + req_align;

			CA_ASSERT_MSG(arena->free >= req_total, "linear arena out of memory");

			arena->used += req_total;
			arena->free -= req_total;

			return block;
		}

		void arena_free(lineararena_t * arena, void * block)
		{
			CA_ASSERT_MSG(false, "not supported");
		}
	}
}