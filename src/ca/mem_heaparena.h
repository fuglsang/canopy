#pragma once

#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		struct heapallocator_t
		{
			void * base;
			size_t size;
			void * handle;
		};

		void create_allocator(heapallocator_t * allocator, void * base, size_t size);
		void destroy_allocator(heapallocator_t * allocator);

		void * allocator_alloc(heapallocator_t * allocator, size_t size, size_t alignment);
		void allocator_free(heapallocator_t * allocator, void * block);

		typedef arena_t<heapallocator_t, core::multithreadpolicy_t> heaparena_mt_t;
		typedef arena_t<heapallocator_t, core::singlethreadpolicy_t> heaparena_t;
	}
}