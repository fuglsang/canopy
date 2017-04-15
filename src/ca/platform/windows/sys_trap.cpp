#include "ca/platform.h"
#include "ca/sys_trap.h"

#if CA_PLATFORM_WINDOWS
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