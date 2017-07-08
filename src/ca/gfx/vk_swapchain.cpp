#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/math_util.h"
#include "ca/mem.h"
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

		static void select_surface_format(VkSurfaceFormatKHR * selected_format, vk_device_t * vk_device, VkSurfaceKHR surface)
		{
			u32 format_count;

			VkResult ret = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_device->physical_device, surface, &format_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_ASSERT(format_count > 0);

			VkSurfaceFormatKHR * formats = mem::arena_alloc<VkSurfaceFormatKHR>(CA_APP_STACK, format_count);
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
			mem::arena_free(CA_APP_STACK, formats);
		}

		static void select_present_mode(VkPresentModeKHR * selected_mode, vk_device_t * vk_device, VkSurfaceKHR surface, swapmode preferred_swapmode)
		{
			u32 mode_count;

			VkResult ret = vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device->physical_device, surface, &mode_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_ASSERT(mode_count > 0);

			VkPresentModeKHR * modes = mem::arena_alloc<VkPresentModeKHR>(CA_APP_STACK, mode_count);
			{
				ret = vkGetPhysicalDeviceSurfacePresentModesKHR(vk_device->physical_device, surface, &mode_count, modes);
				CA_ASSERT(ret == VK_SUCCESS);

				bool selected = false;
				
				for (u32 i = 0; i != mode_count; i++)
				{
					switch (modes[i])
					{
					case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
						selected = (preferred_swapmode == SWAPMODE_FREE);
						break;
					case VK_PRESENT_MODE_FIFO_KHR:
						selected = (preferred_swapmode == SWAPMODE_VSYNC);
						break;
					case VK_PRESENT_MODE_MAILBOX_KHR:
						selected = (preferred_swapmode == SWAPMODE_VSYNC_SKIP);
						break;
					}

					if (selected)
					{
						*selected_mode = modes[i];
						break;
					}
				}

				if (!selected)
				{
					CA_WARN("preferred swapmode not supported... using VK_PRESENT_MODE_FIFO_KHR as fallback");
					*selected_mode = VK_PRESENT_MODE_FIFO_KHR;
				}
			}
			mem::arena_free(CA_APP_STACK, modes);
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
			select_surface_format(&surface_format, vk_device, vk_swapchain->surface);

			CA_LOG("vulkan_swapchain: select surface present mode ... ");
			VkPresentModeKHR surface_present_mode;
			select_present_mode(&surface_present_mode, vk_device, vk_swapchain->surface, swapchain->preferred_swapmode);

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
			desired_image_usage |= VK_IMAGE_USAGE_STORAGE_BIT;

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
			swapchain_create_info.queueFamilyIndexCount = 0;// ignored due to VK_SHARING_MODE_EXCLUSIVE
			swapchain_create_info.pQueueFamilyIndices = nullptr;// ignored due to VK_SHARING_MODE_EXCLUSIVE
			swapchain_create_info.preTransform = desired_transform;
			swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_create_info.presentMode = surface_present_mode;
			swapchain_create_info.clipped = VK_TRUE;
			swapchain_create_info.oldSwapchain = vk_swapchain->swapchain;

			CA_LOG("vulkan_swapchain: create swapchain ... ");
			ret = vkCreateSwapchainKHR(vk_device->device, &swapchain_create_info, &vk_device->allocator, &vk_swapchain->swapchain);
			CA_ASSERT(ret == VK_SUCCESS);
			swapchain->width = desired_image_extents.width;
			swapchain->height = desired_image_extents.height;

			CA_LOG("vulkan_swapchain: create image array ... ");
			ret = vkGetSwapchainImagesKHR(vk_device->device, vk_swapchain->swapchain, &swapchain->length, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("length = %d", swapchain->length);
			vk_swapchain->images = mem::arena_alloc<VkImage>(swapchain->device->arena, swapchain->length);
			ret = vkGetSwapchainImagesKHR(vk_device->device, vk_swapchain->swapchain, &swapchain->length, vk_swapchain->images);
			CA_ASSERT(ret == VK_SUCCESS);

			CA_LOG("vulkan_swapchain: create fences ... ");
			vk_swapchain->fence_index = 0;
			vk_swapchain->fences = mem::arena_alloc<VkFence>(swapchain->device->arena, swapchain->length);
			for (u32 i = 0; i != swapchain->length; i++)
			{
				VkFenceCreateInfo fence_create_info;
				fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_create_info.pNext = nullptr;
				fence_create_info.flags = 0;
				
				vkCreateFence(vk_device->device, &fence_create_info, &vk_device->allocator, &vk_swapchain->fences[i]);
			}

			CA_LOG("vulkan_swapchain: create texture objects ... ");
			swapchain->textures = mem::arena_alloc<texture_t>(swapchain->device->arena, swapchain->length);
			for (u32 i = 0; i != swapchain->length; i++)
			{
				vk_texture_t * vk_texture = mem::arena_alloc<vk_texture_t>(swapchain->device->arena, 1);

				vk_texture->texture = vk_swapchain->images[i];
				vk_texture->format = surface_format.format;

				VkComponentMapping component_mapping;
				component_mapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				component_mapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				component_mapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				component_mapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				VkImageSubresourceRange image_subresource_range;
				image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				image_subresource_range.baseMipLevel = 0;
				image_subresource_range.levelCount = 1;
				image_subresource_range.baseArrayLayer = 0;
				image_subresource_range.layerCount = 1;

				VkImageViewCreateInfo imageview_create_info;
				imageview_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				imageview_create_info.pNext = nullptr;
				imageview_create_info.flags = 0;
				imageview_create_info.image = vk_texture->texture;
				imageview_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
				imageview_create_info.format = vk_texture->format;
				imageview_create_info.components = component_mapping;
				imageview_create_info.subresourceRange = image_subresource_range;

				ret = vkCreateImageView(vk_device->device, &imageview_create_info, &vk_device->allocator, &vk_texture->view);
				CA_ASSERT(ret == VK_SUCCESS);

				texture_t * texture = &swapchain->textures[i];
				texture->handle = vk_texture;
				texture->device = swapchain->device;
				texture->format = NUM_TEXTUREFORMATS;//TODO backwards resolve
				texture->width = swapchain->width;
				texture->height = swapchain->height;
			}
		}

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode preferred_swapmode)
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
			swapchain->preferred_swapmode = preferred_swapmode;
			swapchain->length = 0;

			CA_LOG("vulkan_swapchain: enter internal create ... ");
			create_swapchain_internal(swapchain);

			CA_LOG("vulkan_swapchain: create events ... ");
			core::create_event(&swapchain->recreated);

			CA_LOG("vulkan_swapchain: READY");
		}

		void destroy_swapchain(swapchain_t * swapchain)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			CA_LOG("vulkan_swapchain: destroy events ... ");
			core::destroy_event(&swapchain->recreated);

			CA_LOG("vulkan_swapchain: destroy texture objects ... ");
			for (u32 i = 0; i != swapchain->length; i++)
			{
				vk_texture_t * vk_texture = resolve_type(&swapchain->textures[i]);
				vkDestroyImageView(vk_device->device, vk_texture->view, &vk_device->allocator);
				mem::arena_free(swapchain->device->arena, vk_texture);
			}
			mem::arena_free(swapchain->device->arena, swapchain->textures);

			CA_LOG("vulkan_swapchain: destroy fences ... ");
			for (u32 i = 0; i != swapchain->length; i++)
			{
				vkDestroyFence(vk_device->device, vk_swapchain->fences[i], &vk_device->allocator);
			}
			mem::arena_free(swapchain->device->arena, vk_swapchain->fences);

			CA_LOG("vulkan_swapchain: destroy image array ... ");
			mem::arena_free(swapchain->device->arena, vk_swapchain->images);

			CA_LOG("vulkan_swapchain: destroy swapchain ... ");
			vkDestroySwapchainKHR(vk_device->device, vk_swapchain->swapchain, &vk_device->allocator);

			CA_LOG("vulkan_swapchain: destroy surface ... ");
			vkDestroySurfaceKHR(vk_device->instance, vk_swapchain->surface, &vk_device->allocator);

			mem::arena_free(swapchain->device->arena, vk_swapchain);
			CA_LOG("vulkan_swapchain: CLEAN");

			swapchain->handle = nullptr;
			swapchain->device = nullptr;
			swapchain->window = nullptr;
			swapchain->preferred_swapmode = NUM_SWAPMODES;
			swapchain->length = 0;
		}

		void recreate_swapchain(swapchain_t * swapchain)
		{
			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			CA_LOG("vulkan_swapchain: wait for idle ... ");
			vkDeviceWaitIdle(vk_device->device);

			CA_LOG("vulkan_swapchain: destroy texture objects ... ");
			for (u32 i = 0; i != swapchain->length; i++)
			{
				vk_texture_t * vk_texture = resolve_type(&swapchain->textures[i]);
				vkDestroyImageView(vk_device->device, vk_texture->view, &vk_device->allocator);
				mem::arena_free(swapchain->device->arena, vk_texture);
			}
			mem::arena_free(swapchain->device->arena, swapchain->textures);

			CA_LOG("vulkan_swapchain: destroy fences ... ");
			for (u32 i = 0; i != swapchain->length; i++)
			{
				vkDestroyFence(vk_device->device, vk_swapchain->fences[i], &vk_device->allocator);
			}
			mem::arena_free(swapchain->device->arena, vk_swapchain->fences);

			CA_LOG("vulkan_swapchain: destroy image array ... ");
			mem::arena_free(swapchain->device->arena, vk_swapchain->images);

			CA_LOG("vulkan_swapchain: get old swapchain ... ");
			VkSwapchainKHR old_swapchain = vk_swapchain->swapchain;

			CA_LOG("vulkan_swapchain: enter internal create ... ");
			create_swapchain_internal(swapchain);

			CA_LOG("vulkan_swapchain: destroy old swapchain ... ");
			vkDestroySwapchainKHR(vk_device->device, old_swapchain, &vk_device->allocator);

			CA_LOG("vulkan_swapchain: RECREATED");
			core::event_dispatch(&swapchain->recreated, swapchain);
		}

		static void recreate_swapchain_if_invalid(swapchain_t * swapchain)
		{
			bool invalid_width = (swapchain->width != swapchain->window->rect.width);
			bool invalid_height = (swapchain->height != swapchain->window->rect.height);
			
			if (invalid_width || invalid_height)
			{
				recreate_swapchain(swapchain);
			}
		}

		void swapchain_acquire_blocking(swapchain_t * swapchain, u32 * acquired_index)
		{
			recreate_swapchain_if_invalid(swapchain);

			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			vk_swapchain->fence_index += 1;
			vk_swapchain->fence_index %= swapchain->length;
			VkFence fence = vk_swapchain->fences[vk_swapchain->fence_index];

			VkResult ret = vkAcquireNextImageKHR(vk_device->device, vk_swapchain->swapchain, UINT64_MAX, VK_NULL_HANDLE, fence, acquired_index);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkWaitForFences(vk_device->device, 1, &fence, VK_FALSE, UINT64_MAX);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkResetFences(vk_device->device, 1, &fence);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void swapchain_acquire(swapchain_t * swapchain, semaphore_t * signal_semaphore, fence_t * signal_fence, u32 * acquired_index)
		{
			recreate_swapchain_if_invalid(swapchain);

			vk_device_t * vk_device = resolve_type(swapchain->device);
			vk_swapchain_t * vk_swapchain = resolve_type(swapchain);

			VkResult ret = vkAcquireNextImageKHR(vk_device->device, vk_swapchain->swapchain, UINT64_MAX, resolve_handle(signal_semaphore), resolve_handle(signal_fence), acquired_index);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void swapchain_present(swapchain_t * swapchain, semaphore_t * wait_semaphore, u32 acquired_index)
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
			present_info.pImageIndices = &acquired_index;
			present_info.pResults = nullptr;

			VkResult ret = vkQueuePresentKHR(vk_device->queue, &present_info);
			CA_ASSERT(ret == VK_SUCCESS);

			switch (swapchain->preferred_swapmode)
			{
			case SWAPMODE_VSYNC:
			case SWAPMODE_VSYNC_SKIP:
				sys::window_sync_compositor();
			}
		}
	}
}
#endif