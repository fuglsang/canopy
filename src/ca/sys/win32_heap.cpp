#include "ca/sys_heap.h"
#include "ca/platform.h"

#if CA_PLATFORM_WINDOWS
namespace ca
{
	namespace sys
	{
		void * heap_alloc(size_t size)
		{
			HANDLE heap = GetProcessHeap();
			return HeapAlloc(heap, 0, size);
		}

		void heap_free(void * block)
		{
			HANDLE heap = GetProcessHeap();
			HeapFree(heap, 0, block);
		}
	}
}
#endif