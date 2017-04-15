#include "ca/core_assert.h"
#include "ca/mem_chunkarena.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace mem
	{
		void create_arena(chunkarena_t * arena, void * base, size_t size)
		{
			arena->base = base;
			arena->size = size;
			arena->used = 0;
			arena->free = size;
		}

		void * arena_alloc(chunkarena_t * arena, size_t size, size_t alignment)
		{
			return nullptr;//TODO
		}

		void arena_free(chunkarena_t * arena, void * block)
		{
			CA_ASSERT_MSG(false, "not supported");
		}
	}
}