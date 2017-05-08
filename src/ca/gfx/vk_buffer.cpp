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
		void create_buffer(buffer_t * buffer, device_t * device, buffertype type, size_t size)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_buffer_t * vk_buffer = mem::arena_alloc<vk_buffer_t>(device->arena, 1);

			VkBufferCreateFlags buffer_create_flags = 0;// non-sparse

			VkBufferUsageFlags buffer_usage_flags;
			switch (type)
			{
			case BUFFERTYPE_INDEX:
				buffer_usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
				buffer_usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
				break;
			case BUFFERTYPE_VERTEX:
				buffer_usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
				buffer_usage_flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
				break;
			case BUFFERTYPE_STORAGE:
				buffer_usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
				break;
			case BUFFERTYPE_UNIFORM:
				buffer_usage_flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
				break;
			default:
				CA_ASSERT_MSG(false, "unsupported type");
			}
			
			VkBufferCreateInfo buffer_create_info;
			buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_create_info.pNext = nullptr;
			buffer_create_info.flags = buffer_create_flags;
			buffer_create_info.size = size;
			buffer_create_info.usage = buffer_usage_flags;
			buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			buffer_create_info.queueFamilyIndexCount = 0;
			buffer_create_info.pQueueFamilyIndices = nullptr;

			VkResult ret = vkCreateBuffer(vk_device->device, &buffer_create_info, &vk_device->allocator, &vk_buffer->buffer);
			CA_ASSERT(ret == VK_SUCCESS);

			VkMemoryRequirements memory_requirements;
			vkGetBufferMemoryRequirements(vk_device->device, vk_buffer->buffer, &memory_requirements);

			VkMemoryAllocateInfo memory_allocate_info;
			memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memory_allocate_info.pNext = nullptr;
			memory_allocate_info.allocationSize = memory_requirements.size;
			memory_allocate_info.memoryTypeIndex = 0;//TODO match up with device

			ret = vkAllocateMemory(vk_device->device, &memory_allocate_info, &vk_device->allocator, &vk_buffer->memory);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkBindBufferMemory(vk_device->device, vk_buffer->buffer, vk_buffer->memory, 0);
			CA_ASSERT(ret == VK_SUCCESS);

			buffer->handle = vk_buffer;
			buffer->device = device;
			buffer->type = type;
		}

		void destroy_buffer(buffer_t * buffer)
		{
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			vkFreeMemory(vk_device->device, vk_buffer->memory, &vk_device->allocator);
			vkDestroyBuffer(vk_device->device, vk_buffer->buffer, &vk_device->allocator);

			mem::arena_free(buffer->device->arena, buffer->handle);

			buffer->handle = nullptr;
			buffer->device = nullptr;
		}
	}
}
#endif