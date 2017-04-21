#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys_window.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace sys
	{
		char const * WINDOW_CLASS = "ca_window_class";
		char const * WINDOW_USERDATA = "ca_window_userdata";

		static HWND resolve_handle(window_t * window)
		{
			return reinterpret_cast<HWND>(window->handle);
		}

		static window_t * resolve_window(HWND hWnd)
		{
			return reinterpret_cast<window_t *>(GetProp(hWnd, WINDOW_USERDATA));
		}

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			window_t * window;

			switch (message)
			{
			case WM_CLOSE:
				CA_LOG("WM_CLOSE");
				PostQuitMessage(0);
				break;
			
			case WM_CREATE:
				CA_LOG("WM_CREATE");
				{
					CREATESTRUCT * info = reinterpret_cast<CREATESTRUCT *>(lParam);
					window = reinterpret_cast<window_t *>(info->lpCreateParams);
					window->handle = hWnd;
					SetProp(hWnd, WINDOW_USERDATA, window);
				}
				goto default_proc;

			case WM_MOVE:
				CA_LOG("WM_MOVE");
				window = resolve_window(hWnd);
				window->coords.x = LOWORD(lParam);
				window->coords.y = HIWORD(lParam);
				goto default_proc;

			case WM_SIZE:
				CA_LOG("WM_SIZE");
				window = resolve_window(hWnd);
				window->coords.dx = LOWORD(lParam);
				window->coords.dy = HIWORD(lParam);
				goto default_proc;

			default: default_proc:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}

			return 0;
		}

		void create_window(window_t * window, char const * title, windowcoords_t coords)
		{
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = WndProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.lpszClassName = WINDOW_CLASS;

			ATOM ret = RegisterClass(&wc);
			CA_ASSERT_MSG(ret != 0, "RegisterClass (%s) FAILED", title);

			HWND hWnd = CreateWindow(
				WINDOW_CLASS,						// window class
				title,								// window title
				WS_OVERLAPPEDWINDOW | WS_VISIBLE,	// style
				coords.x,							// pos x
				coords.y,							// pos y
				coords.dx,							// dim x
				coords.dy,							// dim y
				NULL,								// hwnd parent
				NULL,								// hmenu
				GetModuleHandle(nullptr),			// hinstance process
				window								// userdata for WM_CREATE
			);
			CA_ASSERT_MSG(hWnd != NULL, "CreateWindow (%s) FAILED", title);
		}

		void destroy_window(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			DestroyWindow(hWnd);
		}

		bool window_poll(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			MSG msg = {};

			if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				return true;
			}

			return false;
		}

		bool window_poll_blocking(window_t * window)
		{
			HWND hWnd = resolve_handle(window);
			MSG msg = {};

			if (GetMessage(&msg, hWnd, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				return true;
			}

			return false;
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
			SetWindowPos(hWnd, NULL, coords.x, coords.y, coords.dx, coords.dy, 0);
		}
	}
}
#endif