#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct lineararena_t : arena_t
		{
			size_t used;
			size_t free;
		};

		void create_arena(lineararena_t * arena, void * base, size_t size);
		template <typename A>
		void create_arena(lineararena_t * arena, A * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		void * arena_alloc(lineararena_t * arena, size_t size, size_t alignment);
		void arena_free(lineararena_t * arena, void * block);
	}
}