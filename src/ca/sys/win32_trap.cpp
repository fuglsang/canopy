#include "ca/platform.h"

#if CA_PLATFORM_WIN32
#include "ca/sys_trap.h"

namespace ca
{
	namespace sys
	{
		void trap()
		{
			DebugBreak();
		}
	}
}
#endif