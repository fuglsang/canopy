#pragma once

namespace ca
{
	namespace sys
	{
		struct mutex_t
		{
			void * handle;
		};

		void create_mutex(mutex_t * mutex);
		void destroy_mutex(mutex_t * mutex);

		void mutex_lock(mutex_t * mutex);
		void mutex_unlock(mutex_t * mutex);
	}
}