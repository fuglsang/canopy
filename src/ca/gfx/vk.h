#pragma once

#if CA_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error unknown platform
#endif

#include "ca/gfx_device.h"
#include "ca/gfx_semaphore.h"
#include "ca/gfx_swapchain.h"
#include "ca/gfx_cmdbuffer.h"
#include "ca/gfx_texture.h"
#include "ca/gfx_fence.h"

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
				CA_ERROR("realloc not implemented");
				sys::breakpoint();
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

			u32 queue_family;
			VkQueue queue;

			//TODO split queues?
			//VkQueue queue_graphics;
			//VkQueue queue_transfer;
			//VkQueue queue_compute;

			VkCommandPool cmdpool[NUM_CMDBUFFERTYPES];
		};

		struct vk_fence_t
		{
			VkFence fence;
		};

		struct vk_semaphore_t
		{
			VkSemaphore semaphore;
		};

		struct vk_texture_t
		{
			VkImage image;
			VkImageLayout image_layout;
		};

		struct vk_swapchain_t
		{
			VkSurfaceKHR surface;
			VkSwapchainKHR swapchain;

			u32 image_count;
			u32 image_index;
			VkImage * image_array;

			u32 fence_index;
			VkFence * fence_array;
		};

		struct vk_cmdbuffer_t
		{
			VkCommandBuffer cmdbuffer;
		};

		inline vk_device_t * resolve_device(device_t * device)
		{
			return reinterpret_cast<vk_device_t *>(device->handle);
		}

		inline vk_semaphore_t * resolve_semaphore(semaphore_t * semaphore)
		{
			return reinterpret_cast<vk_semaphore_t *>(semaphore->handle);
		}

		inline vk_swapchain_t * resolve_swapchain(swapchain_t * swapchain)
		{
			return reinterpret_cast<vk_swapchain_t *>(swapchain->handle);
		}

		inline vk_cmdbuffer_t * resolve_cmdbuffer(cmdbuffer_t * cmdbuffer)
		{
			return reinterpret_cast<vk_cmdbuffer_t *>(cmdbuffer->handle);
		}

		inline vk_texture_t * resolve_texture(texture_t * cmdbuffer)
		{
			return reinterpret_cast<vk_texture_t *>(cmdbuffer->handle);
		}

		inline vk_fence_t * resolve_fence(fence_t * fence)
		{
			return reinterpret_cast<vk_fence_t *>(fence->handle);
		}
	}
}