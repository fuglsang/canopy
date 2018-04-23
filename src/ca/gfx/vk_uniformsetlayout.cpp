#if CA_TARGET_GFX_VULKAN
#include "ca_base.h"
#include "ca/core_assert.h"
#include "ca/mem.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		extern VkShaderStageFlagBits resolve_stage(shaderstage stage);

		VkDescriptorType resolve_property(uniformtype type)
		{
			switch (type)
			{
			case UNIFORMTYPE_SAMPLER:
				return VK_DESCRIPTOR_TYPE_SAMPLER;

			case UNIFORMTYPE_STORAGE_BUFFER:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

			case UNIFORMTYPE_TEXTURE:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

			case UNIFORMTYPE_CONSTANT_BUFFER:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			default:
				CA_FATAL("unsupported shader property");
				return VK_DESCRIPTOR_TYPE_MAX_ENUM;
			}
		}

		void create_uniformsetlayout(uniformsetlayout_t * uniformsetlayout, device_t * device, uniformsetdecl_t * uniformsetdecl)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_uniformsetlayout_t * vk_uniformsetlayout = mem::arena_alloc<vk_uniformsetlayout_t>(device->arena, 1);

			VkDescriptorSetLayoutBinding * uniformsetlayout_bindings = mem::arena_alloc<VkDescriptorSetLayoutBinding>(CA_APP_STACK, uniformsetdecl->binding_count);
			for (u32 i = 0; i != uniformsetdecl->binding_count; i++)
			{
				uniformsetlayout_bindings[i].binding = uniformsetdecl->bindings[i].binding;
				uniformsetlayout_bindings[i].descriptorType = resolve_property(uniformsetdecl->bindings[i].type);
				uniformsetlayout_bindings[i].descriptorCount = 1;
				uniformsetlayout_bindings[i].stageFlags = resolve_stage(uniformsetdecl->bindings[i].stage);;
				uniformsetlayout_bindings[i].pImmutableSamplers = nullptr;
			}

			VkDescriptorSetLayoutCreateInfo uniformsetlayout_create_info;
			uniformsetlayout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			uniformsetlayout_create_info.pNext = nullptr;
			uniformsetlayout_create_info.flags = 0;// VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
			uniformsetlayout_create_info.bindingCount = 1;
			uniformsetlayout_create_info.pBindings = uniformsetlayout_bindings;

			VkResult ret = vkCreateDescriptorSetLayout(vk_device->device, &uniformsetlayout_create_info, &vk_device->allocator, &vk_uniformsetlayout->uniformsetlayout);
			CA_ASSERT(ret == VK_SUCCESS);

			mem::arena_free(CA_APP_STACK, uniformsetlayout_bindings);

			uniformsetlayout->handle = vk_uniformsetlayout;
			uniformsetlayout->device = device;
		}

		void destroy_uniformsetlayout(uniformsetlayout_t * uniformsetlayout)
		{
			vk_device_t * vk_device = resolve_type(uniformsetlayout->device);
			vk_uniformsetlayout_t * vk_uniformsetlayout = resolve_type(uniformsetlayout);

			vkDestroyDescriptorSetLayout(vk_device->device, vk_uniformsetlayout->uniformsetlayout, &vk_device->allocator);

			mem::arena_free(uniformsetlayout->device->arena, vk_uniformsetlayout);

			uniformsetlayout->handle = nullptr;
			uniformsetlayout->device = nullptr;
		}
	}
}
#endif