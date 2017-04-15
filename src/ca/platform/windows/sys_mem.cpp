#include "ca/sys_mem.h"
#include "ca/platform.h"

#if CA_PLATFORM_WINDOWS
namespace ca
{
	namespace sys
	{
		void * process_alloc(size_t size)
		{
			HANDLE heapHandle = GetProcessHeap();
			return HeapAlloc(heapHandle, 0, size);
		}

		void process_free(void * block)
		{
			HANDLE heapHandle = GetProcessHeap();
			HeapFree(heapHandle, 0, block);
		}
	}
}
#endif