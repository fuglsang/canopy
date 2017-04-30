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
		void create_semaphore(semaphore_t * semaphore, device_t * device)
		{
			vk_device_t * vk_device = resolve_device(device);
			vk_semaphore_t * vk_semaphore = mem::arena_alloc<vk_semaphore_t>(device->arena, 1);

			VkSemaphoreCreateFlags semaphore_create_flags;
			semaphore_create_flags = 0;

			VkSemaphoreCreateInfo semaphore_create_info;
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphore_create_info.pNext = nullptr;
			semaphore_create_info.flags = semaphore_create_flags;

			VkResult ret = vkCreateSemaphore(vk_device->device, &semaphore_create_info, &vk_device->allocator, &vk_semaphore->semaphore);
			CA_ASSERT(ret == VK_SUCCESS);

			semaphore->handle = vk_semaphore;
			semaphore->device = device;
		}

		void destroy_fence(semaphore_t * semaphore)
		{
			vk_device_t * vk_device = resolve_device(semaphore->device);
			vk_semaphore_t * vk_semaphore = resolve_semaphore(semaphore);

			vkDestroySemaphore(vk_device->device, vk_semaphore->semaphore, &vk_device->allocator);

			mem::arena_free(semaphore->device->arena, semaphore->handle);

			semaphore->handle = nullptr;
			semaphore->device = nullptr;
		}
	}
}
#endif