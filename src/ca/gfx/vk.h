#pragma once

#if CA_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#error unknown platform
#endif

#include "ca/gfx.h"

#include <vulkan/vulkan.h>

namespace ca
{
	namespace gfx
	{
		// internal types

		struct vk_buffer_t;
		struct vk_cmdbuffer_t;
		struct vk_cmdpool_t;
		struct vk_device_t;
		struct vk_fence_t;
		struct vk_framebuffer_t;
		struct vk_pipeline_t;
		struct vk_semaphore_t;
		struct vk_shader_t;
		struct vk_swapchain_t;
		struct vk_texture_t;

		struct vk_buffer_t
		{
			VkBuffer buffer;
			VkDeviceMemory memory;
		};

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
			VkDebugReportCallbackEXT debug_callback;
			VkAllocationCallbacks allocator;//TODO allocator per-type? research granularity..
			VkInstance instance;

			VkPhysicalDevice physical_device;
			VkDevice device;
			u32 queue_family;
			VkQueue queue;
		};

		struct vk_fence_t
		{
			VkFence fence;
		};

		struct vk_framebuffer_t
		{
			VkFramebuffer framebuffer;
			VkRenderPass renderpass;
			u32 attachment_count;
			VkClearValue * attachment_clearvalues;
		};

		struct vk_pipeline_t
		{
			VkPipeline pipeline;
			VkPipelineLayout layout;
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
			VkImage * images;
			VkFence * fences;
			u32 fence_index;
		};

		struct vk_texture_t
		{
			VkImage texture;
			VkFormat format;
			VkImageView view;
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

		CA_DEFINE_RESOLVE_VK(buffer);
		CA_DEFINE_RESOLVE_VK(cmdbuffer);
		CA_DEFINE_RESOLVE_VK(cmdpool);
		CA_DEFINE_RESOLVE_VK(device);
		CA_DEFINE_RESOLVE_VK(fence);
		CA_DEFINE_RESOLVE_VK(framebuffer);
		CA_DEFINE_RESOLVE_VK(texture);
		CA_DEFINE_RESOLVE_VK(pipeline);
		CA_DEFINE_RESOLVE_VK(semaphore);
		CA_DEFINE_RESOLVE_VK(shader);
		CA_DEFINE_RESOLVE_VK(swapchain);

		#undef CA_DEFINE_RESOLVE_VK
		#undef CA_DEFINE_RESOLVE_VK_HANDLE
		#undef CA_DEFINE_RESOLVE_VK_TYPE
	}
}