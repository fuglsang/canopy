#pragma once

#if _WIN32
#define CA_TARGET_GFX_VULKAN 1
#define CA_TARGET_SYS_WIN32 1
#else
#error "unknown target platform"
#endif