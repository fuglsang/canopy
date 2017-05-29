#pragma once

#include "ca/mem_heaparena.h"
#include "ca/mem_stackarena.h"

#define CA_APP_HEAP (&mem::app_heap)
#define CA_APP_STACK (&mem::app_stack)

namespace ca
{
	namespace mem
	{
		extern heaparena_mt_t app_heap;// thread safe
		extern thread_local stackarena_t app_stack;// thread local

		void initialize(size_t heap_size, size_t stack_size_per_thread);
		void initialize_tls();

		void finalize();
		void finalize_tls();
	}
}