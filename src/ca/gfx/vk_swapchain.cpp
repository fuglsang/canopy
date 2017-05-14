#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/math_util.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		static void create_surface(VkSurfaceKHR * surface, vk_device_t * vk_device, sys::window_t * window)
		{
		#if CA_PLATFORM_WIN32
			VkWin32SurfaceCreateInfoKHR win32_surface_create_info;
			win32_surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			win32_surface_create_info.pNext = nullptr;
			win32_surface_create_info.flags = 0;
			win32_surface_create_info.hinstance = GetModuleHandle(nullptr);
			win32_surface_create_info.hwnd = reinterpret_cast<HWND>(window->handle);

			VkResult ret = vkCreateWin32SurfaceKHR(vk_device->instance, &win32_surface_create_info, &vk_device->allocator, surface);
			CA_ASSERT(ret == VK_SUCCESS);
		#else
			#error unknown platform
		#endif
		}

		static void select_surface_format(VkSurfaceFormatKHR * selected_format, vk_device_t * vk_device, VkSurfaceKHR surface, mem::heaparena_t * arena)
		{
			u32 format_count;

			VkResult ret = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device->physical_device, surface, &format_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_ASSERT(format_count > 0);

			VkSurfaceFormatKHR * formats = mem::arena_alloc<VkSurfaceFormatKHR>(arena, format_count);
			{
				ret = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device->physical_device, surface, &format_count, formats);
				CA_ASSERT(ret == VK_SUCCESS);

				if (format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
				{
					selected_format->format = VK_FORMAT_B8G8R8A8_UNORM;
					selected_format->colorSpace = formats[0].colorSpace;
				}
				else
				{
					*selected_format = formats[0];
					
					for (u32 i = 0; i != format_count; i++)
					{
						if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
						{
							selected_format->format = VK_FORMAT_B8G8R8A8_UNORM;
							break;
						}
					}
				}
			}
			mem::arena_free(arena, formats);
		}

		static void select_present_mode(VkPresentModeKHR * selected_mode, vk_device_t * vk_device, VkSurfaceKHR surface, mem::heaparena_t * arena, swapmode mode)
		{
			u32 mode_count;

			VkResult ret = vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device->physical_device, surface, &mode_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_ASSERT(mode_count > 0);

			VkPresentModeKHR * modes = mem::arena_alloc<VkPresentModeKHR>(arena, mode_count);
			{
				ret = vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device->physical_device, surface, &mode_count, modes);
				CA_ASSERT(ret == VK_SUCCESS);

				bool has_mail = false;
				bool has_fifo = false;
				bool has_free = false;

				for (u32 i = 0; i != mode_count; i++)
				{
					switch (modes[i])
					{
					case VK_PRESENT_MODE_IMMEDIATE_KHR:
						has_free = true;
						break;

					case VK_PRESENT_MODE_FIFO_KHR:
						has_fifo = true;
						break;

					case VK_PRESENT_MODE_MAILBOX_KHR:
						has_mail = true;
						break;
					}
				}

				switch (mode)
				{
				case SWAPMODE_FREE:
					CA_ASSERT(has_free);
					*selected_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;

				case SWAPMODE_VSYNC:
					CA_ASSERT(has_fifo);
					*selected_mode = VK_PRESENT_MODE_FIFO_KHR;
					break;

				case SWAPMODE_VSYNC_SKIP:
					CA_ASSERT(has_mail || has_fifo);
					if (has_mail)
						*selected_mode = VK_PRESENT_MODE_MAILBOX_KHR;
					else
						*selected_mode = VK_PRESENT_MODE_FIFO_KHR;
				}
			}
			mem::arena_free(arena, modes);
		}

		void create_swapchain_internal(swapchain_t * swapchain)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			CA_LOG("vulkan_swapchain: verify surface supports present ... ");
			VkBool32 surface_supports_present;
			vkGetPhysicalDeviceSurfaceSupportKHR(vk_device->physical_device, vk_device->queue_family, vk_swapchain->surface, &surface_supports_present);
			CA_ASSERT(surface_supports_present);

			CA_LOG("vulkan_swapchain: select surface format ... ");
			VkSurfaceFormatKHR surface_format;
			select_surface_format(&surface_format, vk_device, vk_swapchain->surface, swapchain->device->arena);

			CA_LOG("vulkan_swapchain: select surface present mode ... ");
			VkPresentModeKHR surface_present_mode;
			select_present_mode(&surface_present_mode, vk_device, vk_swapchain->surface, swapchain->device->arena, swapchain->preferred_mode);

			CA_LOG("vulkan_swapchain: select surface capabilities ... ");
			VkSurfaceCapabilitiesKHR surface_capabilities;
			VkResult ret = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_device->physical_device, vk_swapchain->surface, &surface_capabilities);
			CA_ASSERT(ret == VK_SUCCESS);

			u32 desired_image_count = surface_capabilities.minImageCount + 0;
			if (desired_image_count > surface_capabilities.maxImageCount)
				desired_image_count = surface_capabilities.maxImageCount;

			VkExtent2D desired_image_extents;
			desired_image_extents.width = math::clamp(surface_capabilities.currentExtent.width, surface_capabilities.minImageExtent.width, surface_capabilities.maxImageExtent.width);
			desired_image_extents.height = math::clamp(surface_capabilities.currentExtent.height, surface_capabilities.minImageExtent.height, surface_capabilities.maxImageExtent.height);

			VkImageUsageFlags desired_image_usage = 0;
			desired_image_usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			desired_image_usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			VkSurfaceTransformFlagBitsKHR desired_transform;
			if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
				desired_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
			else
				desired_transform = surface_capabilities.currentTransform;

			VkSwapchainCreateInfoKHR swapchain_create_info;
			swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchain_create_info.pNext = nullptr;
			swapchain_create_info.flags = 0;
			swapchain_create_info.surface = vk_swapchain->surface;
			swapchain_create_info.minImageCount = desired_image_count;
			swapchain_create_info.imageFormat = surface_format.format;
			swapchain_create_info.imageColorSpace = surface_format.colorSpace;
			swapchain_create_info.imageExtent = desired_image_extents;
			swapchain_create_info.imageArrayLayers = 1;
			swapchain_create_info.imageUsage = desired_image_usage;
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_create_info.queueFamilyIndexCount = 0;// only for VK_SHARING_MODE_CONCURRENT
			swapchain_create_info.pQueueFamilyIndices = nullptr;// only for VK_SHARING_MODE_CONCURRENT
			swapchain_create_info.preTransform = desired_transform;
			swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_create_info.presentMode = surface_present_mode;
			swapchain_create_info.clipped = VK_TRUE;
			swapchain_create_info.oldSwapchain = vk_swapchain->swapchain;

			CA_LOG("vulkan_swapchain: create swapchain ... ");
			ret = vkCreateSwapchainKHR(vk_device->device, &swapchain_create_info, &vk_device->allocator, &vk_swapchain->swapchain);
			CA_ASSERT(ret == VK_SUCCESS);
			vk_swapchain->dim_x = desired_image_extents.width;
			vk_swapchain->dim_y = desired_image_extents.height;

			CA_LOG("vulkan_swapchain: create images pointers ... ");
			ret = vkGetSwapchainImagesKHR(vk_device->device, vk_swapchain->swapchain, &swapchain->max_buffers_in_flight, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("max_images_in_flight = %d", swapchain->max_buffers_in_flight);
			vk_swapchain->image_index = 0;
			vk_swapchain->images = mem::arena_alloc<VkImage>(swapchain->device->arena, swapchain->max_buffers_in_flight);
			ret = vkGetSwapchainImagesKHR(vk_device->device, vk_swapchain->swapchain, &swapchain->max_buffers_in_flight, vk_swapchain->images);
			CA_ASSERT(ret == VK_SUCCESS);

			CA_LOG("vulkan_swapchain: create texture objects ... ");
			vk_swapchain->textures = mem::arena_alloc<vk_texture_t>(swapchain->device->arena, swapchain->max_buffers_in_flight);
			for (u32 i = 0; i != swapchain->max_buffers_in_flight; i++)
			{
				vk_swapchain->textures[i].texture = vk_swapchain->images[i];
			}

			CA_LOG("vulkan_swapchain: create fences ... ");
			vk_swapchain->fence_index = 0;
			vk_swapchain->fences = mem::arena_alloc<VkFence>(swapchain->device->arena, swapchain->max_buffers_in_flight);
			for (u32 i = 0; i != swapchain->max_buffers_in_flight; i++)
			{
				VkFenceCreateInfo fence_create_info;
				fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_create_info.pNext = nullptr;
				fence_create_info.flags = 0;

				vkCreateFence(vk_device->device, &fence_create_info, &vk_device->allocator, &vk_swapchain->fences[i]);
			}
		}

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode preferred_mode)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_swapchain_t * vk_swapchain = mem::arena_alloc<vk_swapchain_t>(device->arena, 1);

			vk_swapchain->surface = VK_NULL_HANDLE;
			vk_swapchain->swapchain = VK_NULL_HANDLE;

			CA_LOG("vulkan_swapchain: create surface ... ");
			create_surface(&vk_swapchain->surface, vk_device, window);
			CA_ASSERT(vk_swapchain->surface != VK_NULL_HANDLE);

			swapchain->handle = vk_swapchain;
			swapchain->device = device;
			swapchain->window = window;
			swapchain->max_buffers_in_flight = 0;
			swapchain->preferred_mode = preferred_mode;

			CA_LOG("vulkan_swapchain: enter internal create ... ");
			create_swapchain_internal(swapchain);

			CA_LOG("vulkan_swapchain: READY");
		}

		void destroy_swapchain(swapchain_t * swapchain)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			CA_LOG("vulkan_swapchain: destroy fences ... ");
			for (int i = 0; i != swapchain->max_buffers_in_flight; i++)
			{
				vkDestroyFence(vk_device->device, vk_swapchain->fences[i], &vk_device->allocator);
			}
			mem::arena_free(swapchain->device->arena, vk_swapchain->fences);

			CA_LOG("vulkan_swapchain: destroy texture objects ... ");
			mem::arena_free(swapchain->device->arena, vk_swapchain->textures);

			CA_LOG("vulkan_swapchain: destroy image pointers ... ");
			mem::arena_free(swapchain->device->arena, vk_swapchain->images);

			CA_LOG("vulkan_swapchain: destroy swapchain ... ");
			vkDestroySwapchainKHR(vk_device->device, vk_swapchain->swapchain, &vk_device->allocator);

			CA_LOG("vulkan_swapchain: destroy surface ... ");
			vkDestroySurfaceKHR(vk_device->instance, vk_swapchain->surface, &vk_device->allocator);

			mem::arena_free(swapchain->device->arena, swapchain->handle);
			CA_LOG("vulkan_swapchain: CLEAN");

			swapchain->handle = nullptr;
			swapchain->device = nullptr;
			swapchain->window = nullptr;
			swapchain->max_buffers_in_flight = 0;
			swapchain->preferred_mode = NUM_SWAPMODES;
		}

		void recreate_swapchain(swapchain_t * swapchain)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			CA_LOG("vulkan_swapchain: wait for idle ... ");
			vkDeviceWaitIdle(vk_device->device);

			CA_LOG("vulkan_swapchain: destroy appendices ... ");
			for (int i = 0; i != swapchain->max_buffers_in_flight; i++)
			{
				vkDestroyFence(vk_device->device, vk_swapchain->fences[i], &vk_device->allocator);
			}
			mem::arena_free(swapchain->device->arena, vk_swapchain->fences);
			mem::arena_free(swapchain->device->arena, vk_swapchain->textures);
			mem::arena_free(swapchain->device->arena, vk_swapchain->images);

			CA_LOG("vulkan_swapchain: get old swapchain ... ");
			VkSwapchainKHR old_swapchain = vk_swapchain->swapchain;

			CA_LOG("vulkan_swapchain: enter internal create ... ");
			create_swapchain_internal(swapchain);

			CA_LOG("vulkan_swapchain: destroy old swapchain ... ");
			vkDestroySwapchainKHR(vk_device->device, old_swapchain, &vk_device->allocator);

			CA_LOG("vulkan_swapchain: RECREATED");
		}

		static void recreate_swapchain_if_invalid(swapchain_t * swapchain)
		{
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			bool dx_changed = (vk_swapchain->dim_x != swapchain->window->coords.dim_x);
			bool dy_changed = (vk_swapchain->dim_y != swapchain->window->coords.dim_y);

			if (dx_changed || dy_changed)
			{
				recreate_swapchain(swapchain);
			}
		}

		void swapchain_acquire_blocking(swapchain_t * swapchain, texture_t * texture)
		{
			recreate_swapchain_if_invalid(swapchain);

			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			vk_swapchain->fence_index += 1;
			vk_swapchain->fence_index %= swapchain->max_buffers_in_flight;
			VkFence fence = vk_swapchain->fences[vk_swapchain->fence_index];

			VkResult ret = vkAcquireNextImageKHR(vk_device->device, vk_swapchain->swapchain, UINT64_MAX, VK_NULL_HANDLE, fence, &vk_swapchain->image_index);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkWaitForFences(vk_device->device, 1, &fence, VK_FALSE, UINT64_MAX);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkResetFences(vk_device->device, 1, &fence);
			CA_ASSERT(ret == VK_SUCCESS);

			texture->device = swapchain->device;
			texture->handle = &vk_swapchain->textures[vk_swapchain->image_index];
		}

		void swapchain_acquire(swapchain_t * swapchain, semaphore_t * signal_semaphore, fence_t * signal_fence, texture_t * texture)
		{
			recreate_swapchain_if_invalid(swapchain);

			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			VkResult ret = vkAcquireNextImageKHR(vk_device->device, vk_swapchain->swapchain, UINT64_MAX, resolve_handle(signal_semaphore), resolve_handle(signal_fence), &vk_swapchain->image_index);
			CA_ASSERT(ret == VK_SUCCESS);

			texture->device = swapchain->device;
			texture->handle = &vk_swapchain->textures[vk_swapchain->image_index];
		}

		void swapchain_present(swapchain_t * swapchain, semaphore_t * wait_semaphore)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			VkSemaphore wait = resolve_handle(wait_semaphore);
			u32 wait_count = (wait != VK_NULL_HANDLE) ? 1 : 0;

			VkPresentInfoKHR present_info;
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.pNext = nullptr;
			present_info.waitSemaphoreCount = wait_count;
			present_info.pWaitSemaphores = &wait;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &vk_swapchain->swapchain;
			present_info.pImageIndices = &vk_swapchain->image_index;
			present_info.pResults = nullptr;

			switch (swapchain->preferred_mode)
			{
			case SWAPMODE_VSYNC:
			case SWAPMODE_VSYNC_SKIP:
				sys::window_sync_compositor();
			}

			VkResult ret = vkQueuePresentKHR(vk_device->queue, &present_info);
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif