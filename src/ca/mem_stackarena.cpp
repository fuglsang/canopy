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

		void create_allocator(stackallocator_t * allocator, void * base, size_t size)
		{
			allocator->base = base;
			allocator->size = size;
			allocator->next = base;
			allocator->free = size;
		}

		void destroy_allocator(stackallocator_t * allocator) {}

		void * allocator_alloc(stackallocator_t * allocator, size_t size, size_t alignment)
		{
			void * block_base = allocator->next;
			void * block = align_up(block_base, alignment);

			size_t req_align = ptr_diff(block_base, block);
			size_t req_total = size + req_align + sizeof(stackframe_t);
			
			CA_ASSERT_MSG(allocator->free >= req_total, "stack allocator out of memory");

			allocator->next = ptr_add(allocator->next, req_total);
			allocator->free = allocator->size - ptr_diff(allocator->base, allocator->next);

			stackframe_t * frame = reinterpret_cast<stackframe_t *>(allocator->next) - 1;
			
			frame->block_base = block_base;
			frame->block = block;

			return block;
		}

		void allocator_free(stackallocator_t * allocator, void * block)
		{
			CA_ASSERT_MSG(ptr_diff(allocator->base, block) >= 0, "cannot free alien block");
			CA_ASSERT_MSG(ptr_diff(allocator->base, block) < static_cast<ptrdiff_t>(allocator->size), "cannot free alien block");
			CA_ASSERT_MSG(ptr_diff(allocator->base, allocator->next) > 0, "stack allocator underflow");

			stackframe_t * frame = reinterpret_cast<stackframe_t *>(allocator->next) - 1;
			
			CA_ASSERT_MSG(ptr_equals(block, frame->block), "stack allocator operations must be filo");

			allocator->next = frame->block_base;
			allocator->free = allocator->size - ptr_diff(allocator->base, allocator->next);
		}
	}
}