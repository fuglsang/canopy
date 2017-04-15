#include "ca/core_assert.h"
#include "ca/mem_heaparena.h"
#include "ca/mem_util.h"

#include <tlsf/tlsf.h>

namespace ca
{
	namespace mem
	{
		void create_arena(heaparena_t * arena, void * base, size_t size)
		{
			arena->base = base;
			arena->size = size;
			arena->handle = tlsf_create_with_pool(base, size);
		}

		void * arena_alloc(heaparena_t * arena, size_t size, size_t alignment)
		{
			void * block = tlsf_memalign(arena->handle, alignment, size);
			CA_ASSERT_MSG(block != nullptr, "heap arena out of memory");
			return block;
		}

		void arena_free(heaparena_t * arena, void * block)
		{
			tlsf_free(arena->handle, block);
		}
	}
}