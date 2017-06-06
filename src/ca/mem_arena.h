#pragma once

#include "ca/types.h"
#include "ca/core_threadpolicy.h"

namespace ca
{
	namespace mem
	{
		template <typename TAllocator, typename TThreadPolicy>
		struct arena_t
		{
			TAllocator allocator;
			TThreadPolicy threadpolicy;
		};

		template <typename TArena>
		inline void create_arena(TArena * arena, void * base, size_t size);

		template <typename TArena, typename TBaseArena>
		inline void create_arena(TArena * arena, TBaseArena * base_arena, size_t size);

		template <typename TArena>
		inline void destroy_arena(TArena * arena);

		template <typename TArena>
		inline void * arena_alloc(TArena * arena, size_t size, size_t alignment);

		template <typename T, typename TArena>
		inline T * arena_alloc(TArena * arena, u32 count);

		template <typename TArena>
		inline void arena_free(TArena * arena, void * block);
	}
}

#include "ca/mem_arena.inl"