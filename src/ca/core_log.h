#pragma once

#include <stdio.h>

#define CA_LOG(...)																							\
	do																										\
	{																										\
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