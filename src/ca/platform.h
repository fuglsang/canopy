#pragma once

#if _WIN32
#define CA_PLATFORM_WINDOWS 1
#define CA_PLATFORM_VULKAN 1
#else
#error unsupported platform
#endif

#if CA_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif