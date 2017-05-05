#pragma once

#include "ca/types.h"
#include "ca/core_action.h"

namespace ca
{
	namespace sys
	{
		enum windowevent
		{
			WINDOWEVENT_CLOSED,
			WINDOWEVENT_CREATED,
			WINDOWEVENT_MOVED,
			WINDOWEVENT_RESIZED,
			NUM_WINDOWEVENTS,
		};

		struct windowcoords_t
		{
			i32 x;
			i32 y;
			u32 dx;
			u32 dy;
		};

		struct window_t
		{
			void * handle;
			windowcoords_t coords;
			core::action_t<window_t *, windowevent> ev;//TODO intrusive list
			bool system_requested_close;
		};

		void create_window(window_t * window, char const * title, windowcoords_t coords);
		void destroy_window(window_t * window);

		bool window_poll(window_t * window);
		void window_move(window_t * window, windowcoords_t coords);
		void window_hide(window_t * window);
		void window_show(window_t * window);
	}
}