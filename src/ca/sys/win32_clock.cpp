#if CA_TARGET_SYS_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_clock.h"

namespace ca
{
	namespace sys
	{
		static u64 precision_clock_start = 0;

		template <u64 N>
		static CA_INLINE u64 precision_clock()
		{
			LARGE_INTEGER time;
			LARGE_INTEGER freq;

			QueryPerformanceCounter(&time);
			QueryPerformanceFrequency(&freq);

			time.QuadPart -= precision_clock_start;
			time.QuadPart *= N;
			time.QuadPart /= freq.QuadPart;

			return time.QuadPart;
		}

		//-------------
		// clock reset

		void reset_clock()
		{
			LARGE_INTEGER time;

			QueryPerformanceCounter(&time);
			
			precision_clock_start = time.QuadPart;
		}

		//-------------
		// u64 queries

		u64 clock()
		{
			return precision_clock<1>();
		}

		u64 clock_milli()
		{
			return precision_clock<1000>();
		}

		u64 clock_micro()
		{
			return precision_clock<1000000>();
		}

		//-------------
		// f32 queries

		f32 clockf()
		{
			return 0.001f * precision_clock<1000>();
		}

		f32 clockf_milli()
		{
			return 0.001f * precision_clock<1000000>();
		}

		f32 clockf_micro()
		{
			return static_cast<f32>(precision_clock<1000000>());
		}

		//-------------
		// f64 queries

		f64 clockd()
		{
			return 0.001 * precision_clock<1000>();
		}

		f64 clockd_milli()
		{
			return 0.001 * precision_clock<1000000>();
		}

		f64 clockd_micro()
		{
			return static_cast<f64>(precision_clock<1000000>());
		}
	}
}
#endif