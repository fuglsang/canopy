#pragma once

#include "ca/gfx_device.h"
#include "ca/gfx_semaphore.h"
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
			u32 max_chain_length;
		};

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode mode);
		void destroy_swapchain(swapchain_t * swapchain);

		void swapchain_acquire_blocking(swapchain_t * swapchain);
		void swapchain_acquire(swapchain_t * swapchain, semaphore_t * signal_semaphore);
		void swapchain_present(swapchain_t * swapchain, semaphore_t * wait_semaphore);
	}
}