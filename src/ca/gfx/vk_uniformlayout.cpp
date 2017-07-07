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
		extern VkDescriptorType resolve_property(shaderprop type);
		extern VkShaderStageFlagBits resolve_stage(shaderstage stage);

		void create_uniformlayout(uniformlayout_t * uniformlayout, device_t * device, shaderdecl_t * shaderdecl)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_uniformlayout_t * vk_uniformlayout = mem::arena_alloc<vk_uniformlayout_t>(device->arena, 1);

			VkDescriptorSetLayoutBinding * uniformlayout_bindings = mem::arena_alloc<VkDescriptorSetLayoutBinding>(CA_APP_STACK, shaderdecl->property_count);
			for (u32 i = 0; i != shaderdecl->property_count; i++)
			{
				uniformlayout_bindings[i].binding = shaderdecl->properties[i].binding;
				uniformlayout_bindings[i].descriptorType = resolve_property(shaderdecl->properties[i].type);
				uniformlayout_bindings[i].descriptorCount = 1;
				uniformlayout_bindings[i].stageFlags = resolve_stage(shaderdecl->properties[i].stage);;
				uniformlayout_bindings[i].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo uniformlayout_create_info;
			uniformlayout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			uniformlayout_create_info.pNext = nullptr;
			uniformlayout_create_info.flags = 0;// VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
			uniformlayout_create_info.bindingCount = 1;
			uniformlayout_create_info.pBindings = uniformlayout_bindings;

			VkResult ret = vkCreateDescriptorSetLayout(vk_device->device, &uniformlayout_create_info, &vk_device->allocator, &vk_uniformlayout->uniformlayout);
			CA_ASSERT(ret == VK_SUCCESS);

			mem::arena_free(CA_APP_STACK, uniformlayout_bindings);

			uniformlayout->handle = vk_uniformlayout;
			uniformlayout->device = device;
		}

		void destroy_uniformlayout(uniformlayout_t * uniformlayout)
		{
			vk_device_t * vk_device = resolve_type(uniformlayout->device);
			vk_uniformlayout_t * vk_uniformlayout = resolve_type(uniformlayout);

			vkDestroyDescriptorSetLayout(vk_device->device, vk_uniformlayout->uniformlayout, &vk_device->allocator);

			mem::arena_free(uniformlayout->device->arena, vk_uniformlayout);

			uniformlayout->handle = nullptr;
			uniformlayout->device = nullptr;
		}
	}
}
#endif