#include "ca/sys_mem.h"
#include "ca/platform.h"

#if CA_PLATFORM_WINDOWS
namespace ca
{
	namespace sys
	{
		void * process_alloc(size_t size)
		{
			HANDLE h = GetProcessHeap();
			return HeapAlloc(h, 0, size);
		}

		void process_free(void * block)
		{
			HANDLE h = GetProcessHeap();
			HeapFree(h, 0, block);
		}
	}
}
#endif