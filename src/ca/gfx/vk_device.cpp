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
		template <typename TArena>
		struct allocation_callbacks
		{
			static void * VKAPI_PTR alloc(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				TArena * arena = reinterpret_cast<TArena *>(pUserData);
				return arena_alloc(arena, size, alignment);
			}

			static void * VKAPI_PTR realloc(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
			{
				CA_FATAL("not implemented");
				return nullptr;
			}

			static void VKAPI_PTR free(void * pUserData, void * memory)
			{
				TArena * arena = reinterpret_cast<TArena *>(pUserData);
				arena_free(arena, memory);
			}
		};

		static VkBool32 VKAPI_PTR debug_report_callback(
			VkDebugReportFlagsEXT                       flags,
			VkDebugReportObjectTypeEXT                  objectType,
			uint64_t                                    object,
			size_t                                      location,
			int32_t                                     messageCode,
			const char*                                 pLayerPrefix,
			const char*                                 pMessage,
			void*                                       pUserData)
		{
			if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
				CA_ERROR("%s", pMessage);
			else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
				CA_WARN("%s", pMessage);
			else
				CA_LOG("%s", pMessage);

			return VK_FALSE;
		}

		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = VK_NULL_HANDLE;
		PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = VK_NULL_HANDLE;

		static void create_instance(VkInstance * instance, VkAllocationCallbacks * allocator)
		{
			u8 const num_instance_layers = 1;
			char const * instance_layers[num_instance_layers] = {
				"VK_LAYER_LUNARG_standard_validation",
			};

			u8 const num_instance_extensions = 3;
			char const * instance_extensions[num_instance_extensions] = {
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
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
			instance_create_info.enabledLayerCount = num_instance_layers;
			instance_create_info.ppEnabledLayerNames = instance_layers;
			instance_create_info.enabledExtensionCount = num_instance_extensions;
			instance_create_info.ppEnabledExtensionNames = instance_extensions;

			VkResult ret = vkCreateInstance(&instance_create_info, allocator, instance);
			CA_ASSERT(ret == VK_SUCCESS);

			vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugReportCallbackEXT");
			vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(*instance, "vkDestroyDebugReportCallbackEXT");
		}

		static void create_debug_callback(VkDebugReportCallbackEXT * debug_callback, VkInstance instance, VkAllocationCallbacks * allocator)
		{
			VkDebugReportFlagsEXT debug_report_flags = 0;
			debug_report_flags |= VK_DEBUG_REPORT_ERROR_BIT_EXT;
			debug_report_flags |= VK_DEBUG_REPORT_WARNING_BIT_EXT;
			debug_report_flags |= VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

			VkDebugReportCallbackCreateInfoEXT debug_report_create_info;
			debug_report_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_create_info.pNext = nullptr;
			debug_report_create_info.flags = debug_report_flags;
			debug_report_create_info.pfnCallback = debug_report_callback;
			debug_report_create_info.pUserData = nullptr;

			VkResult ret = vkCreateDebugReportCallbackEXT(instance, &debug_report_create_info, allocator, debug_callback);
			CA_ASSERT(ret == VK_SUCCESS);
		}

		static i32 select_queue_family(VkPhysicalDevice device)
		{
			i32 queue_family = -1;
			u32 queue_family_count;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

			VkQueueFlags const required_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
			VkQueueFamilyProperties * queue_family_properties = mem::arena_alloc<VkQueueFamilyProperties>(CA_APP_STACK, queue_family_count);
			{
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties);

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
			mem::arena_free(CA_APP_STACK, queue_family_properties);

			return queue_family;
		}

		static void select_physical_device(VkPhysicalDevice * selected_device, u32 * selected_queue_family, VkInstance instance)
		{
			u32 device_count = 0;
			i32 device_score = -1;
			i32 device_index = -1;
			i32 device_queue_family = -1;

			VkResult ret = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
			CA_ASSERT(ret == VK_SUCCESS);
			CA_LOG("vulkan_device: detected %d devices", device_count);

			VkPhysicalDevice * devices = mem::arena_alloc<VkPhysicalDevice>(CA_APP_STACK, device_count);
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

					i32 queue_family = select_queue_family(devices[i]);
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
					CA_ERROR("vulkan_device: no suitable physical device!");
					*selected_device = VK_NULL_HANDLE;
					*selected_queue_family = 0;
				}
			}
			mem::arena_free(CA_APP_STACK, devices);
		}

		static void create_logical_device(VkDevice * logical_device, VkQueue * logical_device_queue, VkPhysicalDevice physical_device, u32 physical_device_queue_family, VkAllocationCallbacks * allocator)
		{
			u8 const num_device_extensions = 2;
			char const * device_extensions[num_device_extensions] = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				//VK_KHR_MAINTENANCE1_EXTENSION_NAME,
				VK_NV_GLSL_SHADER_EXTENSION_NAME,
			};

			f32 device_queue_priority = 1.0f;
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
			CA_LOG("ret = %d", ret);
			CA_ASSERT(ret == VK_SUCCESS);

			vkGetDeviceQueue(*logical_device, physical_device_queue_family, 0, logical_device_queue);
		}

		void create_device(device_t * device, mem::heaparena_t * arena)
		{
			vk_device_t * vk_device = mem::arena_alloc<vk_device_t>(arena, 1);
			
			vk_device->allocator.pUserData = arena;
			vk_device->allocator.pfnAllocation = &allocation_callbacks<mem::heaparena_t>::alloc;
			vk_device->allocator.pfnReallocation = &allocation_callbacks<mem::heaparena_t>::realloc;
			vk_device->allocator.pfnFree = &allocation_callbacks<mem::heaparena_t>::free;
			vk_device->allocator.pfnInternalAllocation = nullptr;
			vk_device->allocator.pfnInternalFree = nullptr;

			vk_device->instance = VK_NULL_HANDLE;
			vk_device->physical_device = VK_NULL_HANDLE;
			vk_device->device = VK_NULL_HANDLE;
			vk_device->queue = VK_NULL_HANDLE;

			CA_LOG("vulkan_device: create instance ... ");
			create_instance(&vk_device->instance, &vk_device->allocator);
			CA_ASSERT(vk_device->instance != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: create debug callback ... ");
			create_debug_callback(&vk_device->debug_callback, vk_device->instance, &vk_device->allocator);
			CA_ASSERT(vk_device->debug_callback != VK_NULL_HANDLE);

			CA_LOG("vulkan_device: select physical device ... ");
			select_physical_device(&vk_device->physical_device, &vk_device->queue_family, vk_device->instance);
			CA_ASSERT(vk_device->physical_device != VK_NULL_HANDLE);

			vkGetPhysicalDeviceFeatures(vk_device->physical_device, &vk_device->device_features);
			vkGetPhysicalDeviceMemoryProperties(vk_device->physical_device, &vk_device->device_memory_props);
			vkGetPhysicalDeviceProperties(vk_device->physical_device, &vk_device->device_props);

			CA_LOG("vulkan_device: create logical device ... ");
			create_logical_device(&vk_device->device, &vk_device->queue, vk_device->physical_device, vk_device->queue_family, &vk_device->allocator);
			CA_ASSERT(vk_device->device != VK_NULL_HANDLE);
			CA_ASSERT(vk_device->queue != VK_NULL_HANDLE);

			device->handle = vk_device;
			device->arena = arena;
			CA_LOG("vulkan_device: READY");
		}

		void destroy_device(device_t * device)
		{
			vk_device_t * vk_device = resolve_type(device);

			CA_LOG("vulkan_device: destroy logical device ... ");
			vkDestroyDevice(vk_device->device, &vk_device->allocator);

			CA_LOG("vulkan_device: destroy debug callback ... ");
			vkDestroyDebugReportCallbackEXT(vk_device->instance, vk_device->debug_callback, &vk_device->allocator);

			CA_LOG("vulkan_device: destroy instance ... ");
			vkDestroyInstance(vk_device->instance, &vk_device->allocator);

			mem::arena_free(device->arena, vk_device);
			CA_LOG("vulkan_device: CLEAN");

			device->handle = nullptr;
			device->arena = nullptr;
		}

		void device_flush(device_t * device)
		{
			VkResult ret = vkDeviceWaitIdle(resolve_handle(device));
			CA_ASSERT(ret == VK_SUCCESS);
		}

		void device_submit(device_t * device, cmdbuffer_t * cmdbuffer, semaphore_t * wait_semaphore, semaphore_t * signal_semaphore, fence_t * signal_fence)
		{
			vk_device_t * vk_device = resolve_type(device);
			vk_cmdbuffer_t * vk_cmdbuffer = resolve_type(cmdbuffer);

			VkSemaphore wait = resolve_handle(wait_semaphore);
			u32 wait_count = (wait != VK_NULL_HANDLE) ? 1 : 0;

			VkPipelineStageFlags wait_stage_mask = 0;
			wait_stage_mask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

			VkSemaphore signal = resolve_handle(signal_semaphore);
			u32 signal_count = (signal != VK_NULL_HANDLE) ? 1 : 0;

			VkSubmitInfo submit_info;
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.pNext = nullptr;
			submit_info.waitSemaphoreCount = wait_count;
			submit_info.pWaitSemaphores = &wait;
			submit_info.pWaitDstStageMask = &wait_stage_mask;
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &vk_cmdbuffer->cmdbuffer;
			submit_info.signalSemaphoreCount = signal_count;
			submit_info.pSignalSemaphores = &signal;

			VkResult ret = vkQueueSubmit(vk_device->queue, 1, &submit_info, resolve_handle(signal_fence));
			CA_ASSERT(ret == VK_SUCCESS);
		}
	}
}
#endif