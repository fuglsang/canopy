#pragma once

#include "ca/types.h"
#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct chunkarena_t : arena_t
		{
			size_t used;
			size_t free;
		};

		void create_arena(chunkarena_t * arena, void * base, size_t size);
		template <typename A>
		void create_arena(chunkarena_t * arena, A * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		void * arena_alloc(chunkarena_t * arena, size_t size, size_t alignment);
		void arena_free(chunkarena_t * arena, void * block);
	}
}