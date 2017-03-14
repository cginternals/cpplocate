
#pragma once


#include <cpplocate/cpplocate.h>
#include <cpplocate/ModuleInfo.h>

#if defined SYSTEM_LINUX
#include <unistd.h>
#include <limits.h>
#elif defined SYSTEM_WINDOWS
#include <Windows.h>
#elif defined SYSTEM_SOLARIS
#include <stdlib.h>
#include <limits.h>
#elif defined SYSTEM_DARWIN
#include <mach-o/dyld.h>
#elif defined SYSTEM_FREEBSD
#include <sys/types.h>
#include <sys/sysctl.h>
#endif


namespace cpplocate
{


static std::string getDllPath()
{
#if defined(SYSTEM_WINDOWS)
    char path[MAX_PATH];
    path[0] = '\0';

    HMODULE module;
    const auto success = GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(getDllPath), &module);
    if (success)
    {
        GetModuleFileNameA(module, path, sizeof path);
    }
#endif
    
    return path;
}


static ModuleInfo findModule(const std::string & name)
{
    return findModule(name, getDllPath());
}


} // namespace cpplocate
