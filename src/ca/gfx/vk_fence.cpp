#if CA_TARGET_GFX_VULKAN
#include "ca/core_assert.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_fence(fence_t * fence, device_t * device, bool signaled)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_fence_t * vk_fence = mem::arena_alloc<vk_fence_t>(device->arena, 1);

			VkFenceCreateFlags fence_create_flags = 0;
			fence_create_flags |= (signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0);

			VkFenceCreateInfo fence_create_info;
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_create_info.pNext = nullptr;
			fence_create_info.flags = fence_create_flags;

			VkResult ret = vkCreateFence(vk_device->device, &fence_create_info, &vk_device->allocator, &vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS);

			fence->handle = vk_fence;
			fence->device = device;
		}

		void destroy_fence(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_type(fence->device);
			vk_fence_t * vk_fence = resolve_type(fence);

			vkDestroyFence(vk_device->device, vk_fence->fence, &vk_device->allocator);

			mem::arena_free(fence->device->arena, vk_fence);

			fence->handle = nullptr;
			fence->device = nullptr;
		}

		void fence_peek(fence_t * fence, bool * signaled)
		{
			vk_device_t * vk_device = resolve_type(fence->device);
			vk_fence_t * vk_fence = resolve_type(fence);

			VkResult ret = vkGetFenceStatus(vk_device->device, vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS || ret == VK_NOT_READY);

			if (ret == VK_SUCCESS)
				*signaled = true;
			else
				*signaled = false;
		}

		void fence_reset(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_type(fence->device);
			vk_fence_t * vk_fence = resolve_type(fence);

			VkResult ret = vkResetFences(vk_device->device, 1, &vk_fence->fence);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void fence_wait(fence_t * fence)
		{
			vk_device_t * vk_device = resolve_type(fence->device);
			vk_fence_t * vk_fence = resolve_type(fence);

			VkResult ret = vkWaitForFences(vk_device->device, 1, &vk_fence->fence, VK_TRUE, UINT64_MAX);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void fence_wait_reset(fence_t * fence)
		{
			fence_wait(fence);
			fence_reset(fence);
		}
	}
}
#endif