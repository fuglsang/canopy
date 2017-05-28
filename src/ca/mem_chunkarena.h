#pragma once

#include "ca/mem_arena.h"
#include "ca/types.h"

namespace ca
{
	namespace mem
	{
		template <u32 NChunkSize, u32 NChunkAlignment>
		struct chunkallocator_t
		{
			void * base;
			size_t size;
		};

		template <u32 NChunkSize, u32 NChunkAlignment>
		void create_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * base, size_t size)
		{
			allocator->base = base;
			allocator->size = size;
			CA_FATAL("TODO");
		}

		template <u32 NChunkSize, u32 NChunkAlignment>
		void destroy_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator) {}

		template <u32 NChunkSize, u32 NChunkAlignment>
		void * allocator_alloc(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, size_t size, size_t alignment)
		{
			CA_FATAL("TODO");
		}

		template <u32 NChunkSize, u32 NChunkAlignment>
		void allocator_free(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * block)
		{
			CA_FATAL("TODO");
		}

		template <u32 NChunkSize, u32 NChunkAlignment>
		using chunkarena_mt_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, multithreadaccesspolicy_t>;
		template <u32 NChunkSize, u32 NChunkAlignment>
		using chunkarena_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, singlethreadaccesspolicy_t>;
	}
}