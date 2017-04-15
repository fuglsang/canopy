#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct heaparena_t : arena_t
		{
			void * handle;
		};

		void create_arena(heaparena_t * arena, void * base, size_t size);
		template <typename A>
		void create_arena(heaparena_t * arena, A * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		void * arena_alloc(heaparena_t * arena, size_t size, size_t alignment);
		void arena_free(heaparena_t * arena, void * block);
	}
}