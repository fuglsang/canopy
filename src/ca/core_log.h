#pragma once

#include "ca/sys_breakpoint.h"

namespace ca
{
	namespace core
	{
		void log_stderr(char const * fmt, ...);
		void log_stdout(char const * fmt, ...);
	}
}

#define CA_LOG(...)							\
	do										\
	{										\
		ca::core::log_stdout(__VA_ARGS__);	\
		ca::core::log_stdout("\n");			\
	}										\
	while (false)

#define CA_LOG_CONT(...)					\
	do										\
	{										\
		ca::core::log_stdout(__VA_ARGS__);	\
	}										\
	while (false)

#define CA_WARN(...)						\
	do										\
	{										\
		ca::core::log_stdout("[WARNING] ");	\
		ca::core::log_stdout(__VA_ARGS__);	\
		ca::core::log_stdout("\n");			\
	}										\
	while (false)

#define CA_ERROR(...)																							\
	do																											\
	{																											\
		ca::core::log_stderr("//----------------------------------------------------------------------------\n"	\
		                     "// %s, line %d (%s)\n"															\
		                     "// ERROR: ", __FILE__, __LINE__, __func__);										\
		ca::core::log_stderr(__VA_ARGS__);																		\
		ca::core::log_stderr("\n");																				\
	}																											\
	while (false)

#define CA_FATAL(...)																							\
	do																											\
	{																											\
		ca::core::log_stderr("//----------------------------------------------------------------------------\n"	\
		                     "// %s, line %d (%s)\n"															\
		                     "// FATAL ERROR: ", __FILE__, __LINE__, __func__);									\
		ca::core::log_stderr(__VA_ARGS__);																		\
		ca::core::log_stderr("\n");																				\
		ca::sys::breakpoint();																					\
	}																											\
	while (false)
