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
		VkFormat resolve_format(textureformat format)
		{
			switch (format)
			{
				// depth formats
			case TEXTUREFORMAT_DEPTH16:
				return VK_FORMAT_D16_UNORM;
			case TEXTUREFORMAT_DEPTH32F:
				return VK_FORMAT_D32_SFLOAT;
			case TEXTUREFORMAT_DEPTH24_STENCIL8:
				return VK_FORMAT_D24_UNORM_S8_UINT;

				// color formats
			case TEXTUREFORMAT_R8:
				return VK_FORMAT_R8_UNORM;
			case TEXTUREFORMAT_RG8:
				return VK_FORMAT_R8G8_UNORM;
			case TEXTUREFORMAT_RGB8:
				return VK_FORMAT_R8G8B8_UNORM;
			case TEXTUREFORMAT_RGBA8:
				return VK_FORMAT_R8G8B8A8_UNORM;

				// float formats
			case TEXTUREFORMAT_R16F:
				return VK_FORMAT_R16_SFLOAT;
			case TEXTUREFORMAT_RG16F:
				return VK_FORMAT_R16G16B16A16_SFLOAT;

				// ...
			default:
				CA_FATAL("unsupported textureformat");
				return VK_FORMAT_UNDEFINED;
			}
		}

		VkImageUsageFlags resolve_usage_flags(textureformat format)
		{
			switch (format)
			{
				// depth formats
			case TEXTUREFORMAT_DEPTH16:
			case TEXTUREFORMAT_DEPTH32F:
			case TEXTUREFORMAT_DEPTH24_STENCIL8:
				return VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

				// color formats
			case TEXTUREFORMAT_R8:
			case TEXTUREFORMAT_RG8:
			case TEXTUREFORMAT_RGB8:
			case TEXTUREFORMAT_RGBA8:
				return VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				// float formats
			case TEXTUREFORMAT_R16F:
			case TEXTUREFORMAT_RG16F:
				return VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

				// ...
			default:
				CA_FATAL("unsupported textureformat");
				return 0;
			}
		}

		void create_texture(texture_t * texture, device_t * device, textureformat format, u32 width, u32 height)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_texture_t * vk_texture = mem::arena_alloc<vk_texture_t>(device->arena, 1);

			VkImageCreateInfo image_create_info = {};//TODO
			image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_create_info.pNext = nullptr;
			image_create_info.flags = 0;
			image_create_info.imageType = VK_IMAGE_TYPE_2D;
			image_create_info.format = resolve_format(format);
			image_create_info.extent = { width, height, 1 };
			image_create_info.mipLevels = 1;
			image_create_info.arrayLayers = 1;
			image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_create_info.usage = resolve_usage_flags(format);
			image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			image_create_info.queueFamilyIndexCount = 0;// ignored due to VK_SHARING_MODE_EXCLUSIVE
			image_create_info.pQueueFamilyIndices = nullptr;// ignored due to VK_SHARING_MODE_EXCLUSIVE
			image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VkResult ret = vkCreateImage(vk_device->device, &image_create_info, &vk_device->allocator, &vk_texture->texture);
			CA_ASSERT(ret == VK_SUCCESS);

			//TODO allocate memory
			//bind image memory

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

			texture->handle = vk_texture;
			texture->device = device;
			texture->format = format;
			texture->width = width;
			texture->height = height;
		}

		void destroy_texture(texture_t * texture)
		{
			vk_device_t * vk_device = resolve_type(texture->device);
			vk_texture_t * vk_texture = resolve_type(texture);

			vkDestroyImageView(vk_device->device, vk_texture->view, &vk_device->allocator);
			vkDestroyImage(vk_device->device, vk_texture->texture, &vk_device->allocator);

			mem::arena_free(texture->device->arena, vk_texture);

			texture->handle = nullptr;
			texture->device = nullptr;
			texture->format = NUM_TEXTUREFORMATS;
			texture->width = 0;
			texture->height = 0;
		}
	}
}
#endif