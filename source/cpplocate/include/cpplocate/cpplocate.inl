
#pragma once


#include <cpplocate/cpplocate.h>
#include <cpplocate/ModuleInfo.h>

#if defined SYSTEM_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
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

    return path;
#else
    Dl_info dlInfo;
    dladdr(reinterpret_cast<void*>(getDllPath), &dlInfo);

    return dlInfo.dli_fname;
#endif

}


static ModuleInfo findModule(const std::string & name)
{
    return findModule(name, getDllPath());
}


} // namespace cpplocate
