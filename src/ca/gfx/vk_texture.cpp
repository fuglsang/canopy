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
		void create_texture(texture_t * texture, device_t * device)
		{
			vk_device_t * vk_device = resolve_device(device);
			vk_texture_t * vk_texture = mem::arena_alloc<vk_texture_t>(device->arena, 1);

			VkImageCreateInfo image_create_info = {};//TODO

			VkResult ret = vkCreateImage(vk_device->device, &image_create_info, &vk_device->allocator, &vk_texture->image);
			CA_ASSERT(ret == VK_SUCCESS);

			texture->handle = vk_texture;
			texture->device = device;
		}

		void destroy_texture(texture_t * texture)
		{
			vk_device_t * vk_device = resolve_device(texture->device);
			vk_texture_t * vk_texture = resolve_texture(texture);

			vkDestroyImage(vk_device->device, vk_texture->image, &vk_device->allocator);

			mem::arena_free(texture->device->arena, texture->handle);

			texture->handle = nullptr;
			texture->device = nullptr;
		}
	}
}
#endif