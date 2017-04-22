#include "ca/platform.h"

#if CA_PLATFORM_VULKAN
#include "ca/types.h"
#include "ca/core_assert.h"
#include "ca/core_log.h"
#include "ca/gfx/vulkan.h"

namespace ca
{
	namespace gfx
	{
		static void create_instance(VkInstance * instance, VkAllocationCallbacks * allocator)
		{
			u8 const num_instance_extensions = 2;
			char const * instance_extensions[num_instance_extensions] = {
				VK_KHR_SURFACE_EXTENSION_NAME,
			#if CA_PLATFORM_WIN32
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
			#else
				#error unknown platform
			#endif
			};

			VkApplicationInfo application_info;
			application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application_info.pNext = nullptr;
			application_info.pApplicationName = "canopy application";
			application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.pEngineName = "canopy";
			application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			application_info.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo instance_create_info;
			instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instance_create_info.pNext = nullptr;
			instance_create_info.flags = 0;
			instance_create_info.pApplicationInfo = &application_info;
			instance_create_info.enabledLayerCount = 0;
			instance_create_info.ppEnabledLayerNames = nullptr;
			instance_create_info.enabledExtensionCount = num_instance_extensions;
			instance_create_info.ppEnabledExtensionNames = instance_extensions;

			VkResult ret = vkCreateInstance(&instance_create_info, allocator, instance);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		static i32 select_queue_family(VkPhysicalDevice device, mem::heaparena_t * arena)
		{
			i32 queue_family = -1;
			u32 queue_family_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

			VkQueueFlags const required_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
			VkQueueFamilyProperties * queue_family_properties = mem::arena_alloc<VkQueueFamilyProperties>(arena, queue_family_count);
			{
				for (u32 i = 0; i != queue_family_count; i++)
				{
					if (queue_family_properties[i].queueCount < 1)
						continue;

					//TODO check presentation support
					// vkGetPhysicalDeviceWin32PresentationSupportKHR(device, i)
					// vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, ...);

					VkQueueFlags flags = queue_family_properties[i].queueFlags;
					CA_LOG("queue %d flags = %x (graphics? %d, compute? %d)", i, flags, flags & VK_QUEUE_GRAPHICS_BIT, flags & VK_QUEUE_COMPUTE_BIT);
					if ((flags & required_flags) == required_flags)
					{
						queue_family = i;
						break;
					}
				}
			}
			mem::arena_free(arena, queue_family_properties);

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
			u8 const num_device_extensions = 1;
			char const * device_extensions[num_device_extensions] = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			};

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
			device_create_info.enabledExtensionCount = num_device_extensions;
			device_create_info.ppEnabledExtensionNames = device_extensions;
			device_create_info.pEnabledFeatures = &device_features;

			VkResult ret = vkCreateDevice(physical_device, &device_create_info, allocator, logical_device);
			CA_ASSERT(ret == VK_SUCCESS);

			vkGetDeviceQueue(*logical_device, physical_device_queue_family, 0, logical_device_queue);
		}

		void create_device(device_t * device, mem::heaparena_t * arena)
		{
			vk_device_t * vk_device = mem::arena_alloc<vk_device_t>(arena, 1);
			
			vk_device->allocator.pUserData = arena;
			vk_device->allocator.pfnAllocation = &vk_allocation_callbacks<mem::heaparena_t>::alloc;
			vk_device->allocator.pfnReallocation = &vk_allocation_callbacks<mem::heaparena_t>::realloc;
			vk_device->allocator.pfnFree = &vk_allocation_callbacks<mem::heaparena_t>::free;
			vk_device->allocator.pfnInternalAllocation = nullptr;
			vk_device->allocator.pfnInternalFree = nullptr;

			vk_device->instance = VK_NULL_HANDLE;
			vk_device->physical_device = VK_NULL_HANDLE;
			vk_device->device = VK_NULL_HANDLE;
			vk_device->queue = VK_NULL_HANDLE;

			VkPhysicalDevice physical_device;
			u32 physical_device_queue_family;

			CA_LOG("vulkan_device: create instance ... ");
			create_instance(&vk_device->instance, &vk_device->allocator);
			CA_ASSERT(vk_device->instance != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: select physical device ... ");
			select_physical_device(&physical_device, &physical_device_queue_family, vk_device->instance, arena);
			vk_device->physical_device = physical_device;
			CA_ASSERT(physical_device != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: create logical device ... ");
			create_logical_device(&vk_device->device, &vk_device->queue, physical_device, physical_device_queue_family, &vk_device->allocator);
			CA_ASSERT(vk_device->device != VK_NULL_HANDLE);
			CA_ASSERT(vk_device->queue != VK_NULL_HANDLE);

			device->handle = vk_device;
			device->arena = arena;			
			CA_LOG("vulkan_device: READY");
		}

		void destroy_device(device_t * device)
		{
			vk_device_t * vulkan_device = resolve_device(device);

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