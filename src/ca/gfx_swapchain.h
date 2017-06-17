#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_fence.h"
#include "ca/gfx_semaphore.h"
#include "ca/gfx_texture.h"
#include "ca/sys_window.h"
#include "ca/core_event.h"

namespace ca
{
	namespace gfx
	{
		enum swapmode
		{
			SWAPMODE_FREE,
			SWAPMODE_VSYNC,
			SWAPMODE_VSYNC_SKIP,
			NUM_SWAPMODES,
		};

		struct swapchain_t
		{
			void * handle;
			device_t * device;
			sys::window_t * window;
			swapmode preferred_swapmode;
			u32 width;
			u32 height;
			u32 length;
			texture_t * textures;
			core::event_t<swapchain_t *> recreated;
		};

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode preferred_swapmode);
		void destroy_swapchain(swapchain_t * swapchain);

		void swapchain_acquire_blocking(swapchain_t * swapchain, u32 * acquired_image_index);
		void swapchain_acquire(swapchain_t * swapchain, semaphore_t * signal_semaphore, fence_t * signal_fence, u32 * acquired_image_index);
		void swapchain_present(swapchain_t * swapchain, semaphore_t * wait_semaphore, u32 acquired_image_index);
	}
}