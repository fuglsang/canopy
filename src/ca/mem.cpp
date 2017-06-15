#include "ca/core_assert.h"
#include "ca/mem.h"
#include "ca/mem_heaparena.h"
#include "ca/mem_stackarena.h"
#include "ca/sys_process.h"

namespace ca
{
	namespace mem
	{
		void * app_heap_base = nullptr;
		size_t app_heap_size;
		heaparena_mt_t app_heap;
		size_t app_stack_size;
		thread_local stackarena_t app_stack;

		void initialize(size_t heap_size, size_t stack_size_per_thread)
		{
			CA_ASSERT(heap_size > 0);
			CA_ASSERT(stack_size_per_thread > 0);
			CA_ASSERT_MSG(app_heap_base == nullptr, "bad order of initialization");

			app_heap_base = sys::process_alloc(heap_size);
			app_heap_size = heap_size;
			create_arena(&app_heap, app_heap_base, app_heap_size);

			app_stack_size = stack_size_per_thread;
			initialize_tls();
		}

		void initialize_tls()
		{
			CA_ASSERT_MSG(app_heap_base != nullptr, "bad order of initialization");
			create_arena(&app_stack, &app_heap, app_stack_size);
		}

		void finalize()
		{
			CA_ASSERT_MSG(app_heap_base != nullptr, "bad order of initialization");

			finalize_tls();
			app_stack_size = 0;

			destroy_arena(&app_heap);
			sys::process_free(app_heap_base);
			app_heap_base = nullptr;
			app_heap_size = 0;
		}

		void finalize_tls()
		{
			CA_ASSERT_MSG(app_heap_base != nullptr, "bad order of initialization");
			destroy_arena(&app_stack);
		}
	}
}