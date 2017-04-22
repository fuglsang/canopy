#pragma once

#if CA_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error unknown platform
#endif

#include "ca/gfx_device.h"
#include "ca/gfx_swapchain.h"

#include <vulkan/vulkan.h>

namespace ca
{
	namespace gfx
	{
		template <typename A>
		struct vk_allocation_callbacks
		{
			static void * VKAPI_PTR alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				return arena_alloc(arena, size, alignment);
			}

			static void * VKAPI_PTR realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				CA_ASSERT_MSG(false, "not implemented: realloc");
				return nullptr;
			}

			static void VKAPI_PTR free(void * pUserData, void * memory)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				arena_free(arena, memory);
			}
		};

		struct vk_device_t
		{
			VkAllocationCallbacks allocator;
			VkInstance instance;
			VkPhysicalDevice physical_device;
			VkDevice device;
			VkQueue queue;
		};

		struct vk_swapchain_t
		{
			VkSurfaceKHR surface;
			VkSwapchainKHR swapchain;
		};

		inline vk_device_t * resolve_device(device_t * device)
		{
			return reinterpret_cast<vk_device_t *>(device->handle);
		}

		inline vk_swapchain_t * resolve_swapchain(swapchain_t * swapchain)
		{
			return reinterpret_cast<vk_swapchain_t *>(swapchain->handle);
		}
	}
}