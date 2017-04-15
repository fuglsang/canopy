#include "ca/core_assert.h"
#include "ca/mem_stackarena.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace mem
	{
		struct stackframe_t
		{
			void * block_base;
			void * block;
		};

		void create_arena(stackarena_t * arena, void * base, size_t size)
		{
			arena->base = base;
			arena->size = size;
			arena->next = base;
			arena->free = size;
		}

		void * arena_alloc(stackarena_t * arena, size_t size, size_t alignment)
		{
			void * block_base = arena->next;
			void * block = align_up(block_base, alignment);

			size_t req_align = ptr_diff(block_base, block);
			size_t req_total = size + req_align + sizeof(stackframe_t);

			CA_ASSERT_MSG(arena->free >= req_total, "stack arena out of memory");

			stackframe_t * frame = reinterpret_cast<stackframe_t *>(ptr_add(block, size));
			
			frame->block_base = block_base;
			frame->block = block;

			arena->next = ptr_add(arena->next, req_total);
			arena->free = arena->size - ptr_diff(arena->base, arena->next);

			return block;
		}

		void arena_free(stackarena_t * arena, void * block)
		{
			CA_ASSERT_MSG(0 <= ptr_diff(arena->base, block) && ptr_diff(arena->base, block) < (ptrdiff_t)arena->size, "cannot free alien block");
			CA_ASSERT_MSG(ptr_diff(arena->base, arena->next) > 0, "stack arena underflow");

			stackframe_t * frame = reinterpret_cast<stackframe_t *>(arena->next) - 1;
			
			CA_ASSERT_MSG(ptr_equals(block, frame->block), "stack arena operations must be filo");

			arena->base = frame->block_base;
			arena->free = arena->size - ptr_diff(arena->base, arena->next);
		}
	}
}