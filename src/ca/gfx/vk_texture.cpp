#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_texture_internal(vk_texture_t * vk_texture, VkImage image, VkFormat format)
		{
			//TODO decide whether to keep current internal structures
		}

		void create_texture(texture_t * texture, device_t * device)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_texture_t * vk_texture = mem::arena_alloc<vk_texture_t>(device->arena, 1);

			VkImageCreateInfo image_create_info = {};//TODO

			VkResult ret = vkCreateImage(vk_device->device, &image_create_info, &vk_device->allocator, &vk_texture->texture);
			CA_ASSERT(ret == VK_SUCCESS);

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

			ret = vkCreateImageView(vk_device->device, &imageview_create_info, &vk_device->allocator, &vk_texture->view);//TODO
			CA_ASSERT(ret == VK_SUCCESS);

			texture->handle = vk_texture;
			texture->device = device;
		}

		void destroy_texture(texture_t * texture)
		{
			vk_device_t * vk_device = resolve_type(texture->device);
			vk_texture_t * vk_texture = resolve_type(texture);

			vkDestroyImage(vk_device->device, vk_texture->texture, &vk_device->allocator);

			mem::arena_free(texture->device->arena, texture->handle);

			texture->handle = nullptr;
			texture->device = nullptr;
		}
	}
}
#endif