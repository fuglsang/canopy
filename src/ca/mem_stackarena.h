#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct stackarena_t : arena_t
		{
			void * next;
			size_t free;
		};

		void create_arena(stackarena_t * arena, void * base, size_t size);
		template <typename A>
		void create_arena(stackarena_t * arena, A * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		void * arena_alloc(stackarena_t * arena, size_t size, size_t alignment);
		void arena_free(stackarena_t * arena, void * block);
	}
}