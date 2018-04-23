#include "ca/core_log.h"

#include <stdio.h>
#include <stdarg.h>

namespace ca
{
	namespace core
	{
		void log_stderr(char const * fmt, ...)
		{
			va_list argp;
			va_start(argp, fmt);
			vfprintf(stderr, fmt, argp);
			va_end(argp);
		}

		void log_stdout(char const * fmt, ...)
		{
			va_list argp;
			va_start(argp, fmt);
			vfprintf(stdout, fmt, argp);
			va_end(argp);
		}
	}
}