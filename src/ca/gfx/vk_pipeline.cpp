#if CA_TARGET_GFX_VULKAN
#include "ca_base.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vk.h"
#include "ca/gfx_vertexdecl.h"
#include "ca/mem.h"

namespace ca
{
	namespace gfx
	{
		VkShaderStageFlagBits resolve_stage(shaderstage stage)
		{
			switch (stage)
			{
			case SHADERSTAGE_COMPUTE:
				return VK_SHADER_STAGE_COMPUTE_BIT;

			case SHADERSTAGE_FRAGMENT:
				return VK_SHADER_STAGE_FRAGMENT_BIT;

			case SHADERSTAGE_GEOMETRY:
				return VK_SHADER_STAGE_GEOMETRY_BIT;

			case SHADERSTAGE_VERTEX:
				return VK_SHADER_STAGE_VERTEX_BIT;

			default:
				CA_FATAL("unsupported shader stage");
				return VK_SHADER_STAGE_ALL;
			}
		}

		VkFormat resolve_format(vertexdecl_t::attribdecl_t * decl)
		{
			#define __TRY_SELECT(BITS, SUFFIX)										\
			switch (decl->component_count)											\
			{																		\
				case 1: return VK_FORMAT_R##BITS##SUFFIX;							\
				case 2: return VK_FORMAT_R##BITS##G##BITS##SUFFIX;					\
				case 3: return VK_FORMAT_R##BITS##G##BITS##B##BITS##SUFFIX;			\
				case 4: return VK_FORMAT_R##BITS##G##BITS##B##BITS##A##BITS##SUFFIX;\
			}

			switch (decl->component_type)
			{
			case VERTEXATTRIBTYPE_F32:
				__TRY_SELECT(32, _SFLOAT);
				break;

			case VERTEXATTRIBTYPE_I32:
				__TRY_SELECT(32, _SINT);
				break;

			case VERTEXATTRIBTYPE_U32:
				__TRY_SELECT(32, _UINT);
				break;

			case VERTEXATTRIBTYPE_F64:
				__TRY_SELECT(64, _SFLOAT);
				break;

			case VERTEXATTRIBTYPE_I64:
				__TRY_SELECT(64, _SINT);
				break;

			case VERTEXATTRIBTYPE_U64:
				__TRY_SELECT(64, _UINT);
				break;
			}

			#undef __TRY_SELECT
			CA_FATAL("unsupported vertex attribute");
			return VK_FORMAT_UNDEFINED;
		}

		void create_pipeline(pipeline_t * pipeline, framebuffer_t * framebuffer, shader_t * stages, u32 stage_count, uniformsetlayout_t * uniformsetlayout, vertexdecl_t * vdecl)
		{
			device_t * device = framebuffer->device;
			vk_device_t * vk_device = resolve_type(device);
			vk_pipeline_t * vk_pipeline = mem::arena_alloc<vk_pipeline_t>(device->arena, 1);
			vk_framebuffer_t * vk_framebuffer = resolve_type(framebuffer);

			VkPipelineShaderStageCreateInfo * pipeline_shaderstages = mem::arena_alloc<VkPipelineShaderStageCreateInfo>(CA_APP_STACK, stage_count);
			for (u32 i = 0; i != stage_count; i++)
			{
				pipeline_shaderstages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				pipeline_shaderstages[i].pNext = nullptr;
				pipeline_shaderstages[i].flags = 0;
				pipeline_shaderstages[i].stage = resolve_stage(stages[i].stage);
				pipeline_shaderstages[i].module = resolve_type(&stages[i])->shader;
				pipeline_shaderstages[i].pName = "main";
				pipeline_shaderstages[i].pSpecializationInfo = nullptr;
			}

			VkPipelineLayoutCreateInfo pipelinelayout_create_info;
			pipelinelayout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelinelayout_create_info.pNext = nullptr;
			pipelinelayout_create_info.flags = 0;
			pipelinelayout_create_info.setLayoutCount = 1;
			pipelinelayout_create_info.pSetLayouts = &resolve_type(uniformsetlayout)->uniformsetlayout;
			pipelinelayout_create_info.pushConstantRangeCount = 0;//TODO late
			pipelinelayout_create_info.pPushConstantRanges = nullptr;//TODO late

			VkResult ret = vkCreatePipelineLayout(vk_device->device, &pipelinelayout_create_info, &vk_device->allocator, &vk_pipeline->layout);
			CA_ASSERT(ret == VK_SUCCESS);

			VkVertexInputBindingDescription * pipeline_vertexbindings = mem::arena_alloc<VkVertexInputBindingDescription>(CA_APP_STACK, vdecl->buffer_count);
			for (u32 i = 0; i != vdecl->buffer_count; i++)
			{
				pipeline_vertexbindings[i].binding = vdecl->buffers[i].binding;
				pipeline_vertexbindings[i].stride = vdecl->buffers[i].stride;
				pipeline_vertexbindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			}

			VkVertexInputAttributeDescription * pipeline_vertexattribs = mem::arena_alloc<VkVertexInputAttributeDescription>(CA_APP_STACK, vdecl->attrib_count);
			for (u32 i = 0; i != vdecl->attrib_count; i++)
			{
				pipeline_vertexattribs[i].location = vdecl->attribs[i].location;
				pipeline_vertexattribs[i].binding = vdecl->buffers[vdecl->attribs[i].buffer_index].binding;
				pipeline_vertexattribs[i].format = resolve_format(&vdecl->attribs[i]);
				pipeline_vertexattribs[i].offset = vdecl->attribs[i].offset;
			}

			VkPipelineVertexInputStateCreateInfo pipeline_vertexinputstate;
			pipeline_vertexinputstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipeline_vertexinputstate.pNext = nullptr;
			pipeline_vertexinputstate.flags = 0;
			pipeline_vertexinputstate.vertexBindingDescriptionCount = vdecl->buffer_count;
			pipeline_vertexinputstate.pVertexBindingDescriptions = pipeline_vertexbindings;
			pipeline_vertexinputstate.vertexAttributeDescriptionCount = vdecl->attrib_count;
			pipeline_vertexinputstate.pVertexAttributeDescriptions = pipeline_vertexattribs;

			VkPipelineInputAssemblyStateCreateInfo pipeline_inputassemblystate;
			pipeline_inputassemblystate.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			pipeline_inputassemblystate.pNext = nullptr;
			pipeline_inputassemblystate.flags = 0;
			pipeline_inputassemblystate.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//TODO topology
			pipeline_inputassemblystate.primitiveRestartEnable = VK_FALSE;

			VkPipelineViewportStateCreateInfo pipeline_viewportstate;
			pipeline_viewportstate.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			pipeline_viewportstate.pNext = nullptr;
			pipeline_viewportstate.flags = 0;
			pipeline_viewportstate.viewportCount = 1;
			pipeline_viewportstate.pViewports = nullptr;// ignored due to dynamic state
			pipeline_viewportstate.scissorCount = 1;
			pipeline_viewportstate.pScissors = nullptr;// ignored due to dynamic state

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
			pipeline_colorblendstate.logicOp = VK_LOGIC_OP_NO_OP;
			pipeline_colorblendstate.attachmentCount = vk_framebuffer->attachment_count;//TODO depth stencil (attachment_count)
			pipeline_colorblendstate.pAttachments = pipeline_colorblendattachments;
			pipeline_colorblendstate.blendConstants[0] = 0.0f;
			pipeline_colorblendstate.blendConstants[1] = 0.0f;
			pipeline_colorblendstate.blendConstants[2] = 0.0f;
			pipeline_colorblendstate.blendConstants[3] = 0.0f;

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
			pipeline_create_info.stageCount = stage_count;
			pipeline_create_info.pStages = pipeline_shaderstages;
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
			mem::arena_free(CA_APP_STACK, pipeline_vertexattribs);
			mem::arena_free(CA_APP_STACK, pipeline_vertexbindings);
			mem::arena_free(CA_APP_STACK, pipeline_shaderstages);

			pipeline->handle = vk_pipeline;
			pipeline->device = device;
			pipeline->type = PIPELINETYPE_GRAPHICS;
		}

		void destroy_pipeline(pipeline_t * pipeline)
		{
			vk_device_t * vk_device = resolve_type(pipeline->device);
			vk_pipeline_t * vk_pipeline = resolve_type(pipeline);

			vkDestroyPipelineLayout(vk_device->device, vk_pipeline->layout, &vk_device->allocator);
			vkDestroyPipeline(vk_device->device, vk_pipeline->pipeline, &vk_device->allocator);

			pipeline->handle = nullptr;
			pipeline->device = nullptr;
			pipeline->type = NUM_PIPELINETYPES;
		}
	}
}
#endif