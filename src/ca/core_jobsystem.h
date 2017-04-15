#pragma once

#include "ca/sys_mutex.h"
#include "ca/sys_thread.h"

namespace ca
{
	namespace core
	{
		struct jobsystem_t
		{

		};

		void create_jobsystem(jobsystem_t * jobsystem, u32 num_workers);
		void destroy_jobsystem(jobsystem_t * jobsystem);
	}
}