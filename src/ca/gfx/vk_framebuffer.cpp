#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/mem.h"
#include "ca/gfx/vk.h"

namespace ca
{
	namespace gfx
	{
		VkAttachmentLoadOp resolve_loadop(rendertargetloadop loadop)
		{
			switch (loadop)
			{
			case RENDERTARGETLOADOP_CLEAR:
				return VK_ATTACHMENT_LOAD_OP_CLEAR;

			case RENDERTARGETLOADOP_DISCARD:
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			case RENDERTARGETLOADOP_LOAD:
				return VK_ATTACHMENT_LOAD_OP_LOAD;

			default:
				CA_FATAL("unsupported loadop");
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			}
		}

		VkAttachmentStoreOp resolve_storeop(rendertargetstoreop storeop)
		{
			switch (storeop)
			{
			case RENDERTARGETSTOREOP_DISCARD:
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;

			case RENDERTARGETSTOREOP_STORE:
				return VK_ATTACHMENT_STORE_OP_STORE;

			default:
				CA_FATAL("unsupported storeop");
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
		}

		void create_framebuffer(framebuffer_t * framebuffer, device_t * device, rendertarget_t * attachments, u32 attachment_count)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_framebuffer_t * vk_framebuffer = mem::arena_alloc<vk_framebuffer_t>(device->arena, 1);

			CA_ASSERT(attachment_count != 0);
			u32 width = attachments[0].texture->width;
			u32 height = attachments[0].texture->height;
			for (u32 i = 1; i != attachment_count; i++)
			{
				CA_ASSERT(attachments[i].texture->width == width);
				CA_ASSERT(attachments[i].texture->height == height);
			}

			vk_framebuffer->attachment_count = attachment_count;
			vk_framebuffer->attachment_clearvalues = mem::arena_alloc<VkClearValue>(device->arena, attachment_count);

			VkAttachmentDescription * attachment_descs = mem::arena_alloc<VkAttachmentDescription>(CA_APP_STACK, attachment_count);
			VkAttachmentReference * attachment_color = mem::arena_alloc<VkAttachmentReference>(CA_APP_STACK, attachment_count);
			VkAttachmentReference * attachment_depth = mem::arena_alloc<VkAttachmentReference>(CA_APP_STACK, attachment_count);
			VkImageView * attachment_views = mem::arena_alloc<VkImageView>(CA_APP_STACK, attachment_count);
			{
				u32 attachment_color_count = 0;
				u32 attachment_depth_count = 0;

				for (u32 i = 0; i != attachment_count; i++)
				{
					VkAttachmentLoadOp loadop = resolve_loadop(attachments[i].loadop);
					VkAttachmentStoreOp storeop = resolve_storeop(attachments[i].storeop);

					attachment_descs[i].flags = 0;
					attachment_descs[i].format = resolve_type(attachments[i].texture)->format;
					attachment_descs[i].samples = VK_SAMPLE_COUNT_1_BIT;
					attachment_descs[i].loadOp = loadop;
					attachment_descs[i].storeOp = storeop;
					attachment_descs[i].stencilLoadOp = loadop;
					attachment_descs[i].stencilStoreOp = storeop;
					attachment_descs[i].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					attachment_descs[i].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//TODO

					attachment_color[i].attachment = i;
					attachment_color[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					attachment_color_count++;

					//TODO depth stencil

					attachment_views[i] = resolve_type(attachments[i].texture)->view;
				}

				CA_ASSERT(attachment_depth_count <= 1);

				VkSubpassDescription subpass_desc;
				subpass_desc.flags = 0;// TODO maybe VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX for stereo?
				subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass_desc.inputAttachmentCount = 0;
				subpass_desc.pInputAttachments = nullptr;
				subpass_desc.colorAttachmentCount = attachment_color_count;
				subpass_desc.pColorAttachments = attachment_color;
				subpass_desc.pResolveAttachments = nullptr;
				subpass_desc.pDepthStencilAttachment = (attachment_depth_count == 1) ? attachment_depth : nullptr;
				subpass_desc.preserveAttachmentCount = 0;
				subpass_desc.pPreserveAttachments = nullptr;

				//VkSubpassDependency subpass_dep;
				//subpass_dep.srcSubpass = VK_SUBPASS_EXTERNAL;
				//subpass_dep.dstSubpass = 0;
				//subpass_dep.srcStageMask;//TODO
				//subpass_dep.dstStageMask;//TODO
				//subpass_dep.srcAccessMask;//TODO
				//subpass_dep.dstAccessMask;//TODO
				//subpass_dep.dependencyFlags = 0;//TODO

				VkRenderPassCreateInfo renderpass_create_info;
				renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
				renderpass_create_info.pNext = nullptr;
				renderpass_create_info.flags = 0;
				renderpass_create_info.attachmentCount = attachment_count;
				renderpass_create_info.pAttachments = attachment_descs;
				renderpass_create_info.subpassCount = 1;
				renderpass_create_info.pSubpasses = &subpass_desc;
				renderpass_create_info.dependencyCount = 0;
				renderpass_create_info.pDependencies = nullptr;

				VkResult ret = vkCreateRenderPass(vk_device->device, &renderpass_create_info, &vk_device->allocator, &vk_framebuffer->renderpass);
				CA_ASSERT(ret == VK_SUCCESS);

				VkFramebufferCreateInfo framebuffer_create_info;
				framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_create_info.pNext = nullptr;
				framebuffer_create_info.flags = 0;
				framebuffer_create_info.renderPass = vk_framebuffer->renderpass;
				framebuffer_create_info.attachmentCount = attachment_count;
				framebuffer_create_info.pAttachments = attachment_views;
				framebuffer_create_info.width = width;
				framebuffer_create_info.height = height;
				framebuffer_create_info.layers = 1;

				ret = vkCreateFramebuffer(vk_device->device, &framebuffer_create_info, &vk_device->allocator, &vk_framebuffer->framebuffer);
				CA_ASSERT(ret == VK_SUCCESS);
			}
			mem::arena_free(CA_APP_STACK, attachment_views);
			mem::arena_free(CA_APP_STACK, attachment_depth);
			mem::arena_free(CA_APP_STACK, attachment_color);
			mem::arena_free(CA_APP_STACK, attachment_descs);

			for (u32 i = 0; i != attachment_count; i++)
			{
				VkClearValue * clearvalue = &vk_framebuffer->attachment_clearvalues[i];
				clearvalue->color.float32[0] = attachments[i].clear_color[0];
				clearvalue->color.float32[1] = attachments[i].clear_color[1];
				clearvalue->color.float32[2] = attachments[i].clear_color[2];
				clearvalue->color.float32[3] = attachments[i].clear_color[3];

				//TODO depth stencil
				//clearvalue->depthStencil.depth = attachments[i].clear_depth;
				//clearvalue->depthStencil.stencil = attachments[i].clear_stencil;
			}

			framebuffer->handle = vk_framebuffer;
			framebuffer->device = device;
			framebuffer->width = width;
			framebuffer->height = height;
		}

		void destroy_framebuffer(framebuffer_t * framebuffer)
		{
			vk_device_t * vk_device = resolve_type(framebuffer->device);
			vk_framebuffer_t * vk_framebuffer = resolve_type(framebuffer);

			vkDestroyFramebuffer(vk_device->device, vk_framebuffer->framebuffer, &vk_device->allocator);
			vkDestroyRenderPass(vk_device->device, vk_framebuffer->renderpass, &vk_device->allocator);

			mem::arena_free(framebuffer->device->arena, vk_framebuffer->attachment_clearvalues);
			mem::arena_free(framebuffer->device->arena, vk_framebuffer);

			framebuffer->handle = nullptr;
			framebuffer->device = nullptr;
		}
	}
}
#endif