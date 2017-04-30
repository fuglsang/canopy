#pragma once

#include "ca/core_log.h"
#include "ca/sys_breakpoint.h"

#define CA_ASSERT(condition)												\
	do																		\
	{																		\
		if ((condition) == false)											\
		{																	\
			CA_ERROR("assertion FAILED ( " #condition " )");				\
			ca::sys::breakpoint();											\
		}																	\
	}																		\
	while (false)

#define CA_ASSERT_MSG(condition, ...)										\
	do																		\
	{																		\
		if ((condition) == false)											\
		{																	\
			CA_ERROR("assertion FAILED ( " #condition " ) " __VA_ARGS__);	\
			ca::sys::breakpoint();											\
		}																	\
	}																		\
	while (false)