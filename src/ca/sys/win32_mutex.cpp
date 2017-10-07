#if CA_TARGET_SYS_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_mutex.h"
#include "ca/core_assert.h"

namespace ca
{
	namespace sys
	{
		void create_mutex(mutex_t * mutex)
		{
			mutex->handle = CreateMutex(0, FALSE, NULL);
			CA_ASSERT(mutex->handle != NULL);
		}

		void destroy_mutex(mutex_t * mutex)
		{
			BOOL ret = CloseHandle(mutex->handle);
			CA_ASSERT(ret != 0);
		}

		void mutex_lock(mutex_t * mutex)
		{
			DWORD ret = WaitForSingleObject(mutex->handle, INFINITE);
			CA_ASSERT(ret == WAIT_OBJECT_0);
		}

		void mutex_unlock(mutex_t * mutex)
		{
			BOOL ret = ReleaseMutex(mutex->handle);
			CA_ASSERT(ret != 0);
		}
	}
}
#endif