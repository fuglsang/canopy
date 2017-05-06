#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_fence.h"
#include "ca/gfx_semaphore.h"
#include "ca/gfx_texture.h"
#include "ca/sys_window.h"

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
			u32 max_buffers_in_flight;
			swapmode preferred_mode;
		};

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode preferred_mode);
		void destroy_swapchain(swapchain_t * swapchain);
		void recreate_swapchain(swapchain_t * swapchain);//TODO maybe not expose this?

		void swapchain_acquire_blocking(swapchain_t * swapchain, texture_t * texture);
		void swapchain_acquire(swapchain_t * swapchain, semaphore_t * signal_semaphore, fence_t * signal_fence, texture_t * texture);
		void swapchain_present(swapchain_t * swapchain, semaphore_t * wait_semaphore);
	}
}