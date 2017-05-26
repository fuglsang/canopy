#pragma once

#if _WIN32
#define CA_PLATFORM_WIN32 1
#define CA_PLATFORM_VULKAN 1
#else
#error unknown platform
#endif