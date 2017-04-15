#pragma once

namespace ca
{
	namespace sys
	{
		void * process_alloc(size_t size);
		void process_free(void * block);
	}
}