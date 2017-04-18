#pragma once

#if _WIN32
#define CA_PLATFORM_WIN32 1
#define CA_PLATFORM_VULKAN 1
#else
#error unknown platform
#endif

#if CA_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif