#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/mem_arena.h"

namespace ca
{
	namespace mem
	{
		template <size_t NChunkSize, size_t NChunkAlignment>
		struct chunkallocator_t
		{
			void * base;
			size_t size;

			void * chunk_base;
			size_t chunk_size;

			u32 * free_base;
			u32 * free_head;
			u32 * free_tail;
		};

		template <size_t NChunkSize, size_t NChunkAlignment>
		void create_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * base, size_t size)
		{
			// |---indices---|-------------chunk-------------|---|------------->>>
			// ^base         ^chunkalign-------------------------^chunkalign--->>>

			void * aligned_base = align_up(base, NChunkAlignment);
			size_t aligned_size = align_up(NChunkSize, NChunkAlignment);
			size_t aligned_area = aligned_size * ((size - ptr_diff(base, aligned_base)) / aligned_size);

			/*
			totalarea = N * sizeindex + N * sizechunk
			totalarea = N * (sizeindex + sizechunk)
			N = totalarea / (sizeindex + sizechunk)
			*/

			size_t index_size = sizeof(u32);
			u32 aligned_count = aligned_area / (index_size + aligned_size);
			void * chunk_base = align_up(ptr_add(base, aligned_count + index_size), NChunkAlignment);

			allocator->base = base;
			allocator->size = size;

			allocator->chunk_base = chunk_base;
			allocator->chunk_size = aligned_size;

			allocator->free_base = base;
			allocator->free_head = allocator->free;
			allocator->free_tail = allocator->free + aligned_count;

			for (u32 i = 0; i != count; i++)
			{
				allocator->free_base[i] = i;
			}
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void destroy_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator) {}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void * allocator_alloc(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, size_t size, size_t alignment)
		{
			CA_ASSERT_MSG(allocator->free_head < allocator->free_tail, "chunk allocator out of memory");
			CA_ASSERT_MSG(NChunkAlignment == alignment, "chunk allocator unfit alignment %d", alignment);
			CA_ASSERT_MSG(NChunkSize >= size, "chunk allocator unfit size %d", size);

			u32 chunk_index = *(allocator->free_head++);

			void * block = ptr_add(allocator->chunk_base, allocator->chunk_size * chunk_index);
			return block;
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void allocator_free(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * block)
		{
			CA_ASSERT_MSG(allocator->free_head > allocator->free_base, "chunk allocator underflow");

			u32 chunk_index = ptr_diff(allocator->chunk_base, block) / allocator->chunk_size;

			allocator->index_free[--allocator->index_cursor] = chunk_index;
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		using chunkarena_mt_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, core::multithreadpolicy_t>;
		template <size_t NChunkSize, size_t NChunkAlignment>
		using chunkarena_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, core::singlethreadpolicy_t>;
	}
}