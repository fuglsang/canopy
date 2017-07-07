#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/mem.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		void create_uniformset(uniformset_t * uniformset, uniformpool_t * uniformpool, shaderdecl_t * shaderdecl)
		{
			vk_device_t * vk_device = resolve_type(uniformpool->device);
			vk_uniformpool_t * vk_uniformpool = resolve_type(uniformpool);
			vk_uniformset_t * vk_uniformset = mem::arena_alloc<vk_uniformset_t>(uniformpool->device->arena, 1);

			VkDescriptorSetAllocateInfo uniformset_allocate_info;
			uniformset_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			uniformset_allocate_info.pNext = nullptr;
			uniformset_allocate_info.descriptorPool = vk_uniformpool->uniformpool;
			uniformset_allocate_info.descriptorSetCount = 1;
			uniformset_allocate_info.pSetLayouts = nullptr;//TODO

			VkResult ret = vkAllocateDescriptorSets(vk_device->device, &uniformset_allocate_info, &vk_uniformset->uniformset);
			CA_ASSERT(ret == VK_SUCCESS);

			uniformset->handle = vk_uniformset;
			uniformset->uniformpool = uniformpool;
		}

		void destroy_uniformset(uniformset_t * uniformset)
		{
			vk_device_t * vk_device = resolve_type(uniformset->uniformpool->device);
			vk_uniformpool_t * vk_uniformpool = resolve_type(uniformset->uniformpool);
			vk_uniformset_t * vk_uniformset = resolve_type(uniformset);

			VkResult ret = vkFreeDescriptorSets(vk_device->device, vk_uniformpool->uniformpool, 1, &vk_uniformset->uniformset);
			CA_ASSERT(ret == VK_SUCCESS);

			mem::arena_free(uniformset->uniformpool->device->arena, vk_uniformset);

			uniformset->handle = nullptr;
			uniformset->uniformpool = nullptr;
		}
	}
}
#endif