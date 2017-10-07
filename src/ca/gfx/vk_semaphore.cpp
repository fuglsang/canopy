#if CA_TARGET_GFX_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_semaphore(semaphore_t * semaphore, device_t * device)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_semaphore_t * vk_semaphore = mem::arena_alloc<vk_semaphore_t>(device->arena, 1);

			VkSemaphoreCreateInfo semaphore_create_info;
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphore_create_info.pNext = nullptr;
			semaphore_create_info.flags = 0;

			VkResult ret = vkCreateSemaphore(vk_device->device, &semaphore_create_info, &vk_device->allocator, &vk_semaphore->semaphore);
			CA_ASSERT(ret == VK_SUCCESS);

			semaphore->handle = vk_semaphore;
			semaphore->device = device;
		}

		void destroy_semaphore(semaphore_t * semaphore)
		{
			vk_device_t * vk_device = resolve_type(semaphore->device);
			vk_semaphore_t * vk_semaphore = resolve_type(semaphore);

			vkDestroySemaphore(vk_device->device, vk_semaphore->semaphore, &vk_device->allocator);

			mem::arena_free(semaphore->device->arena, vk_semaphore);

			semaphore->handle = nullptr;
			semaphore->device = nullptr;
		}
	}
}
#endif