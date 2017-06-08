#include "ca/core_assert.h"
#include "ca/mem_heaparena.h"
#include "ca/mem_util.h"

#include <tlsf/tlsf.h>

namespace ca
{
	namespace mem
	{
		void create_allocator(heapallocator_t * allocator, void * base, size_t size)
		{
			allocator->base = base;
			allocator->size = size;
			allocator->handle = tlsf_create_with_pool(base, size);
		}

		void destroy_allocator(heapallocator_t * allocator)
		{
			tlsf_destroy(allocator->handle);
		}

		void * allocator_alloc(heapallocator_t * allocator, size_t size, size_t alignment)
		{
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
			void * block = tlsf_memalign(allocator->handle, alignment, size);
			CA_ASSERT_MSG(block != nullptr, "heap allocator out of memory");
			return block;
		}

		void allocator_free(heapallocator_t * allocator, void * block)
		{
			tlsf_free(allocator->handle, block);
		}
	}
}