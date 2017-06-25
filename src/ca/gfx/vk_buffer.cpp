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
		i32 select_memory_type_index(VkPhysicalDeviceMemoryProperties * memory_props, buffermemorytype memory_type)
		{
			i32 selected_index = -1;
			i32 selected_index_uma = -1;

			VkMemoryPropertyFlags desired_props = 0;
			VkMemoryPropertyFlags desired_props_uma = 0;

			switch (memory_type)
			{
			case BUFFERMEMORYTYPE_MAPPABLE:
				desired_props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				break;

			case BUFFERMEMORYTYPE_MAPPABLE_COHERENT:
				desired_props |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
				desired_props |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
				break;

			case BUFFERMEMORYTYPE_STAGED:
				desired_props |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				break;

			default:
				CA_FATAL("unsupported buffer memory type");
			}

			desired_props_uma = desired_props | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			for (u32 i = 0; i != memory_props->memoryTypeCount; i++)
			{
				if (selected_index == -1 && (memory_props->memoryTypes[i].propertyFlags & desired_props) == desired_props)
					selected_index = i;
				if (selected_index_uma == -1 && (memory_props->memoryTypes[i].propertyFlags & desired_props_uma) == desired_props_uma)
					selected_index_uma = i;
			}

			if (selected_index_uma != -1)
				return selected_index_uma;
			else
				return selected_index;
		}

		void create_buffer(buffer_t * buffer, device_t * device, buffertype type, buffermemorytype memory_type, size_t size)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_buffer_t * vk_buffer = mem::arena_alloc<vk_buffer_t>(device->arena, 1);

			VkBufferCreateFlags buffer_create_flags = 0;// non-sparse

			VkBufferUsageFlags buffer_usage_flags = 0;
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
				CA_FATAL("unsupported type");
			}
			
			VkBufferCreateInfo buffer_create_info;
			buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_create_info.pNext = nullptr;
			buffer_create_info.flags = buffer_create_flags;
			buffer_create_info.size = size;
			buffer_create_info.usage = buffer_usage_flags;
			buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			buffer_create_info.queueFamilyIndexCount = 0;// ignored due to VK_SHARING_MODE_EXCLUSIVE
			buffer_create_info.pQueueFamilyIndices = nullptr;// ignored due to VK_SHARING_MODE_EXCLUSIVE

			VkResult ret = vkCreateBuffer(vk_device->device, &buffer_create_info, &vk_device->allocator, &vk_buffer->buffer);
			CA_ASSERT(ret == VK_SUCCESS);

			VkMemoryRequirements memory_requirements;
			vkGetBufferMemoryRequirements(vk_device->device, vk_buffer->buffer, &memory_requirements);

			i32 memory_type_index = select_memory_type_index(&vk_device->device_memory_props, memory_type);
			CA_ASSERT(memory_type_index != -1);

			VkMemoryAllocateInfo memory_allocate_info;
			memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			memory_allocate_info.pNext = nullptr;
			memory_allocate_info.allocationSize = memory_requirements.size;
			memory_allocate_info.memoryTypeIndex = memory_type_index;

			ret = vkAllocateMemory(vk_device->device, &memory_allocate_info, &vk_device->allocator, &vk_buffer->memory);
			CA_ASSERT(ret == VK_SUCCESS);

			ret = vkBindBufferMemory(vk_device->device, vk_buffer->buffer, vk_buffer->memory, 0);
			CA_ASSERT(ret == VK_SUCCESS);

			buffer->handle = vk_buffer;
			buffer->device = device;
			buffer->type = type;
			buffer->memory_type = memory_type;
			buffer->size = size;
		}

		void destroy_buffer(buffer_t * buffer)
		{
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			vkFreeMemory(vk_device->device, vk_buffer->memory, &vk_device->allocator);
			vkDestroyBuffer(vk_device->device, vk_buffer->buffer, &vk_device->allocator);

			mem::arena_free(buffer->device->arena, vk_buffer);

			buffer->handle = nullptr;
			buffer->device = nullptr;
			buffer->type = NUM_BUFFERTYPES;
			buffer->memory_type = NUM_BUFFERMEMORYTYPES;
			buffer->size = 0;
		}

		void * buffer_map(buffer_t * buffer, size_t offset, size_t size)
		{
			CA_ASSERT_MSG(buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE || buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE_COHERENT, "buffer is not mappable");
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			void * mapped_base = nullptr;
			
			VkResult ret = vkMapMemory(vk_device->device, vk_buffer->memory, offset, size, 0, &mapped_base);
			CA_ASSERT(ret == VK_SUCCESS);

			return mapped_base;
		}

		void buffer_flush_range(buffer_t * buffer, size_t offset, size_t size)
		{
			CA_ASSERT_MSG(buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE, "buffer is not pure mappable (non-coherent)");
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			VkMappedMemoryRange mapped_memory_range;
			mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mapped_memory_range.pNext = nullptr;
			mapped_memory_range.memory = vk_buffer->memory;
			mapped_memory_range.offset = offset;
			mapped_memory_range.size = size;

			VkResult ret = vkFlushMappedMemoryRanges(vk_device->device, 1, &mapped_memory_range);
			CA_ASSERT(ret = VK_SUCCESS);
		}

		void buffer_invalidate_range(buffer_t * buffer, size_t offset, size_t size)
		{
			CA_ASSERT_MSG(buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE, "buffer is not pure mappable (non-coherent)");
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			VkMappedMemoryRange mapped_memory_range;
			mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mapped_memory_range.pNext = nullptr;
			mapped_memory_range.memory = vk_buffer->memory;
			mapped_memory_range.offset = offset;
			mapped_memory_range.size = size;

			VkResult ret = vkInvalidateMappedMemoryRanges(vk_device->device, 1, &mapped_memory_range);
			CA_ASSERT(ret = VK_SUCCESS);
		}

		void buffer_unmap(buffer_t * buffer)
		{
			CA_ASSERT_MSG(buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE || buffer->memory_type == BUFFERMEMORYTYPE_MAPPABLE_COHERENT, "buffer is not mappable");
			vk_device_t * vk_device = resolve_type(buffer->device);
			vk_buffer_t * vk_buffer = resolve_type(buffer);

			vkUnmapMemory(vk_device->device, vk_buffer->memory);
		}
	}
}
#endif