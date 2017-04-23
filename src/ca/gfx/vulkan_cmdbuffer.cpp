#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vulkan.h"

namespace ca
{
	namespace gfx
	{
		void create_cmdbuffer(cmdbuffer_t * cmdbuffer, device_t * device, cmdbuffertype type)
		{
			vk_device_t * vk_device = resolve_device(device);
			vk_cmdbuffer_t * vk_cmdbuffer = mem::arena_alloc<vk_cmdbuffer_t>(device->arena, 1);

			VkCommandBufferAllocateInfo allocate_info;
			allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocate_info.pNext = nullptr;
			allocate_info.commandPool = vk_device->cmdpool[type];
			allocate_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			allocate_info.commandBufferCount = 1;

			CA_LOG("vulkan_cmdbuffer: allocate cmdbuffer ... ");
			VkResult ret = vkAllocateCommandBuffers(vk_device->device, &allocate_info, &vk_cmdbuffer->cmdbuffer);
			CA_ASSERT(ret == VK_SUCCESS);

			cmdbuffer->handle = vk_cmdbuffer;
			cmdbuffer->device = device;
			cmdbuffer->type = type;
			CA_LOG("vulkan_cmdbuffer: READY");
		}

		void destroy_cmdbuffer(cmdbuffer_t * cmdbuffer)
		{
			vk_device_t * vk_device = resolve_device(cmdbuffer->device);
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);

			CA_LOG("vulkan_cmdbuffer: destroy cmdbuffer ... ");
			vkFreeCommandBuffers(vk_device->device, vk_device->cmdpool[cmdbuffer->type], 1, &vk_cmdbuffer->cmdbuffer);

			mem::arena_free(cmdbuffer->device->arena, cmdbuffer->handle);
			CA_LOG("vulkan_cmdbuffer: CLEAN");
		}

		void cmdbuffer_begin(cmdbuffer_t * cmdbuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);

			//TODO
			//VkCommandBufferInheritanceInfo inheritance_info;
			//inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			//inheritance_info.pNext = nullptr;
			//inheritance_info.renderPass;
			//inheritance_info.subpass;
			//inheritance_info.framebuffer;
			//inheritance_info.occlusionQueryEnable;
			//inheritance_info.queryFlags;
			//inheritance_info.pipelineStatistics;

			VkCommandBufferBeginInfo begin_info;
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.pNext = nullptr;
			begin_info.flags = 0;
			begin_info.pInheritanceInfo = nullptr;//TODO

			VkResult ret = vkBeginCommandBuffer(vk_cmdbuffer->cmdbuffer, &begin_info);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void cmdbuffer_exec(cmdbuffer_t * cmdbuffer, cmdbuffer_t * cmdbuffer_exec)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);
			vk_cmdbuffer_t * vk_cmdbuffer_exec = resolve_cmdbuffer(cmdbuffer_exec);
			vkCmdExecuteCommands(vk_cmdbuffer->cmdbuffer, 1, &vk_cmdbuffer_exec->cmdbuffer);
		}

		void cmdbuffer_draw(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 vertex_count)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);
			vkCmdDraw(vk_cmdbuffer->cmdbuffer, vertex_count, 1, vertex_start, 0);
		}

		void cmdbuffer_draw_indexed(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 index_start, u32 index_count)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);
			vkCmdDrawIndexed(vk_cmdbuffer->cmdbuffer, index_count, 1, index_start, vertex_start, 0);
		}

		void cmdbuffer_end(cmdbuffer_t * cmdbuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_cmdbuffer(cmdbuffer);

			VkResult ret = vkEndCommandBuffer(vk_cmdbuffer->cmdbuffer);
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif