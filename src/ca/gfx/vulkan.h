#pragma once

#include <vulkan/vulkan.h>

namespace ca
{
	namespace gfx
	{
		template <typename A>
		struct vulkan_allocation_callbacks
		{
			static void * VKAPI_PTR alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				return arena_alloc(arena, size, alignment);
			}

			static void * VKAPI_PTR realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				//TODO
				CA_LOG("realloc missing impl");
				return nullptr;
			}

			static void VKAPI_PTR free(void * pUserData, void * memory)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				arena_free(arena, memory);
			}
		};

		struct vulkan_device_t
		{
			VkAllocationCallbacks allocator;
			VkInstance instance;
			VkDevice device;
		};

		inline vulkan_device_t * resolve_device(void * device)
		{
			return reinterpret_cast<vulkan_device_t *>(device);
		}
	}
}