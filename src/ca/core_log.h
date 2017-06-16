#pragma once

#include "ca/sys_breakpoint.h"

#include <cstdio>

#define CA_LOG(...)																							\
	do																										\
	{																										\
		fprintf(stdout, __VA_ARGS__);																		\
		fprintf(stdout, "\n");																				\
	}																										\
	while (false)

#define CA_LOG_CONT(...)																					\
	do																										\
	{																										\
		fprintf(stdout, __VA_ARGS__);																		\
	}																										\
	while (false)

#define CA_WARN(...)																						\
	do																										\
	{																										\
		fprintf(stdout, "[WARNING] ");																		\
		fprintf(stdout, __VA_ARGS__);																		\
		fprintf(stdout, "\n");																				\
	}																										\
	while (false)

#define CA_ERROR(...)																						\
	do																										\
	{																										\
		fprintf(stderr, "//----------------------------------------------------------------------------\n"	\
						"// %s, line %d (%s)\n"																\
						"// ERROR: ", __FILE__, __LINE__, __func__);										\
		fprintf(stderr, __VA_ARGS__);																		\
		fprintf(stderr, "\n");																				\
	}																										\
	while (false)

#define CA_FATAL(...)																						\
	do																										\
	{																										\
		fprintf(stderr, "//----------------------------------------------------------------------------\n"	\
						"// %s, line %d (%s)\n"																\
						"// FATAL ERROR: ", __FILE__, __LINE__, __func__);									\
		fprintf(stderr, __VA_ARGS__);																		\
		fprintf(stderr, "\n");																				\
		ca::sys::breakpoint();																				\
	}																										\
	while (false)