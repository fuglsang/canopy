#if CA_TARGET_GFX_VULKAN
#include "ca_base.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_cmdpool(cmdpool_t * cmdpool, device_t * device)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_cmdpool_t * vk_cmdpool = mem::arena_alloc<vk_cmdpool_t>(device->arena, 1);

			VkCommandPoolCreateFlags cmdpool_create_flags = 0;
			cmdpool_create_flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

			VkCommandPoolCreateInfo cmdpool_create_info;
			cmdpool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			cmdpool_create_info.pNext = nullptr;
			cmdpool_create_info.flags = cmdpool_create_flags;
			cmdpool_create_info.queueFamilyIndex = vk_device->queue_family;

			VkResult ret = vkCreateCommandPool(vk_device->device, &cmdpool_create_info, &vk_device->allocator, &vk_cmdpool->cmdpool);
			CA_ASSERT(ret == VK_SUCCESS);

			cmdpool->handle = vk_cmdpool;
			cmdpool->device = device;
		}

		void destroy_cmdpool(cmdpool_t * cmdpool)
		{
			vk_device_t * vk_device = resolve_type(cmdpool->device);
			vk_cmdpool_t * vk_cmdpool = resolve_type(cmdpool);

			vkDestroyCommandPool(vk_device->device, vk_cmdpool->cmdpool, &vk_device->allocator);

			cmdpool->handle = nullptr;
			cmdpool->device = nullptr;
		}

		void cmdpool_reset(cmdpool_t * cmdpool)
		{
			vk_device_t * vk_device = resolve_type(cmdpool->device);
			vk_cmdpool_t * vk_cmdpool = resolve_type(cmdpool);

			VkResult ret = vkResetCommandPool(vk_device->device, vk_cmdpool->cmdpool, 0);
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif