#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_window.h"
#include "ca/core_assert.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace ca
{
	namespace sys
	{
		char const * window_name = "ca_window";
		char const * window_data = "ca_window_data";

		static HWND resolve_handle(window_t * window)
		{
			return reinterpret_cast<HWND>(window->handle);
		}

		static window_t * resolve_window(HWND hWnd)
		{
			return reinterpret_cast<window_t *>(GetProp(hWnd, window_data));
		}

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			window_t * window;

			switch (message)
			{
			case WM_CLOSE:
				window = resolve_window(hWnd);
				window->system_requested_close = true;
				PostQuitMessage(0);
				core::event_dispatch(&window->event, window, WINDOWEVENT_CLOSED);
				break;
			
			case WM_CREATE:
				{
					CREATESTRUCT * info = reinterpret_cast<CREATESTRUCT *>(lParam);
					window = reinterpret_cast<window_t *>(info->lpCreateParams);
					window->handle = hWnd;
					SetProp(hWnd, window_data, window);
					core::event_dispatch(&window->event, window, WINDOWEVENT_CREATED);
				}
				goto default_proc;

			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE)
				{
					CA_LOG("ESCAPE");
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}
				goto default_proc;

			case WM_MOVE:
				window = resolve_window(hWnd);
				window->coords.x = LOWORD(lParam);
				window->coords.y = HIWORD(lParam);
				core::event_dispatch(&window->event, window, WINDOWEVENT_MOVED);
				goto default_proc;

			case WM_SIZE:
				window = resolve_window(hWnd);
				window->coords.dim_x = LOWORD(lParam);
				window->coords.dim_y = HIWORD(lParam);
				core::event_dispatch(&window->event, window, WINDOWEVENT_RESIZED);
				goto default_proc;

			default: default_proc:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			return 0;
		}

		static void adjust_coords_accomodate_style(windowcoords_t * coords, DWORD style)
		{
			RECT rect;
			rect.left = coords->x;
			rect.top = coords->y;
			rect.right = coords->x + coords->dim_x;
			rect.bottom = coords->y + coords->dim_y;

			BOOL ret = AdjustWindowRect(&rect, style, FALSE);
			CA_ASSERT(ret == TRUE);

			coords->x = rect.left;
			coords->y = rect.top;
			coords->dim_x = rect.right - rect.left;
			coords->dim_y = rect.bottom - rect.top;
		}

		void create_window(window_t * window, char const * title, windowcoords_t coords)
		{
			WNDCLASS wc = {};
			wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WndProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.lpszClassName = window_name;

			ATOM ret = RegisterClass(&wc);
			CA_ASSERT_MSG(ret != 0, "RegisterClass (%s) FAILED", title);

			DWORD const style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			adjust_coords_accomodate_style(&coords, style);

			window->handle = nullptr;
			window->coords = coords;
			window->system_requested_close = false;

			core::create_event(&window->event);

			HWND hWnd = CreateWindow(
				window_name,				// window class
				title,						// window title
				style,						// style
				coords.x,					// pos x
				coords.y,					// pos y
				coords.dim_x,				// dim x
				coords.dim_y,				// dim y
				NULL,						// hwnd parent
				NULL,						// hmenu
				GetModuleHandle(nullptr),	// hinstance process
				window						// userdata for WM_CREATE
			);
			CA_ASSERT_MSG(hWnd != NULL, "CreateWindow (%s) FAILED", title);
		}

		void destroy_window(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			DestroyWindow(hWnd);

			window->handle = nullptr;
			window->coords = { 0, 0, 0, 0 };
			window->system_requested_close = false;

			core::destroy_event(&window->event);
		}

		bool window_poll(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			MSG msg = {};

			while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (window->system_requested_close)
				return false;
			else
				return true;
		}

		void window_hide(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			ShowWindow(hWnd, SW_HIDE);
		}

		void window_show(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			ShowWindow(hWnd, SW_SHOW);
		}

		void window_move(window_t * window, windowcoords_t coords)
		{
			HWND hWnd = resolve_handle(window);
			SetWindowPos(hWnd, NULL, coords.x, coords.y, coords.dim_x, coords.dim_y, 0);
		}

		void window_sync_compositor()
		{
			HRESULT ret = DwmFlush();
			CA_ASSERT(ret == S_OK);
		}
	}
}
#endif