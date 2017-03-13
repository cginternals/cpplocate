
#include <cpplocate/cpplocate.h>


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


#include <array>
#include <cstdlib>
#include <vector>
#include <string>

#include <cpplocate/utils.h>
#include <cpplocate/ModuleInfo.h>


namespace
{


#ifdef SYSTEM_WINDOWS
    const char pathDelim = '\\';
#else
    const char pathDelim = '/';
#endif


/**
*  @brief
*    Get path to the current executable
*
*  @return
*    Path to executable (including filename)
*
*  @remarks
*    The path is returned in native format, e.g., backslashes on Windows.
*/
std::string obtainExecutablePath()
{

#if defined SYSTEM_LINUX

    std::array<char, PATH_MAX> exePath;

    auto len = ::readlink("/proc/self/exe", exePath.data(), exePath.size());

    if (len == -1 || len == exePath.size())
    {
        return "";
    }

    return std::string(exePath.data(), len);

#elif defined SYSTEM_WINDOWS

    std::array<char, MAX_PATH> exePath;

    if (GetModuleFileNameA(GetModuleHandleA(nullptr), exePath.data(), exePath.size()) == 0)
    {
        return "";
    }

    return std::string(exePath.data());

#elif defined SYSTEM_SOLARIS

    std::array<char, PATH_MAX> exePath;

    if (realpath(getexecname(), exePath.data()) == nullptr)
    {
        return "";
    }

    return std::string(exePath.data());

#elif defined SYSTEM_DARWIN

    std::array<char, PATH_MAX> exePath;

    auto len = exePath.size();

    if (_NSGetExecutablePath(exePath.data(), &len) != 0)
    {
        return "";
    }

    auto realPath = realpath(exePath.data(), nullptr);

    if (realPath)
    {
        strncpy(exePath.data(), realPath, len);
        free(realPath);
    }

    return std::string(exePath.data(), len);

#elif defined SYSTEM_FREEBSD

    std::array<char, 2048> exePath;

    auto len = exePath.size();

    int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME -1 };

    if (sysctl(mib, 4, exePath, &len, nullptr, 0) != 0)
    {
        return "";
    }

    return std::string(exePath.data(), len);

#else

    return "";

#endif
}

/**
*  @brief
*    Get list of paths in the 'CPPLOCATE_PATHS' environment variable
*
*  @return
*    List of paths
*
*  @remarks
*    The paths are returned in native format, e.g., backslashes on Windows.
*/
std::vector<std::string> getPaths()
{
    std::vector<std::string> paths;
    const auto cppLocatePath = cpplocate::utils::getEnv("CPPLOCATE_PATH");

    cpplocate::utils::getPaths(cppLocatePath, paths);

    return paths;
}


} // namespace


namespace cpplocate
{


const std::string & getExecutablePath()
{
    static const auto executablePath = obtainExecutablePath();

    return executablePath;
}

const std::string & getModulePath()
{
    static const auto modulePath = utils::getDirectoryPath(getExecutablePath());

    return modulePath;
}

ModuleInfo findModule(const std::string & name)
{
    ModuleInfo info;

    // Search at current module location
    if (utils::loadModule(getModulePath(), name, info))
    {
        return info;
    }

    // Search all paths in CPPLOCATE_PATH
    static const auto paths = getPaths();
    for (const auto & path : paths)
    {
        if (utils::loadModule(path, name, info))
        {
            return info;
        }

        if (utils::loadModule(utils::trimPath(path) + pathDelim + name, name, info))
        {
            return info;
        }
    }

    // Search in standard locations
#if defined SYSTEM_WINDOWS
    auto programFiles64 = utils::getEnv("programfiles");
    auto programFiles32 = utils::getEnv("programfiles(x86)");

    if (utils::loadModule(programFiles64 + "\\" + name, name, info))
    {
        return info;
    }

    if (utils::loadModule(programFiles32 + "\\" + name, name, info))
    {
        return info;
    }
#else
    if (utils::loadModule("/usr/share/" + name, name, info))
    {
        return info;
    }

    if (utils::loadModule("/usr/local/share/" + name, name, info))
    {
        return info;
    }
#endif

    // Not found
    return ModuleInfo();
}


} // namespace cpplocate
