#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys_clock.h"

namespace ca
{
	namespace sys
	{
		f64 clock()
		{
			LARGE_INTEGER time;
			LARGE_INTEGER freq;

			QueryPerformanceCounter(&time);
			QueryPerformanceFrequency(&freq);

			time.QuadPart *= 1;
			time.QuadPart /= freq.QuadPart;

			return static_cast<f64>(time.QuadPart);
		}

		f64 clock_milli()
		{
			LARGE_INTEGER time;
			LARGE_INTEGER freq;

			QueryPerformanceCounter(&time);
			QueryPerformanceFrequency(&freq);

			time.QuadPart *= 1000;
			time.QuadPart /= freq.QuadPart;

			return static_cast<f64>(time.QuadPart);
		}

		f64 clock_micro()
		{
			LARGE_INTEGER time;
			LARGE_INTEGER freq;

			QueryPerformanceCounter(&time);
			QueryPerformanceFrequency(&freq);

			time.QuadPart *= 1000000;
			time.QuadPart /= freq.QuadPart;

			return static_cast<f64>(time.QuadPart);
		}
	}
}
#endif