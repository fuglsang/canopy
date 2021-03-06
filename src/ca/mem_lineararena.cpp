#include "ca/core_assert.h"
#include "ca/mem_lineararena.h"
#include "ca/mem_util.h"

namespace ca
{
	namespace mem
	{
		void create_allocator(linearallocator_t * allocator, void * base, size_t size)
		{
			allocator->base = base;
			allocator->size = size;
			allocator->next = base;
			allocator->free = size;
		}

		void destroy_allocator(linearallocator_t * allocator) {}

		void * allocator_alloc(linearallocator_t * allocator, size_t size, size_t alignment)
		{
			void * block_base = allocator->next;
			void * block = align_up(block_base, alignment);

			size_t req_align = ptr_diff(block_base, block);
			size_t req_total = size + req_align;

			CA_ASSERT_MSG(allocator->free >= req_total, "linear allocator out of memory");

			allocator->next = ptr_add(allocator->next, req_total);
			allocator->free = allocator->size - ptr_diff(allocator->base, allocator->next);

			return block;
		}

		void allocator_free(linearallocator_t * allocator, void * block)
		{
			CA_FATAL("linear allocator does not support free");
		}
	}
}