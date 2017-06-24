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
		char const * WINDOW_NAME = "ca_window";
		char const * WINDOW_DATA = "ca_window_data";

		static HWND resolve_handle(window_t * window)
		{
			return reinterpret_cast<HWND>(window->handle);
		}

		static window_t * resolve_window(HWND hWnd)
		{
			return reinterpret_cast<window_t *>(GetProp(hWnd, WINDOW_DATA));
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
					SetProp(hWnd, WINDOW_DATA, window);
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
				window->rect.x = LOWORD(lParam);
				window->rect.y = HIWORD(lParam);
				core::event_dispatch(&window->event, window, WINDOWEVENT_MOVED);
				goto default_proc;

			case WM_SIZE:
				window = resolve_window(hWnd);
				window->rect.width = LOWORD(lParam);
				window->rect.height = HIWORD(lParam);
				core::event_dispatch(&window->event, window, WINDOWEVENT_RESIZED);
				goto default_proc;

			default: default_proc:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			return 0;
		}

		static void adjust_coords_accomodate_style(windowrect_t * rect, DWORD style)
		{
			RECT winrect;
			winrect.left = rect->x;
			winrect.top = rect->y;
			winrect.right = rect->x + rect->width;
			winrect.bottom = rect->y + rect->height;

			BOOL ret = AdjustWindowRect(&winrect, style, FALSE);
			CA_ASSERT(ret == TRUE);

			rect->x = winrect.left;
			rect->y = winrect.top;
			rect->width = winrect.right - winrect.left;
			rect->height = winrect.bottom - winrect.top;
		}

		void create_window(window_t * window, char const * title, windowrect_t rect)
		{
			WNDCLASS wc = {};
			wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WndProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.lpszClassName = WINDOW_NAME;

			ATOM ret = RegisterClass(&wc);
			CA_ASSERT_MSG(ret != 0, "RegisterClass (%s) FAILED", title);

			DWORD const style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
			adjust_coords_accomodate_style(&rect, style);

			window->handle = nullptr;
			window->rect = rect;
			window->system_requested_close = false;

			core::create_event(&window->event);

			HWND hWnd = CreateWindow(
				WINDOW_NAME,				// window class
				title,						// window title
				style,						// style
				rect.x,						// pos x
				rect.y,						// pos y
				rect.width,					// dim x
				rect.height,				// dim y
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
			window->rect = { 0, 0, 0, 0 };
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

		void window_move(window_t * window, windowrect_t rect)
		{
			HWND hWnd = resolve_handle(window);
			SetWindowPos(hWnd, NULL, rect.x, rect.y, rect.width, rect.height, 0);
		}

		void window_sync_compositor()
		{
			HRESULT ret = DwmFlush();
			CA_ASSERT(ret == S_OK);
		}
	}
}
#endif