#include "ca/core_assert.h"
#include "ca/mem_chunkarena.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace mem
	{
		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void create_arena(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, void * base, size_t size)
		{
			arena->base = base;
			arena->size = size;
			arena->used = 0;
			arena->free = size;
		}

		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void * arena_alloc(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, size_t size, size_t alignment)
		{
			return nullptr;//TODO
		}

		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void arena_free(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, void * block)
		{
			CA_ASSERT_MSG(false, "not supported");
		}
	}
}