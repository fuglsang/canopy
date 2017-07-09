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

			mem::arena_free(cmdbuffer->cmdpool->device->arena, vk_cmdbuffer);

			cmdbuffer->handle = nullptr;
			cmdbuffer->cmdpool = nullptr;
		}

		void cmdbuffer_reset(cmdbuffer_t * cmdbuffer)
		{
			VkResult ret = vkResetCommandBuffer(resolve_handle(cmdbuffer), 0);
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

		void cmdbuffer_begin_renderpass(cmdbuffer_t * cmdbuffer, framebuffer_t * framebuffer)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_framebuffer_t * vk_framebuffer = resolve_type(framebuffer);
			
			VkRect2D render_area;
			render_area.offset = { 0, 0 };
			render_area.extent = { framebuffer->width, framebuffer->height };

			VkRenderPassBeginInfo renderpass_begin_info;
			renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderpass_begin_info.pNext = nullptr;
			renderpass_begin_info.renderPass = vk_framebuffer->renderpass;
			renderpass_begin_info.framebuffer = vk_framebuffer->framebuffer;
			renderpass_begin_info.renderArea = render_area;
			renderpass_begin_info.clearValueCount = vk_framebuffer->attachment_count;
			renderpass_begin_info.pClearValues = vk_framebuffer->attachment_clearvalues;

			vkCmdBeginRenderPass(vk_cmdbuffer->cmdbuffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		}

		void cmdbuffer_end_renderpass(cmdbuffer_t * cmdbuffer)
		{
			vkCmdEndRenderPass(resolve_handle(cmdbuffer));
		}

		void cmdbuffer_clear_color(cmdbuffer_t * cmdbuffer, texture_t * texture, math::fvec4_t const & color)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_texture_t * vk_texture = resolve_type(texture);

			VkClearColorValue ccv;
			ccv.float32[0] = color.x;
			ccv.float32[1] = color.y;
			ccv.float32[2] = color.z;
			ccv.float32[3] = color.w;

			VkImageSubresourceRange image_subresource_range;
			image_subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_subresource_range.baseMipLevel = 0;
			image_subresource_range.levelCount = 1;
			image_subresource_range.baseArrayLayer = 0;
			image_subresource_range.layerCount = 1;

			vkCmdClearColorImage(vk_cmdbuffer->cmdbuffer, vk_texture->texture, VK_IMAGE_LAYOUT_GENERAL, &ccv, 1, &image_subresource_range);
		}

		void cmdbuffer_bind_pipeline(cmdbuffer_t * cmdbuffer, pipeline_t * pipeline)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_pipeline_t * vk_pipeline = resolve_type(pipeline);

			CA_ASSERT(pipeline->type == PIPELINETYPE_GRAPHICS);
			vkCmdBindPipeline(vk_cmdbuffer->cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->pipeline);
		}

		void cmdbuffer_bind_uniformset(cmdbuffer_t * cmdbuffer, pipeline_t * pipeline, u32 index, uniformset_t * uniformset)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_pipeline_t * vk_pipeline = resolve_type(pipeline);
			vk_uniformset_t * vk_uniformset = resolve_type(uniformset);

			vkCmdBindDescriptorSets(vk_cmdbuffer->cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->layout, index, 1, &vk_uniformset->uniformset, 0, nullptr);
		}

		void cmdbuffer_bind_indexbuffer(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset, size_t stride)
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
				CA_FATAL("unsupported index buffer stride");
			}

			vkCmdBindIndexBuffer(vk_cmdbuffer->cmdbuffer, vk_buffer->buffer, offset, index_type);
		}

		void cmdbuffer_bind_vertexbuffer(cmdbuffer_t * cmdbuffer, buffer_t * buffer, size_t offset)
		{
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			VkDeviceSize const vk_offset = offset;

			vkCmdBindVertexBuffers(vk_cmdbuffer->cmdbuffer, 0, 1, &vk_buffer->buffer, &vk_offset);
		}

		void cmdbuffer_set_viewport(cmdbuffer_t * cmdbuffer, i32 x, i32 y, u32 width, u32 height)
		{
			VkViewport viewport;
			viewport.x = static_cast<f32>(x);
			viewport.y = static_cast<f32>(y);// TODO find out best way to flip viewport
			viewport.width = static_cast<f32>(width);
			viewport.height = static_cast<f32>(height);// maybe rely on VK_KHR_maintenance1 ?
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(resolve_handle(cmdbuffer), 0, 1, &viewport);
		}

		void cmdbuffer_set_scissor(cmdbuffer_t * cmdbuffer, i32 x, i32 y, u32 width, u32 height)
		{
			VkRect2D scissor;
			scissor.offset.x = x;
			scissor.offset.y = y;
			scissor.extent.width = width;
			scissor.extent.height = height;
			vkCmdSetScissor(resolve_handle(cmdbuffer), 0, 1, &scissor);
		}

		void cmdbuffer_draw(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 vertex_count)
		{
			vkCmdDraw(resolve_handle(cmdbuffer), vertex_count, 1, vertex_start, 0);
		}

		void cmdbuffer_draw_indexed(cmdbuffer_t * cmdbuffer, u32 vertex_start, u32 index_start, u32 index_count)
		{
			vkCmdDrawIndexed(resolve_handle(cmdbuffer), index_count, 1, index_start, vertex_start, 0);
		}

		void cmdbuffer_end(cmdbuffer_t * cmdbuffer)
		{
			VkResult ret = vkEndCommandBuffer(resolve_handle(cmdbuffer));
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif