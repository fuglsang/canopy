#pragma once

#include "ca/mem_arena.h"
#include "ca/types.h"

namespace ca
{
	namespace mem
	{
		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		struct chunkarena_t : arena_t
		{
			size_t used;
			size_t free;
		};

		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void create_arena(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, void * base, size_t size);
		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT, typename A>
		void create_arena(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, A * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void * arena_alloc(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, size_t size, size_t alignment);
		template <u32 CHUNK_SIZE, u32 CHUNK_ALIGNMENT>
		void arena_free(chunkarena_t<CHUNK_SIZE, CHUNK_ALIGNMENT> * arena, void * block);
	}
}