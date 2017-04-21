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
		static void create_instance(VkInstance * instance, VkAllocationCallbacks * allocator)
		{
			VkApplicationInfo application_info;
			application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application_info.pNext = nullptr;
			application_info.pApplicationName = "canopy application";
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

			VkResult ret = vkCreateInstance(&create_instance_info, allocator, instance);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		static void create_surface(VkSurfaceKHR * surface, VkInstance instance, VkAllocationCallbacks * allocator)
		{
		#if VK_KHR_win32_surface			
			VkWin32SurfaceCreateInfoKHR create_info;
			create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			create_info.pNext = nullptr;
			create_info.flags = 0;
			create_info.hinstance = GetModuleHandle(nullptr);
			create_info.hwnd = 0;// TODO get window handle

			VkResult ret = vkCreateWin32SurfaceKHR(instance, &create_info, allocator, surface);
			CA_ASSERT(ret == VK_SUCCESS);
		#else
			#error unknown platform
		#endif
		}

		static i32 select_queue_family(VkPhysicalDevice device, mem::heaparena_t * arena)
		{
			i32 queue_family = -1;
			u32 queue_properties_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_properties_count, nullptr);

			VkQueueFlags const queue_family_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
			VkQueueFamilyProperties * queue_properties = mem::arena_alloc<VkQueueFamilyProperties>(arena, queue_properties_count);
			{
				for (u32 i = 0; i != queue_properties_count; i++)
				{
					if (queue_family_flags == (queue_family_flags & queue_properties[i].queueFlags) && queue_properties[i].queueCount > 0)
					{
						queue_family = i;
						break;
					}
				}
			}
			mem::arena_free(arena, queue_properties);

			return queue_family;
		}

		static void select_physical_device(VkPhysicalDevice * selected_device, u32 * selected_queue_family, VkInstance instance, mem::heaparena_t * arena)
		{
			u32 device_count = 0;
			i32 device_score = -1;
			i32 device_index = -1;
			i32 device_queue_family = -1;

			VkResult ret = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("vulkan_device: detected %d devices", device_count);

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

					i32 queue_family = select_queue_family(devices[i], arena);
					if (queue_family != -1 && score > device_score)
					{
						device_score = score;
						device_index = i;
						device_queue_family = queue_family;
					}
					CA_LOG("vulkan_device: device %d: '%s', score %d, queue family %d", i, device_properties.deviceName, score, queue_family);
				}

				if (device_index != -1)
				{
					CA_LOG("vulkan_device: device %d <-- selected", device_index);
					*selected_device = devices[device_index];
					*selected_queue_family = device_queue_family;
				}
				else
				{
					CA_LOG("vulkan_device: no suitable devices");
					*selected_device = VK_NULL_HANDLE;
					*selected_queue_family = 0;
				}
			}
			mem::arena_free(arena, devices);
		}

		static void create_logical_device(VkDevice * logical_device, VkQueue * logical_device_queue, VkPhysicalDevice physical_device, u32 physical_device_queue_family, VkAllocationCallbacks * allocator)
		{
			float device_queue_priority = 1.0f;
			VkPhysicalDeviceFeatures device_features = {};

			VkDeviceQueueCreateInfo device_queue_create_info;
			device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			device_queue_create_info.pNext = nullptr;
			device_queue_create_info.flags = 0;
			device_queue_create_info.queueFamilyIndex = physical_device_queue_family;
			device_queue_create_info.queueCount = 1;
			device_queue_create_info.pQueuePriorities = &device_queue_priority;

			VkDeviceCreateInfo device_create_info;
			device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_create_info.pNext = nullptr;
			device_create_info.flags = 0;
			device_create_info.queueCreateInfoCount = 1;
			device_create_info.pQueueCreateInfos = &device_queue_create_info;
			device_create_info.enabledLayerCount = 0;
			device_create_info.ppEnabledLayerNames = nullptr;
			device_create_info.enabledExtensionCount = 0;
			device_create_info.ppEnabledExtensionNames = nullptr;
			device_create_info.pEnabledFeatures = &device_features;

			VkResult ret = vkCreateDevice(physical_device, &device_create_info, allocator, logical_device);
			CA_ASSERT(ret == VK_SUCCESS);

			vkGetDeviceQueue(*logical_device, physical_device_queue_family, 0, logical_device_queue);
		}

		void create_device(device_t * device, mem::heaparena_t * arena)
		{
			vulkan_device_t * device_handles = mem::arena_alloc<vulkan_device_t>(arena, 1);
			
			device_handles->allocator.pUserData = arena;
			device_handles->allocator.pfnAllocation = &vulkan_allocation_callbacks<mem::heaparena_t>::alloc;
			device_handles->allocator.pfnReallocation = &vulkan_allocation_callbacks<mem::heaparena_t>::realloc;
			device_handles->allocator.pfnFree = &vulkan_allocation_callbacks<mem::heaparena_t>::free;
			device_handles->allocator.pfnInternalAllocation = nullptr;
			device_handles->allocator.pfnInternalFree = nullptr;

			device_handles->instance = VK_NULL_HANDLE;
			device_handles->device = VK_NULL_HANDLE;
			device_handles->queue = VK_NULL_HANDLE;

			VkPhysicalDevice physical_device;
			u32 physical_device_queue_family;

			CA_LOG("vulkan_device: create instance ...");
			create_instance(&device_handles->instance, &device_handles->allocator);
			CA_ASSERT(device_handles->instance != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: select physical device ...");
			select_physical_device(&physical_device, &physical_device_queue_family, device_handles->instance, arena);
			CA_ASSERT(physical_device != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: create logical device ...");
			create_logical_device(&device_handles->device, &device_handles->queue, physical_device, physical_device_queue_family, &device_handles->allocator);
			CA_ASSERT(device_handles->device != VK_NULL_HANDLE);
			CA_ASSERT(device_handles->queue != VK_NULL_HANDLE);

			device->handle = device_handles;
			device->arena = arena;			
			CA_LOG("vulkan_device: READY");
		}

		void destroy_device(device_t * device)
		{
			vulkan_device_t * vulkan_device = resolve_device(device->handle);

			CA_LOG("vulkan_device: destroy logical device ... ");
			vkDestroyDevice(vulkan_device->device, &vulkan_device->allocator);

			CA_LOG("vulkan_device: destroy instance ... ");
			vkDestroyInstance(vulkan_device->instance, &vulkan_device->allocator);

			mem::arena_free(device->arena, device->handle);
			CA_LOG("vulkan_device: CLEAN");
		}
	}
}
#endif