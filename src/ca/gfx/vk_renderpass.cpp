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
		VkAttachmentLoadOp resolve_loadop(renderloadop loadop)
		{
			switch (loadop)
			{
			case RENDERLOADOP_CLEAR:
				return VK_ATTACHMENT_LOAD_OP_CLEAR;

			case RENDERLOADOP_DISCARD:
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;

			case RENDERLOADOP_LOAD:
				return VK_ATTACHMENT_LOAD_OP_LOAD;

			default:
				CA_FATAL("unsupported loadop");
				return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			}
		}

		VkAttachmentStoreOp resolve_storeop(renderstoreop storeop)
		{
			switch (storeop)
			{
			case RENDERSTOREOP_DISCARD:
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;

			case RENDERSTOREOP_STORE:
				return VK_ATTACHMENT_STORE_OP_STORE;

			default:
				CA_FATAL("unsupported storeop");
				return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			}
		}

		void create_renderpass(renderpass_t * renderpass, device_t * device, renderattachment_t * attachments, u32 attachment_count)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_renderpass_t * vk_renderpass = mem::arena_alloc<vk_renderpass_t>(device->arena, 1);
			
			CA_ASSERT(attachment_count != 0);
			vk_renderpass->dim_x = attachments[0].texture->dim_x;
			vk_renderpass->dim_y = attachments[0].texture->dim_y;
			vk_renderpass->attachment_count = attachment_count;
			vk_renderpass->attachment_clearvalues = mem::arena_alloc<VkClearValue>(device->arena, attachment_count);

			VkAttachmentDescription * attachment_descs = mem::arena_alloc<VkAttachmentDescription>(CA_APP_STACK, attachment_count);
			VkAttachmentReference * attachment_refs = mem::arena_alloc<VkAttachmentReference>(CA_APP_STACK, attachment_count);
			VkImageView * attachment_views = mem::arena_alloc<VkImageView>(CA_APP_STACK, attachment_count);
			{
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

					attachment_refs[i].attachment = i;
					attachment_refs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					attachment_views[i] = resolve_type(attachments[i].texture)->view;
				}

				VkSubpassDescription subpass_desc;
				subpass_desc.flags = 0;// TODO maybe VK_SUBPASS_DESCRIPTION_PER_VIEW_ATTRIBUTES_BIT_NVX for stereo?
				subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
				subpass_desc.inputAttachmentCount = 0;
				subpass_desc.pInputAttachments = nullptr;
				subpass_desc.colorAttachmentCount = attachment_count;
				subpass_desc.pColorAttachments = attachment_refs;
				subpass_desc.pResolveAttachments = nullptr;
				subpass_desc.pDepthStencilAttachment = nullptr;// TODO
				subpass_desc.preserveAttachmentCount = 0;
				subpass_desc.pPreserveAttachments = nullptr;

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

				VkResult ret = vkCreateRenderPass(vk_device->device, &renderpass_create_info, &vk_device->allocator, &vk_renderpass->renderpass);
				CA_ASSERT(ret == VK_SUCCESS);

				VkFramebufferCreateInfo framebuffer_create_info;
				framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_create_info.pNext = nullptr;
				framebuffer_create_info.flags = 0;
				framebuffer_create_info.renderPass = vk_renderpass->renderpass;
				framebuffer_create_info.attachmentCount = attachment_count;
				framebuffer_create_info.pAttachments = attachment_views;
				framebuffer_create_info.width = vk_renderpass->dim_x;
				framebuffer_create_info.height = vk_renderpass->dim_y;
				framebuffer_create_info.layers = 1;

				ret = vkCreateFramebuffer(vk_device->device, &framebuffer_create_info, &vk_device->allocator, &vk_renderpass->framebuffer);
				CA_ASSERT(ret == VK_SUCCESS);
			}
			mem::arena_free(CA_APP_STACK, attachment_views);
			mem::arena_free(CA_APP_STACK, attachment_refs);
			mem::arena_free(CA_APP_STACK, attachment_descs);

			renderpass->handle = vk_renderpass;
			renderpass->device = device;
		}

		void destroy_renderpass(renderpass_t * renderpass)
		{
			vk_device_t * vk_device = resolve_type(renderpass->device);
			vk_renderpass_t * vk_renderpass = resolve_type(renderpass);

			vkDestroyFramebuffer(vk_device->device, vk_renderpass->framebuffer, &vk_device->allocator);
			vkDestroyRenderPass(vk_device->device, vk_renderpass->renderpass, &vk_device->allocator);

			mem::arena_free(renderpass->device->arena, vk_renderpass->attachment_clearvalues);
			mem::arena_free(renderpass->device->arena, renderpass->handle);

			renderpass->handle = nullptr;
			renderpass->device = nullptr;
		}
	}
}
#endif