#if CA_TARGET_SYS_WIN32
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