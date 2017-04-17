#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx_device.h"
#include "ca/gfx/vulkan.h"

namespace ca
{
	namespace gfx
	{
		struct queue_indices_t
		{
			u32 graphic;
			u32 compute;
		};

		queue_indices_t select_queue_indices(VkPhysicalDevice device, mem::heaparena_t * arena)
		{
			queue_indices_t queue_indices;
			queue_indices.graphic = -1;
			queue_indices.compute = -1;

			u32 queue_properties_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_properties_count, nullptr);

			VkQueueFamilyProperties * queue_properties = mem::arena_alloc<VkQueueFamilyProperties>(arena, queue_properties_count);
			{
				for (u32 i = 0; i != queue_properties_count; i++)
				{
					if (queue_properties[i].queueCount < 1)
						continue;
					if (queue_indices.graphic == -1 && queue_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						queue_indices.graphic = i;
					if (queue_indices.compute == -1 && queue_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
						queue_indices.compute = i;
					if (queue_indices.graphic != -1 && queue_indices.compute != -1)
						break;
				}
			}
			mem::arena_free(arena, queue_properties);

			return queue_indices;
		}

		VkPhysicalDevice select_physical_device(VkInstance instance, mem::heaparena_t * arena)
		{
			VkResult ret;

			CA_LOG("vulkan_device: select physical device ...");
			VkPhysicalDevice device = VK_NULL_HANDLE;
			u32 device_count = 0;
			i32 device_score = -1;
			i32 device_index = -1;

			ret = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("vulkan_device: %d physical devices", device_count);

			VkPhysicalDevice * devices = mem::arena_alloc<VkPhysicalDevice>(arena, device_count);
			{
				ret = vkEnumeratePhysicalDevices(instance, &device_count, devices);
				CA_ASSERT(ret == VK_SUCCESS);

				VkPhysicalDeviceFeatures device_features;
				VkPhysicalDeviceProperties device_properties;

				for (u32 i = 0; i != device_count; i++)
				{
					vkGetPhysicalDeviceFeatures(devices[i], &device_features);
					vkGetPhysicalDeviceProperties(devices[i], &device_properties);

					i32 score = -1;
					if (device_features.geometryShader)
					{
						switch (device_properties.deviceType)
						{
						case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
							score = 8; break;
						case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
							score = 4; break;
						case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
							score = 2; break;
						case VK_PHYSICAL_DEVICE_TYPE_CPU:
							score = 1; break;
						}
					}

					CA_LOG("vulkan_device: index %d: %s (score %d)", i, device_properties.deviceName, score);
					if (score > device_score)
					{
						queue_indices_t queue_indices = select_queue_indices(devices[i], arena);
						CA_LOG("vulkan_device: index %d: graphics queue %d, compute queue %d", i, queue_indices.graphic, queue_indices.compute);

						if (queue_indices.graphic != -1 && queue_indices.compute != -1)
						{
							device_score = score;
							device_index = i;
						}
					}
				}

				if (device_index != -1)
				{
					CA_LOG("vulkan_device: selected index %d", device_index);
					device = devices[device_index];
				}
				else
				{
					CA_LOG("vulkan_device: no suitable physical devices");
				}
			}
			mem::arena_free(arena, devices);

			return device;
		}

		void create_device(device_t * device, mem::heaparena_t * arena)
		{
			VkResult ret;

			vulkan_device_t vulkan_device;
			vulkan_device.instance = VK_NULL_HANDLE;
			vulkan_device.device = VK_NULL_HANDLE;
			vulkan_device.allocator.pUserData = arena;
			vulkan_device.allocator.pfnAllocation = &vulkan_allocation_callbacks<mem::heaparena_t>::alloc;
			vulkan_device.allocator.pfnReallocation = &vulkan_allocation_callbacks<mem::heaparena_t>::realloc;
			vulkan_device.allocator.pfnFree = &vulkan_allocation_callbacks<mem::heaparena_t>::free;
			vulkan_device.allocator.pfnInternalAllocation = nullptr;
			vulkan_device.allocator.pfnInternalFree = nullptr;

			VkApplicationInfo application_info;
			application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application_info.pNext = nullptr;
			application_info.pApplicationName = "test";
			application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.pEngineName = "canopy";
			application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo create_instance_info;
			create_instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_instance_info.pNext = nullptr;
			create_instance_info.flags = 0;
			create_instance_info.pApplicationInfo = &application_info;
			create_instance_info.enabledLayerCount = 0;
			create_instance_info.ppEnabledLayerNames = nullptr;
			create_instance_info.enabledExtensionCount = 0;
			create_instance_info.ppEnabledExtensionNames = nullptr;

			// CREATE INSTANCE
			CA_LOG("vulkan_device: create instance ...");
			ret = vkCreateInstance(&create_instance_info, &vulkan_device.allocator, &vulkan_device.instance);
			CA_ASSERT(ret == VK_SUCCESS);

			// SELECT PHYSICAL DEVICE
			VkPhysicalDevice physical_device = select_physical_device(vulkan_device.instance, arena);
			CA_ASSERT(physical_device != VK_NULL_HANDLE);

			// CREATE LOGICAL DEVICE
			VkDeviceCreateInfo device_create_info;
			device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_create_info.pNext = nullptr;
			device_create_info.flags = 0;
			device_create_info.queueCreateInfoCount = 0;//TODO
			device_create_info.pQueueCreateInfos = nullptr;//TODO
			device_create_info.enabledLayerCount = 0;
			device_create_info.ppEnabledLayerNames = nullptr;
			device_create_info.enabledExtensionCount = 0;
			device_create_info.ppEnabledExtensionNames = nullptr;
			device_create_info.pEnabledFeatures = 0;//TODO
					
			CA_LOG("vulkan_device: create logical device ...");
			ret = vkCreateDevice(physical_device, &device_create_info, &vulkan_device.allocator, &vulkan_device.device);
			CA_ASSERT(ret == VK_SUCCESS);

			CA_LOG("vulkan_device: allocate descriptor ...");
			device->handle = mem::arena_alloc<vulkan_device_t>(arena, 1);
			device->arena = arena;
			memcpy(device->handle, &vulkan_device, sizeof(vulkan_device_t));

			CA_LOG("vulkan_device: READY");
		}

		void destroy_device(device_t * device)
		{
			vulkan_device_t * vulkan_device = resolve_device(device->handle);

			CA_LOG("vulkan_device: destroy logical device ... ");
			vkDestroyDevice(vulkan_device->device, &vulkan_device->allocator);
			CA_LOG("vulkan_device: destroy instance ... ");
			vkDestroyInstance(vulkan_device->instance, &vulkan_device->allocator);

			CA_LOG("vulkan_device: free descriptor ...");
			mem::arena_free(device->arena, device->handle);

			CA_LOG("vulkan_device: CLEAN");
		}
	}
}
#endif