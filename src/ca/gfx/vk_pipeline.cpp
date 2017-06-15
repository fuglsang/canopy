#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vk.h"
#include "ca/mem.h"

namespace ca
{
	namespace gfx
	{
		struct pipelinestage_t
		{
			shader_t * shader;
		};

		void create_pipeline(pipeline_t * pipeline, framebuffer_t * framebuffer)
		{
			device_t * device = framebuffer->device;
			vk_device_t * vk_device = resolve_type(device);
			vk_pipeline_t * vk_pipeline = mem::arena_alloc<vk_pipeline_t>(device->arena, 1);
			vk_framebuffer_t * vk_framebuffer = resolve_type(framebuffer);

			VkPipelineLayoutCreateInfo pipelinelayout_create_info;
			pipelinelayout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelinelayout_create_info.pNext = nullptr;
			pipelinelayout_create_info.flags = 0;
			pipelinelayout_create_info.setLayoutCount = 0;//TODO
			pipelinelayout_create_info.pSetLayouts = nullptr;//TODO
			pipelinelayout_create_info.pushConstantRangeCount = 0;//TODO
			pipelinelayout_create_info.pPushConstantRanges = nullptr;//TODO

			VkResult ret = vkCreatePipelineLayout(vk_device->device, &pipelinelayout_create_info, &vk_device->allocator, &vk_pipeline->layout);
			CA_ASSERT(ret == VK_SUCCESS);

			VkPipelineVertexInputStateCreateInfo pipeline_vertexinputstate;
			pipeline_vertexinputstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipeline_vertexinputstate.pNext = nullptr;
			pipeline_vertexinputstate.flags = 0;
			pipeline_vertexinputstate.vertexBindingDescriptionCount = 0;//TODO
			pipeline_vertexinputstate.pVertexBindingDescriptions = nullptr;//TODO
			pipeline_vertexinputstate.vertexAttributeDescriptionCount = 0;//TODO
			pipeline_vertexinputstate.pVertexAttributeDescriptions = nullptr;//TODO

			VkPipelineInputAssemblyStateCreateInfo pipeline_inputassemblystate;
			pipeline_inputassemblystate.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipeline_inputassemblystate.pNext = nullptr;
			pipeline_inputassemblystate.flags = 0;
			pipeline_inputassemblystate.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//TODO
			pipeline_inputassemblystate.primitiveRestartEnable = VK_FALSE;

			VkPipelineViewportStateCreateInfo pipeline_viewportstate;
			pipeline_viewportstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipeline_viewportstate.pNext = nullptr;
			pipeline_viewportstate.flags = 0;
			pipeline_viewportstate.viewportCount = 1;
			pipeline_viewportstate.pViewports = nullptr;// ignored because of dynamic state
			pipeline_viewportstate.scissorCount = 1;
			pipeline_viewportstate.pScissors = nullptr;// ignored because of dynamic state

			VkPipelineRasterizationStateCreateInfo pipeline_rasterizationstate;
			pipeline_rasterizationstate.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			pipeline_rasterizationstate.pNext = nullptr;
			pipeline_rasterizationstate.flags = 0;
			pipeline_rasterizationstate.depthClampEnable = VK_FALSE;
			pipeline_rasterizationstate.rasterizerDiscardEnable = VK_FALSE;
			pipeline_rasterizationstate.polygonMode = VK_POLYGON_MODE_FILL;
			pipeline_rasterizationstate.cullMode = VK_CULL_MODE_NONE;//TODO cull mode
			pipeline_rasterizationstate.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			pipeline_rasterizationstate.depthBiasEnable = VK_FALSE;
			pipeline_rasterizationstate.depthBiasConstantFactor = 0.0f;
			pipeline_rasterizationstate.depthBiasClamp = 0.0f;
			pipeline_rasterizationstate.depthBiasSlopeFactor = 1.0f;
			pipeline_rasterizationstate.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo pipeline_multisamplestate;
			pipeline_multisamplestate.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipeline_multisamplestate.pNext = nullptr;
			pipeline_multisamplestate.flags = 0;
			pipeline_multisamplestate.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			pipeline_multisamplestate.sampleShadingEnable = VK_FALSE;
			pipeline_multisamplestate.minSampleShading = 1.0f;// ??
			pipeline_multisamplestate.pSampleMask = nullptr;
			pipeline_multisamplestate.alphaToCoverageEnable = VK_FALSE;
			pipeline_multisamplestate.alphaToOneEnable = VK_FALSE;

			//TODO blendfactors
			VkPipelineColorBlendAttachmentState * pipeline_colorblendattachments = mem::arena_alloc<VkPipelineColorBlendAttachmentState>(CA_APP_STACK, vk_framebuffer->attachment_count);//TODO depth stencil (attachment_count)
			for (u32 i = 0; i != vk_framebuffer->attachment_count; i++)
			{
				pipeline_colorblendattachments[i].blendEnable = VK_FALSE;
				pipeline_colorblendattachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				pipeline_colorblendattachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				pipeline_colorblendattachments[i].colorBlendOp = VK_BLEND_OP_ADD;
				pipeline_colorblendattachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
				pipeline_colorblendattachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				pipeline_colorblendattachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
				pipeline_colorblendattachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			}

			VkPipelineColorBlendStateCreateInfo pipeline_colorblendstate;
			pipeline_colorblendstate.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipeline_colorblendstate.pNext = nullptr;
			pipeline_colorblendstate.flags = 0;
			pipeline_colorblendstate.logicOpEnable = VK_FALSE;
			pipeline_colorblendstate.logicOp = VK_LOGIC_OP_MAX_ENUM;
			pipeline_colorblendstate.attachmentCount = vk_framebuffer->attachment_count;//TODO depth stencil (attachment_count)
			pipeline_colorblendstate.pAttachments = pipeline_colorblendattachments;
			pipeline_colorblendstate.blendConstants[0] = 1.0f;
			pipeline_colorblendstate.blendConstants[1] = 1.0f;
			pipeline_colorblendstate.blendConstants[2] = 1.0f;
			pipeline_colorblendstate.blendConstants[3] = 1.0f;

			u32 const pipeline_dynamic_count = 2;
			VkDynamicState pipeline_dynamic[pipeline_dynamic_count] = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};

			VkPipelineDynamicStateCreateInfo pipeline_dynamicstate;
			pipeline_dynamicstate.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipeline_dynamicstate.pNext = nullptr;
			pipeline_dynamicstate.flags = 0;
			pipeline_dynamicstate.dynamicStateCount = pipeline_dynamic_count;
			pipeline_dynamicstate.pDynamicStates = pipeline_dynamic;

			VkGraphicsPipelineCreateInfo pipeline_create_info;
			pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipeline_create_info.pNext = nullptr;
			pipeline_create_info.flags = 0;
			pipeline_create_info.stageCount = 0;//TODO stages
			pipeline_create_info.pStages = nullptr;//TODO stages
			pipeline_create_info.pVertexInputState = &pipeline_vertexinputstate;
			pipeline_create_info.pInputAssemblyState = &pipeline_inputassemblystate;
			pipeline_create_info.pTessellationState = nullptr;
			pipeline_create_info.pViewportState = &pipeline_viewportstate;
			pipeline_create_info.pRasterizationState = &pipeline_rasterizationstate;
			pipeline_create_info.pMultisampleState = &pipeline_multisamplestate;
			pipeline_create_info.pDepthStencilState = nullptr;//TODO depth stencil
			pipeline_create_info.pColorBlendState = &pipeline_colorblendstate;
			pipeline_create_info.pDynamicState = &pipeline_dynamicstate;
			pipeline_create_info.layout = vk_pipeline->layout;
			pipeline_create_info.renderPass = vk_framebuffer->renderpass;
			pipeline_create_info.subpass = 0;
			pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
			pipeline_create_info.basePipelineIndex = -1;

			ret = vkCreateGraphicsPipelines(vk_device->device, VK_NULL_HANDLE, 1, &pipeline_create_info, &vk_device->allocator, &vk_pipeline->pipeline);
			CA_ASSERT(ret == VK_SUCCESS);

			mem::arena_free(CA_APP_STACK, pipeline_colorblendattachments);

			pipeline->handle = vk_pipeline;
			pipeline->device = device;
		}

		void destroy_pipeline(pipeline_t * pipeline)
		{
			vk_device_t * vk_device = resolve_type(pipeline->device);
			vk_pipeline_t * vk_pipeline = resolve_type(pipeline);

			vkDestroyPipelineLayout(vk_device->device, vk_pipeline->layout, &vk_device->allocator);
			vkDestroyPipeline(vk_device->device, vk_pipeline->pipeline, &vk_device->allocator);

			pipeline->handle = nullptr;
			pipeline->device = nullptr;
		}
	}
}
#endif