#if CA_TARGET_GFX_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/mem.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_uniformpool(uniformpool_t * uniformpool, device_t * device, u32 max_uniforms, u32 max_uniformsets)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_uniformpool_t * vk_uniformpool = mem::arena_alloc<vk_uniformpool_t>(device->arena, 1);

			u32 const uniformpool_size_count = VK_DESCRIPTOR_TYPE_RANGE_SIZE;
			VkDescriptorPoolSize * uniformpool_sizes = mem::arena_alloc<VkDescriptorPoolSize>(CA_APP_STACK, uniformpool_size_count);
			for (u32 i = VK_DESCRIPTOR_TYPE_BEGIN_RANGE, j = i + uniformpool_size_count; i != j; i++)
			{
				uniformpool_sizes[i].type = static_cast<VkDescriptorType>(i);
				uniformpool_sizes[i].descriptorCount = max_uniforms;
			}

			VkDescriptorPoolCreateInfo uniformpool_create_info;
			uniformpool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			uniformpool_create_info.pNext = nullptr;
			uniformpool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			uniformpool_create_info.maxSets = max_uniformsets;
			uniformpool_create_info.poolSizeCount = uniformpool_size_count;
			uniformpool_create_info.pPoolSizes = uniformpool_sizes;

			VkResult ret = vkCreateDescriptorPool(vk_device->device, &uniformpool_create_info, &vk_device->allocator, &vk_uniformpool->uniformpool);
			CA_ASSERT(ret == VK_SUCCESS);

			mem::arena_free(CA_APP_STACK, uniformpool_sizes);

			uniformpool->handle = vk_uniformpool;
			uniformpool->device = device;
		}

		void destroy_uniformpool(uniformpool_t * uniformpool)
		{
			vk_device_t * vk_device = resolve_type(uniformpool->device);
			vk_uniformpool_t * vk_uniformpool = resolve_type(uniformpool);

			vkDestroyDescriptorPool(vk_device->device, vk_uniformpool->uniformpool, &vk_device->allocator);

			mem::arena_free(uniformpool->device->arena, vk_uniformpool);

			uniformpool->handle = nullptr;
			uniformpool->device = nullptr;
		}
	}
}
#endif