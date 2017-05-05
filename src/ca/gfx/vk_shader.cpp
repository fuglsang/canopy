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
		void create_shader(shader_t * shader, device_t * device)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_shader_t * vk_shader = mem::arena_alloc<vk_shader_t>(device->arena, 1);

			VkShaderModuleCreateInfo shader_create_info;
			shader_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_create_info.pNext = nullptr;
			shader_create_info.flags = 0;
			shader_create_info.codeSize = 0;//TODO
			shader_create_info.pCode = nullptr;

			VkResult ret = vkCreateShaderModule(vk_device->device, &shader_create_info, &vk_device->allocator, &vk_shader->shader);
			CA_ASSERT(ret == VK_SUCCESS);

			shader->handle = vk_shader;
			shader->device = device;
		}

		void destroy_shader(shader_t * shader)
		{
			vk_device_t * vk_device = resolve_type(shader->device);
			vk_shader_t * vk_shader = resolve_type(shader);

			vkDestroyShaderModule(vk_device->device, vk_shader->shader, &vk_device->allocator);

			mem::arena_free(shader->device->arena, shader->handle);

			shader->handle = nullptr;
			shader->device = nullptr;
		}
	}
}
#endif