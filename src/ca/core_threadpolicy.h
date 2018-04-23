#pragma once

#include "ca_base.h"
#include "ca/sys_mutex.h"

namespace ca
{
	namespace core
	{
		struct multithreadpolicy_t
		{
			sys::mutex_t mutex;
		};

		CA_INLINE void create_threadpolicy(multithreadpolicy_t * threadpolicy)
		{
			sys::create_mutex(&threadpolicy->mutex);
		}

		CA_INLINE void destroy_threadpolicy(multithreadpolicy_t * threadpolicy)
		{
			sys::destroy_mutex(&threadpolicy->mutex);
		}

		CA_INLINE void threadpolicy_begin(multithreadpolicy_t * threadpolicy)
		{
			sys::mutex_lock(&threadpolicy->mutex);
		}

		CA_INLINE void threadpolicy_end(multithreadpolicy_t * threadpolicy)
		{
			sys::mutex_unlock(&threadpolicy->mutex);
		}

		struct singlethreadpolicy_t
		{};

		CA_INLINE void create_threadpolicy(singlethreadpolicy_t * threadpolicy)
		{}

		CA_INLINE void destroy_threadpolicy(singlethreadpolicy_t * threadpolicy)
		{}

		CA_INLINE void threadpolicy_begin(singlethreadpolicy_t * threadpolicy)
		{}

		CA_INLINE void threadpolicy_end(singlethreadpolicy_t * threadpolicy)
		{}
	}
}