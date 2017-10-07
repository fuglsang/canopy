#if CA_TARGET_SYS_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_process.h"

namespace ca
{
	namespace sys
	{
		void * process_alloc(size_t size)
		{
			HANDLE heap = GetProcessHeap();
			return HeapAlloc(heap, 0, size);
		}

		void process_free(void * block)
		{
			HANDLE heap = GetProcessHeap();
			HeapFree(heap, 0, block);
		}
	}
}
#endif