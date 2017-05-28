#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct stackallocator_t
		{
			void * base;
			size_t size;
			void * next;
			size_t free;
		};

		void create_allocator(stackallocator_t * allocator, void * base, size_t size);
		void destroy_allocator(stackallocator_t * allocator);

		void * allocator_alloc(stackallocator_t * allocator, size_t size, size_t alignment);
		void allocator_free(stackallocator_t * allocator, void * block);

		typedef arena_t<stackallocator_t, multithreadaccesspolicy_t> stackarena_mt_t;
		typedef arena_t<stackallocator_t, singlethreadaccesspolicy_t> stackarena_t;
	}
}