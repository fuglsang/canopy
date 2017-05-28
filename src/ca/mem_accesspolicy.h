#pragma once

#include "ca/sys_mutex.h"

namespace ca
{
	namespace mem
	{
		struct multithreadaccesspolicy_t
		{
			sys::mutex_t mutex;
		};

		inline void create_accesspolicy(multithreadaccesspolicy_t * accesspolicy)
		{
			sys::create_mutex(&accesspolicy->mutex);
		}

		inline void destroy_accesspolicy(multithreadaccesspolicy_t * accesspolicy)
		{
			sys::destroy_mutex(&accesspolicy->mutex);
		}

		inline void accesspolicy_begin(multithreadaccesspolicy_t * accesspolicy)
		{
			sys::mutex_lock(&accesspolicy->mutex);
		}

		inline void accesspolicy_end(multithreadaccesspolicy_t * accesspolicy)
		{
			sys::mutex_unlock(&accesspolicy->mutex);
		}

		struct singlethreadaccesspolicy_t {};

		inline void create_accesspolicy(singlethreadaccesspolicy_t * accesspolicy) {}
		inline void destroy_accesspolicy(singlethreadaccesspolicy_t * accesspolicy) {}

		inline void accesspolicy_begin(singlethreadaccesspolicy_t * accesspolicy) {}
		inline void accesspolicy_end(singlethreadaccesspolicy_t * accesspolicy) {}
	}
}