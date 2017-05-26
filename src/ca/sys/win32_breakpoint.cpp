#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys/win32.h"
#include "ca/sys_breakpoint.h"

namespace ca
{
	namespace sys
	{
		void breakpoint()
		{
			DebugBreak();
		}
	}
}
#endif