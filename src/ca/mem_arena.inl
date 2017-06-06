#pragma once

namespace ca
{
	namespace mem
	{
		template <typename TArena>
		inline void create_arena(TArena * arena, void * base, size_t size)
		{
			create_allocator(&arena->allocator, base, size);
			core::create_threadpolicy(&arena->threadpolicy);
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
			core::destroy_threadpolicy(&arena->threadpolicy);
		}

		template <typename TArena>
		inline void * arena_alloc(TArena * arena, size_t size, size_t alignment)
		{
			core::threadpolicy_begin(&arena->threadpolicy);
			void * block = allocator_alloc(&arena->allocator, size, alignment);
			core::threadpolicy_end(&arena->threadpolicy);
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
			core::threadpolicy_begin(&arena->threadpolicy);
			allocator_free(&arena->allocator, block);
			core::threadpolicy_end(&arena->threadpolicy);
		}
	}
}