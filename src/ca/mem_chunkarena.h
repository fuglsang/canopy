#pragma once

#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/mem_arena.h"
#include "ca/mem_util.h"

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

			u32 * index_base;
			u32 * index_head;
			u32 * index_tail;
		};

		template <size_t NChunkSize, size_t NChunkAlignment>
		void create_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * base, size_t size)
		{
			// |---indices---|-------------chunk-------------|---|------------->>>
			// ^base         ^chunkalign-------------------------^chunkalign--->>>

			void * aligned_base = align_up(base, NChunkAlignment);
			size_t aligned_size = align_up(NChunkSize, NChunkAlignment);
			size_t aligned_area = aligned_size * ((size - ptr_diff(base, aligned_base)) / aligned_size);

			// totalarea = N * sizeindex + N * sizechunk
			// totalarea = N * (sizeindex + sizechunk)
			// N = totalarea / (sizeindex + sizechunk)

			size_t index_size = sizeof(u32);
			u32 aligned_count = aligned_area / (index_size + aligned_size);
			void * chunk_base = align_up(ptr_add(base, aligned_count + index_size), NChunkAlignment);

			CA_LOG("index_size %d, aligned_size %d, aligned_area %d, aligned_count %d", index_size, aligned_size, aligned_area, aligned_count);

			allocator->base = base;
			allocator->size = size;

			allocator->chunk_base = chunk_base;
			allocator->chunk_size = aligned_size;

			allocator->index_base = reinterpret_cast<u32 *>(base);
			allocator->index_head = allocator->index_base;
			allocator->index_tail = allocator->index_base + aligned_count;

			for (u32 i = 0; i != aligned_count; i++)
			{
				allocator->index_base[i] = i;
			}
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void destroy_allocator(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator) {}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void * allocator_alloc(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, size_t size, size_t alignment)
		{
			CA_ASSERT_MSG(allocator->index_head < allocator->index_tail, "chunk allocator out of memory");
			CA_ASSERT_MSG(alignment <= NChunkAlignment, "alignment must be <= %d", NChunkAlignment);
			CA_ASSERT_MSG(align_ispow2(alignment), "alignment must be power of 2");
			CA_ASSERT_MSG(size <= NChunkSize, "size must be <= %d", NChunkSize);

			u32 chunk_index = *(allocator->index_head++);

			void * block = ptr_add(allocator->chunk_base, allocator->chunk_size * chunk_index);
			return block;
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		void allocator_free(chunkallocator_t<NChunkSize, NChunkAlignment> * allocator, void * block)
		{
			CA_ASSERT_MSG(allocator->index_head > allocator->index_base, "chunk allocator underflow");

			ptrdiff_t chunk_offset = ptr_diff(allocator->chunk_base, block);
			CA_ASSERT_MSG((chunk_offset % allocator->chunk_size) == 0, "cannot free alien block");
			
			u32 chunk_index = chunk_offset / allocator->chunk_size;

			*(--allocator->index_head) = chunk_index;
		}

		template <size_t NChunkSize, size_t NChunkAlignment>
		using chunkarena_mt_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, core::multithreadpolicy_t>;
		template <size_t NChunkSize, size_t NChunkAlignment>
		using chunkarena_t = arena_t<chunkallocator_t<NChunkSize, NChunkAlignment>, core::singlethreadpolicy_t>;
	}
}