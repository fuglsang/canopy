#pragma once

namespace ca
{
	namespace sys
	{
		void * heap_alloc(size_t size);
		void heap_free(void * block);
	}
}