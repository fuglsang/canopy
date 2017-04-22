#pragma once

namespace ca
{
	namespace mem
	{
		struct arena_t
		{
			void * base;
			size_t size;
		};

		template <typename A>
		struct scoped_arena_block
		{
			A * arena;
			void * block;

			~scoped_arena_block()
			{
				arena_free(arena, block);
			}
		};

		template <typename A>
		inline scoped_arena_block<A> arena_free_on_return(A * arena, void * block)
		{
			return { arena, block };
		}
	}
}