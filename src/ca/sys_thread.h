#pragma once

#include "ca_base.h"
#include "ca/core_action.h"

namespace ca
{
	namespace sys
	{
		enum threadstate
		{
			THREADSTATE_INITIALIZING,
			THREADSTATE_SUSPENDED,
			THREADSTATE_RUNNING,
			THREADSTATE_JOINED,
			NUM_THREADSTATES,
		};

		struct thread_t
		{
			void * handle;
			threadstate state;
			core::action_t<> action;
		};

		void create_thread(thread_t * thread, core::action_t<> action);
		void destroy_thread(thread_t * thread);

		void thread_sleep(u64 microseconds);
		void thread_suspend(thread_t * thread);
		void thread_resume(thread_t * thread);
		void thread_join(thread_t * thread);
	}
}