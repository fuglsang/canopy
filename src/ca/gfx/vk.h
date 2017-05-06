#pragma once

#if CA_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error unknown platform
#endif

#include "ca/gfx_cmdbuffer.h"
#include "ca/gfx_cmdpool.h"
#include "ca/gfx_device.h"
#include "ca/gfx_fence.h"
#include "ca/gfx_semaphore.h"
#include "ca/gfx_shader.h"
#include "ca/gfx_swapchain.h"
#include "ca/gfx_texture.h"

#include <vulkan/vulkan.h>

namespace ca
{
	namespace gfx
	{
		// vk callbacks

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

		struct vk_debug_callbacks
		{
			static VkBool32 VKAPI_PTR report(
				VkDebugReportFlagsEXT                       flags,
				VkDebugReportObjectTypeEXT                  objectType,
				uint64_t                                    object,
				size_t                                      location,
				int32_t                                     messageCode,
				const char*                                 pLayerPrefix,
				const char*                                 pMessage,
				void*                                       pUserData)
			{
				CA_LOG("%s", pMessage);
				return VK_FALSE;
			}
		};

		// vk_... types

		struct vk_cmdbuffer_t;
		struct vk_cmdpool_t;
		struct vk_device_t;
		struct vk_fence_t;
		struct vk_semaphore_t;
		struct vk_shader_t;
		struct vk_swapchain_t;
		struct vk_texture_t;

		struct vk_cmdbuffer_t
		{
			VkCommandBuffer cmdbuffer;
		};

		struct vk_cmdpool_t
		{
			VkCommandPool cmdpool;
		};

		struct vk_device_t
		{
			VkAllocationCallbacks allocator;//TODO allocator per-type? research granularity..
			VkInstance instance;

			VkDebugReportCallbackEXT debug_callback;

			VkPhysicalDevice physical_device;
			VkDevice device;

			u32 queue_family;
			VkQueue queue;
		};

		struct vk_fence_t
		{
			VkFence fence;
		};

		struct vk_semaphore_t
		{
			VkSemaphore semaphore;
		};

		struct vk_shader_t
		{
			VkShaderModule shader;
		};

		struct vk_swapchain_t
		{
			VkSurfaceKHR surface;
			VkSwapchainKHR swapchain;

			u32 image_index;
			VkImage * images;
			u32 fence_index;
			VkFence * fences;

			vk_texture_t * textures;
		};

		struct vk_texture_t
		{
			VkImage texture;
		};

		// resolve_type(), resolve_handle()

		#define CA_DEFINE_RESOLVE_VK_TYPE(ID)							\
		inline vk_##ID##_t * resolve_type(ID##_t * ID)					\
		{																\
			return reinterpret_cast<vk_##ID##_t *>(ID->handle);			\
		}

		#define CA_DEFINE_RESOLVE_VK_HANDLE(ID)							\
		inline decltype(vk_##ID##_t::##ID) resolve_handle(ID##_t * ID)	\
		{																\
			if (ID != nullptr)											\
				return resolve_type(ID)->ID;							\
			else														\
				return VK_NULL_HANDLE;									\
		}

		#define CA_DEFINE_RESOLVE_VK(ID) CA_DEFINE_RESOLVE_VK_TYPE(ID); CA_DEFINE_RESOLVE_VK_HANDLE(ID)

		CA_DEFINE_RESOLVE_VK(cmdbuffer);
		CA_DEFINE_RESOLVE_VK(cmdpool);
		CA_DEFINE_RESOLVE_VK(device);
		CA_DEFINE_RESOLVE_VK(fence);
		CA_DEFINE_RESOLVE_VK(texture);
		CA_DEFINE_RESOLVE_VK(semaphore);
		CA_DEFINE_RESOLVE_VK(shader);
		CA_DEFINE_RESOLVE_VK(swapchain);

		#undef CA_DEFINE_RESOLVE_VK
		#undef CA_DEFINE_RESOLVE_VK_HANDLE
		#undef CA_DEFINE_RESOLVE_VK_TYPE
	}
}

