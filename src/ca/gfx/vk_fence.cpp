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
		void create_fence(fence_t * fence, device_t * device, bool signaled)
		{
			vk_device_t * vk_device = resolve_device(device);
			vk_fence_t * vk_fence = mem::arena_alloc<vk_fence_t>(device->arena, 1);

			VkFenceCreateInfo fence_create_info;
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_create_info.pNext = nullptr;
			fence_create_info.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

			VkResult ret = vkCreateFence(vk_device->device, &fence_create_info, &vk_device->allocator, &vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS);

			fence->handle = vk_fence;
			fence->device = device;
		}

		void destroy_fence(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_device(fence->device);
			vk_fence_t * vk_fence = resolve_fence(fence);

			vkDestroyFence(vk_device->device, vk_fence->fence, &vk_device->allocator);

			mem::arena_free(fence->device->arena, fence->handle);

			fence->handle = nullptr;
			fence->device = nullptr;
		}

		void fence_peek_signaled(fence_t * fence, bool * signaled)
		{
			vk_device_t * vk_device = resolve_device(fence->device);
			vk_fence_t * vk_fence = resolve_fence(fence);

			VkResult ret = vkGetFenceStatus(vk_device->device, vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS || ret == VK_NOT_READY);

			if (ret == VK_SUCCESS)
				*signaled = true;
			else
				*signaled = false;
		}

		void fence_wait_signaled(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_device(fence->device);
			vk_fence_t * vk_fence = resolve_fence(fence);

			VkResult ret = vkWaitForFences(vk_device->device, 1, &vk_fence->fence, VK_TRUE, UINT64_MAX);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void fence_reset_signaled(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_device(fence->device);
			vk_fence_t * vk_fence = resolve_fence(fence);

			VkResult ret = vkResetFences(vk_device->device, 1, &vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif