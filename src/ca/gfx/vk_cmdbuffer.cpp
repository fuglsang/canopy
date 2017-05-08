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
		void create_cmdbuffer(cmdbuffer_t * cmdbuffer, cmdpool_t * cmdpool)
		{
			vk_device_t * vk_device = resolve_type(cmdpool->device);
			vk_cmdpool_t * vk_cmdpool = resolve_type(cmdpool);
			vk_cmdbuffer_t * vk_cmdbuffer = mem::arena_alloc<vk_cmdbuffer_t>(cmdpool->device->arena, 1);

			VkCommandBufferAllocateInfo allocate_info;
			allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocate_info.pNext = nullptr;
			allocate_info.commandPool = vk_cmdpool->cmdpool;
			allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocate_info.commandBufferCount = 1;

			VkResult ret = vkAllocateCommandBuffers(vk_device->device, &allocate_info, &vk_cmdbuffer->cmdbuffer);
			CA_ASSERT(ret == VK_SUCCESS);

			cmdbuffer->handle = vk_cmdbuffer;
			cmdbuffer->cmdpool = cmdpool;
		}

		void destroy_cmdbuffer(cmdbuffer_t * cmdbuffer)
		{
			vk_device_t * vk_device = resolve_type(cmdbuffer->cmdpool->device);
			vk_cmdpool_t * vk_cmdpool = resolve_type(cmdbuffer->cmdpool);
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);

			vkFreeCommandBuffers(vk_device->device, vk_cmdpool->cmdpool, 1, &vk_cmdbuffer->cmdbuffer);

			mem::arena_free(cmdbuffer->cmdpool->device->arena, cmdbuffer->handle);

			cmdbuffer->handle = nullptr;
			cmdbuffer->cmdpool = nullptr;
		}

		void cmdbuffer_reset(cmdbuffer_t * cmdbuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);

			VkResult ret = vkResetCommandBuffer(vk_cmdbuffer->cmdbuffer, 0);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void cmdbuffer_begin(cmdbuffer_t * cmdbuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);

			VkCommandBufferUsageFlags cmdbuffer_usage_flags = 0;
			cmdbuffer_usage_flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			VkCommandBufferInheritanceInfo cmdbuffer_inheritance_info = {};//TODO
			cmdbuffer_inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			//cmdbuffer_inheritance_info.pNext = nullptr;
			//cmdbuffer_inheritance_info.renderPass;
			//cmdbuffer_inheritance_info.subpass;
			//cmdbuffer_inheritance_info.framebuffer;
			//cmdbuffer_inheritance_info.occlusionQueryEnable;
			//cmdbuffer_inheritance_info.queryFlags;
			//cmdbuffer_inheritance_info.pipelineStatistics;

			VkCommandBufferBeginInfo cmdbuffer_begin_info;
			cmdbuffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdbuffer_begin_info.pNext = nullptr;
			cmdbuffer_begin_info.flags = cmdbuffer_usage_flags;
			cmdbuffer_begin_info.pInheritanceInfo = &cmdbuffer_inheritance_info;

			VkResult ret = vkBeginCommandBuffer(vk_cmdbuffer->cmdbuffer, &cmdbuffer_begin_info);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void cmdbuffer_clear_color(cmdbuffer_t * cmdbuffer, texture_t * texture, math::vec4_t const & color)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_texture_t * vk_texture = resolve_type(texture);

			VkClearColorValue ccv;
			memcpy(ccv.float32, color.e, sizeof(ccv.float32));

			VkImageSubresourceRange image_subresource_range;
			image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_subresource_range.baseMipLevel = 0;
			image_subresource_range.levelCount = 1;
			image_subresource_range.baseArrayLayer = 0;
			image_subresource_range.layerCount = 1;

			vkCmdClearColorImage(vk_cmdbuffer->cmdbuffer, vk_texture->texture, VK_IMAGE_LAYOUT_GENERAL, &ccv, 1, &image_subresource_range);
		}

		void cmdbuffer_bind_index(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset, size_t stride)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			VkIndexType index_type;
			switch (stride)
			{
			case 2:
				index_type = VK_INDEX_TYPE_UINT16;
				break;
			case 4:
				index_type = VK_INDEX_TYPE_UINT32;
				break;
			default:
				CA_ASSERT_MSG(false, "unsupported stride");
			}

			vkCmdBindIndexBuffer(vk_cmdbuffer->cmdbuffer, vk_buffer->buffer, offset, index_type);
		}

		void cmdbuffer_bind_vertex(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			VkDeviceSize const vk_offset = offset;

			vkCmdBindVertexBuffers(vk_cmdbuffer->cmdbuffer, 0, 1, &vk_buffer->buffer, &vk_offset);
		}

		void cmdbuffer_draw(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 vertex_count)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vkCmdDraw(vk_cmdbuffer->cmdbuffer, vertex_count, 1, vertex_start, 0);
		}

		void cmdbuffer_draw_indexed(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 index_start, u32 index_count)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vkCmdDrawIndexed(vk_cmdbuffer->cmdbuffer, index_count, 1, index_start, vertex_start, 0);
		}

		void cmdbuffer_end(cmdbuffer_t * cmdbuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);

			VkResult ret = vkEndCommandBuffer(vk_cmdbuffer->cmdbuffer);
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif