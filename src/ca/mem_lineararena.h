#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct linearallocator_t
		{
			void * base;
			size_t size;
			size_t used;
			size_t free;
		};

		void create_allocator(linearallocator_t * allocator, void * base, size_t size);
		void destroy_allocator(linearallocator_t * allocator);

		void * allocator_alloc(linearallocator_t * allocator, size_t size, size_t alignment);
		void allocator_free(linearallocator_t * allocator, void * block);

		typedef arena_t<linearallocator_t, multithreadaccesspolicy_t> lineararena_mt_t;
		typedef arena_t<linearallocator_t, singlethreadaccesspolicy_t> lineararena_t;
	}
}