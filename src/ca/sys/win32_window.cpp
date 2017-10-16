#if CA_TARGET_SYS_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_window.h"
#include "ca/core_assert.h"
#include "ca/input_keystate.h"

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

		static input::keycode resolve_key(WPARAM wParam)
		{
			// win32 keycodes
			// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
			
			switch (wParam)
			{
			// 0--9
			case 0x30:		return input::KEY_0;
			case 0x31:		return input::KEY_1;
			case 0x32:		return input::KEY_2;
			case 0x33:		return input::KEY_3;
			case 0x34:		return input::KEY_4;
			case 0x35:		return input::KEY_5;
			case 0x36:		return input::KEY_6;
			case 0x37:		return input::KEY_7;
			case 0x38:		return input::KEY_8;
			case 0x39:		return input::KEY_9;

			// a--z
			case 0x41:		return input::KEY_A;
			case 0x42:		return input::KEY_B;
			case 0x43:		return input::KEY_C;
			case 0x44:		return input::KEY_D;
			case 0x45:		return input::KEY_E;
			case 0x46:		return input::KEY_F;
			case 0x47:		return input::KEY_G;
			case 0x48:		return input::KEY_H;
			case 0x49:		return input::KEY_I;
			case 0x4A:		return input::KEY_J;
			case 0x4B:		return input::KEY_K;
			case 0x4C:		return input::KEY_L;
			case 0x4D:		return input::KEY_M;
			case 0x4E:		return input::KEY_N;
			case 0x4F:		return input::KEY_O;
			case 0x50:		return input::KEY_P;
			case 0x51:		return input::KEY_Q;
			case 0x52:		return input::KEY_R;
			case 0x53:		return input::KEY_S;
			case 0x54:		return input::KEY_T;
			case 0x55:		return input::KEY_U;
			case 0x56:		return input::KEY_V;
			case 0x57:		return input::KEY_W;
			case 0x58:		return input::KEY_X;
			case 0x59:		return input::KEY_Y;
			case 0x5A:		return input::KEY_Z;

			// f1--f12
			case VK_F1:		return input::KEY_F1;
			case VK_F2:		return input::KEY_F2;
			case VK_F3:		return input::KEY_F3;
			case VK_F4:		return input::KEY_F4;
			case VK_F5:		return input::KEY_F5;
			case VK_F6:		return input::KEY_F6;
			case VK_F7:		return input::KEY_F7;
			case VK_F8:		return input::KEY_F8;
			case VK_F9:		return input::KEY_F9;
			case VK_F10:	return input::KEY_F10;
			case VK_F11:	return input::KEY_F11;
			case VK_F12:	return input::KEY_F12;

			// cursor
			case VK_LEFT:	return input::KEY_LEFT;
			case VK_UP:		return input::KEY_UP;
			case VK_RIGHT:	return input::KEY_RIGHT;
			case VK_DOWN:	return input::KEY_DOWN;

			// modifier
			case VK_MENU:	return input::KEY_ALT;
			case VK_CONTROL:return input::KEY_CTRL;
			case VK_SHIFT:	return input::KEY_SHIFT;

			// operator
			case VK_BACK:	return input::KEY_BACKSPACE;
			case VK_DELETE:	return input::KEY_DELETE;
			case VK_RETURN:	return input::KEY_ENTER;
			case VK_ESCAPE:	return input::KEY_ESCAPE;
			case VK_SPACE:	return input::KEY_SPACE;
			case VK_TAB:	return input::KEY_TAB;
			}

			// unknown
			return input::KEY_UNKNOWN;
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
				{
					window = resolve_window(hWnd);
					input::keycode key = resolve_key(wParam);
					input::keystate_register_key_down(&window->keystate, key);
				}
				goto default_proc;

			case WM_KEYUP:
				{
					window = resolve_window(hWnd);
					input::keycode key = resolve_key(wParam);
					input::keystate_register_key_up(&window->keystate, key);
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

			DWORD const style = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
			adjust_coords_accomodate_style(&rect, style);

			window->handle = nullptr;
			window->rect = rect;
			window->system_requested_close = false;

			core::create_event(&window->event);

			input::keystate_reset(&window->keystate);

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

			input::keystate_rollover(&window->keystate);

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