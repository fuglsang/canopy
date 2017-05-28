#pragma once

namespace ca
{
	namespace mem
	{
		template <typename TArena>
		inline void create_arena(TArena * arena, void * base, size_t size)
		{
			create_allocator(&arena->allocator, base, size);
			create_accesspolicy(&arena->accesspolicy);
		}

		template <typename TArena, typename TBaseArena>
		inline void create_arena(TArena * arena, TBaseArena * base_arena, size_t size)
		{
			create_arena(arena, arena_alloc(base_arena, size, 1), size);
		}

		template <typename TArena>
		inline void destroy_arena(TArena * arena)
		{
			destroy_allocator(&arena->allocator);
			destroy_accesspolicy(&arena->accesspolicy);
		}

		template <typename TArena>
		inline void * arena_alloc(TArena * arena, size_t size, size_t alignment)
		{
			accesspolicy_begin(&arena->accesspolicy);
			void * block = allocator_alloc(&arena->allocator, size, alignment);
			accesspolicy_end(&arena->accesspolicy);
			return block;
		}

		template <typename T, typename TArena>
		inline T * arena_alloc(TArena * arena, u32 count)
		{
			return reinterpret_cast<T *>(arena_alloc(arena, count * sizeof(T), alignof(T)));
		}

		template <typename TArena>
		inline void arena_free(TArena * arena, void * block)
		{
			accesspolicy_begin(&arena->accesspolicy);
			allocator_free(&arena->allocator, block);
			accesspolicy_end(&arena->accesspolicy);
		}
	}
}