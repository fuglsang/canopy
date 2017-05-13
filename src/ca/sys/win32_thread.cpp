#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys_thread.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace sys
	{
		DWORD WINAPI win32_delegate(LPVOID lpParam)
		{
			thread_t * thread = static_cast<thread_t *>(lpParam);
			thread->state = THREADSTATE_RUNNING;
			thread->action();
			return 0;
		}

		void create_thread(thread_t * thread, core::action_t<> action)
		{
			thread->state = THREADSTATE_INITIALIZING;
			thread->action = action;
			thread->handle = CreateThread(
				NULL,			// default security attribs
				0,				// default stack size
				win32_delegate,	// start address
				thread,			// start parameters
				0,				// run immediately
				NULL);			// no identifier

			CA_ASSERT(thread->handle != NULL);
		}

		void destroy_thread(thread_t * thread)
		{
			BOOL ret = CloseHandle(thread->handle);
			CA_ASSERT(ret != 0);
		}

		void thread_sleep(u64 duration_milli)
		{
			Sleep((DWORD)duration_milli);
		}

		void thread_suspend(thread_t * thread)
		{
			CA_ASSERT(thread->state == THREADSTATE_RUNNING);
			DWORD ret = SuspendThread(thread->handle);
			CA_ASSERT(ret != -1);
			thread->state = THREADSTATE_SUSPENDED;
		}

		void thread_resume(thread_t * thread)
		{
			CA_ASSERT(thread->state == THREADSTATE_SUSPENDED);
			DWORD ret = ResumeThread(thread->handle);
			CA_ASSERT(ret != -1);
			thread->state = THREADSTATE_RUNNING;
		}

		void thread_join(thread_t * thread)
		{
			CA_ASSERT(thread->state == THREADSTATE_RUNNING);
			DWORD ret = WaitForSingleObject(thread->handle, INFINITE);
			CA_ASSERT(ret != WAIT_OBJECT_0);
			thread->state = THREADSTATE_JOINED;
		}
	}
}
#endif