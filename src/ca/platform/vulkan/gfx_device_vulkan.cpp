#include "ca/gfx_device.h"
#include "ca/platform.h"
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"

#include <vulkan/vulkan.h>

#if CA_PLATFORM_VULKAN
namespace ca
{
	namespace gfx
	{
		template <typename A>
		struct vulkan_allocation_callbacks
		{
			static void * VKAPI_PTR alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				return arena_alloc(arena, size, alignment);
			}

			static void * VKAPI_PTR realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				//TODO
				CA_LOG("realloc missing impl");
				return nullptr;
			}

			static void VKAPI_PTR free(void * pUserData, void * memory)
			{
				A * arena = reinterpret_cast<A *>(pUserData);
				arena_free(arena, memory);
			}
		};

		struct vulkan_device_data
		{
			VkDevice device;
			VkInstance instance;
			VkAllocationCallbacks allocator;
		};

		void create_device(device_t * device, mem::heaparena_t * arena)
		{
			VkResult ret;

			vulkan_device_data data;
			data.instance = VK_NULL_HANDLE;
			data.device = VK_NULL_HANDLE;
			data.allocator.pUserData = arena;
			data.allocator.pfnAllocation = &vulkan_allocation_callbacks<mem::heaparena_t>::alloc;
			data.allocator.pfnReallocation = &vulkan_allocation_callbacks<mem::heaparena_t>::realloc;
			data.allocator.pfnFree = &vulkan_allocation_callbacks<mem::heaparena_t>::free;
			data.allocator.pfnInternalAllocation = nullptr;
			data.allocator.pfnInternalFree = nullptr;

			// INSTANCE PARAMS
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

			// GET INSTANCE
			CA_LOG("vulkan_device: create instance ...");
			ret = vkCreateInstance(&create_instance_info, &data.allocator, &data.instance);
			CA_ASSERT(ret == VK_SUCCESS);

			// GET PHYSICAL DEVICE
			CA_LOG("vulkan_device: scanning physical devices ...");
			i32 physical_device_index = -1;
			u32 physical_device_count = 0;
			ret = vkEnumeratePhysicalDevices(data.instance, &physical_device_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("vulkan_device: found %d physical devices", physical_device_count);

			VkPhysicalDevice * physical_devices = (VkPhysicalDevice *)mem::arena_alloc(arena, physical_device_count * sizeof(VkPhysicalDevice), alignof(VkPhysicalDevice));
			{
				ret = vkEnumeratePhysicalDevices(data.instance, &physical_device_count, physical_devices);
				CA_ASSERT(ret == VK_SUCCESS);

				for (u32 i = 0; i != physical_device_count; i++)
				{
					physical_device_index = i;
					break;
				}

				if (physical_device_index != -1)
				{
					VkPhysicalDevice physical_device = physical_devices[physical_device_index];
					VkPhysicalDeviceProperties physical_device_properties;
					vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
					CA_LOG("vulkan_device: selected %s", physical_device_properties.deviceName);

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
					ret = vkCreateDevice(physical_devices[physical_device_index], &device_create_info, &data.allocator, &data.device);
					CA_ASSERT(ret == VK_SUCCESS);

					CA_LOG("vulkan_device: allocate descriptor ...");
					device->handle = mem::arena_alloc<vulkan_device_data>(arena, 1, 1);
					memcpy(device->handle, &data, sizeof(vulkan_device_data));

					CA_LOG("vulkan_device: READY");
				}
			}
			mem::arena_free(arena, physical_devices);
		}

		void destroy_device(device_t * device)
		{
			vulkan_device_data * data = reinterpret_cast<vulkan_device_data *>(device->handle);

			CA_LOG("vulkan_device: destroy logical device ... ");
			vkDestroyDevice(data->device, &data->allocator);
			CA_LOG("vulkan_device: destroy instance ... ");
			vkDestroyInstance(data->instance, &data->allocator);

			CA_LOG("vulkan_device: free descriptor ...");
			mem::arena_free(device->arena, device->handle);

			CA_LOG("vulkan_device: CLEAN");
		}
	}
}
#endif