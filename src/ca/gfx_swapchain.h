#pragma once

#include "ca/gfx_device.h"
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
		};

		void create_swapchain(swapchain_t * swapchain, device_t * device, sys::window_t * window, swapmode mode);
		void destroy_swapchain(swapchain_t * swapchain);
	}
}