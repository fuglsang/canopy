#pragma once

#include "ca/core_log.h"
#include "ca/sys_breakpoint.h"

#define CA_ASSERT(condition)												\
	do																		\
	{																		\
		if ((condition) == false)											\
		{																	\
			CA_FATAL("assertion FAILED ( " #condition " )");				\
		}																	\
	}																		\
	while (false)

#define CA_ASSERT_MSG(condition, ...)										\
	do																		\
	{																		\
		if ((condition) == false)											\
		{																	\
			CA_FATAL("assertion FAILED ( " #condition " ) " __VA_ARGS__);	\
		}																	\
	}																		\
	while (false)