
#include <cpplocate/cpplocate.h>

#if defined SYSTEM_LINUX
    #include <unistd.h>
    #include <limits.h>
    #include <dlfcn.h>
#elif defined SYSTEM_WINDOWS
    #include <Windows.h>
#elif defined SYSTEM_SOLARIS
    #include <stdlib.h>
    #include <limits.h>
    #include <dlfcn.h>
#elif defined SYSTEM_DARWIN
    #include <mach-o/dyld.h>
    #include <dlfcn.h>
#elif defined SYSTEM_FREEBSD
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <dlfcn.h>
#endif


#include <array>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
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

    auto len = std::uint32_t(exePath.size());

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

    int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

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
*    Get path to the current application bundle
*
*  @return
*    Path to bundle (including filename)
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*    If the current executable is part of a macOS application bundle,
*    this function returns the part to the bundle. Otherwise, an
*    empty string is returned.
*/
std::string obtainBundlePath()
{
    // Get directory where the executable is located
    std::string exeDir = cpplocate::utils::getDirectoryPath(cpplocate::getExecutablePath());
    std::replace(exeDir.begin(), exeDir.end(), '\\', '/');

    // Split path into components
    std::vector<std::string> components;
    cpplocate::utils::split(exeDir, '/', components);

    // If this is a bundle, we must have at least three components
    if (components.size() >= 3)
    {
        // Check for bundle
        if (components[components.size() - 1] == "MacOS" &&
            components[components.size() - 2] == "Contents")
        {
            // Remove '/Contents/MacOS' from path
            components.pop_back();
            components.pop_back();

            // Compose path to bundle
            return cpplocate::utils::unifiedPath(cpplocate::utils::join(components, "/"));
        }
    }

    // No bundle
    return "";
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

const std::string & getBundlePath()
{
    static const auto bundlePath = obtainBundlePath();

    return bundlePath;
}

const std::string & getModulePath()
{
    static const auto modulePath = utils::getDirectoryPath(getExecutablePath());

    return modulePath;
}

std::string getLibraryPath(void * symbol)
{
    if (!symbol)
    {
        return "";
    }

#if defined CPPLOCATE_STATIC_DEFINE

    return "";

#elif defined SYSTEM_WINDOWS

    std::array<char, PATH_MAX> path;
    path[0] = '\0';

    HMODULE module;

    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(symbol),
            &module))
    {
        GetModuleFileNameA(module, path.data, path.size());
    }

    if (!path)
    {
        return "";
    }

    return utils::unifiedPath(std::string(path));

#else

    Dl_info dlInfo;
    dladdr(reinterpret_cast<void*>(symbol), &dlInfo);

    if (!dlInfo.dli_fname)
    {
        return "";
    }

    return utils::unifiedPath(std::string(dlInfo.dli_fname));

#endif
}

std::string locatePath(const std::string & relPath, const std::string & systemDir, void * symbol)
{
    const auto libDir    = utils::getDirectoryPath(getLibraryPath(symbol));
    const auto exeDir    = utils::getDirectoryPath(getExecutablePath());
    const auto bundleDir = utils::getDirectoryPath(getBundlePath());

    for (const auto & dir : { libDir, exeDir, bundleDir })
    {
        // Check <basedir>/<relpath>
        auto subdir = dir;
        auto path = subdir + "/" + relPath;
        if (utils::fileExists(path))
            return subdir;

        // Check <basedir>/../<relpath>
        subdir = dir + "/..";
        path = subdir + "/" + relPath;
        if (utils::fileExists(path))
            return subdir;

        // Check <basedir>/../../<relpath>
        subdir = dir + "/../..";
        path = subdir + "/" + relPath;
        if (utils::fileExists(path))
            return subdir;

        // Check if it is a system path
        const auto basePath = utils::getSystemBasePath(path);
        if (!basePath.empty() && !systemDir.empty())
        {
            subdir = basePath + "/" + systemDir;
            path = subdir + "/" + relPath;
            if (utils::fileExists(path))
                return subdir;
        }
    }

    // Check app bundle resources
    if (!bundleDir.empty())
    {
        const auto subdir = bundleDir + "/Contents/Resources";
        const auto path = subdir + "/" + relPath;

        if (utils::fileExists(path))
            return subdir;
    }

    // Could not find path
    return "";
}

ModuleInfo findModule(const std::string & name)
{
    ModuleInfo info;

    // Search at current executable location
    if (utils::loadModule(getModulePath(), name, info))
    {
        return info;
    }

    for (const std::string & path : getPaths())
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
    const auto programFiles64 = utils::getEnv("programfiles");
    const auto programFiles32 = utils::getEnv("programfiles(x86)");

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
