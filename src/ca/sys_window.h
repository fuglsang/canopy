#pragma once

#include "ca_base.h"
#include "ca/core_event.h"
#include "ca/input_cursor.h"
#include "ca/input_keycode.h"

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

		struct windowrect_t
		{
			i32 x;
			i32 y;
			u32 width;
			u32 height;
		};

		struct window_t
		{
			void * handle;
			windowrect_t rect;
			bool system_requested_close;
			core::event_t<window_t *, windowevent> event;

			input::cursor_t mouse;
			input::key_t keys[input::NUM_KEYS];
		};

		void create_window(window_t * window, char const * title, windowrect_t rect);
		void destroy_window(window_t * window);

		bool window_poll(window_t * window);
		void window_move(window_t * window, windowrect_t rect);
		void window_hide(window_t * window);
		void window_show(window_t * window);
		void window_sync_compositor();
	}
}