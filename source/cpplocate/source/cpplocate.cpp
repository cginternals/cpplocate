
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

} // namespace


namespace cpplocate
{


std::string getExecutablePath()
{

#if defined SYSTEM_LINUX

    char exePath[PATH_MAX];

    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));

    if (len == -1 || len == sizeof(exePath)) {
        len = 0;
    }
    exePath[len] = '\0';

#elif defined SYSTEM_WINDOWS

    char exePath[MAX_PATH];

    if (GetModuleFileNameA(GetModuleHandleA(nullptr), exePath, sizeof(exePath)) == 0) {
        exePath[0] = '\0';
    }

#elif defined SYSTEM_SOLARIS

    char exePath[PATH_MAX];

    if (realpath(getexecname(), exePath) == nullptr) {
        exePath[0] = '\0';
    }

#elif defined SYSTEM_DARWIN

    char exePath[PATH_MAX];
    uint32_t len = sizeof(exePath);

    if (_NSGetExecutablePath(exePath, &len) == 0) {
        char * realPath = realpath(exePath, nullptr);
        if (realPath) {
            strncpy(exePath, realPath, len);
            free(realPath);
        }
    } else {
        exePath[0] = '\0';
    }

#elif defined SYSTEM_FREEBSD

    char exePath[2048];
    size_t len = sizeof(exePath);

    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;

    if (sysctl(mib, 4, exePath, &len, nullptr, 0) != 0) {
        exePath[0] = '\0';
    }

#else

    char * exePath = "";

#endif

    return utils::unifiedPath(std::string(exePath));
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

    char path[MAX_PATH];
    path[0] = '\0';

    HMODULE module;

    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(symbol),
            &module))
    {
        GetModuleFileNameA(module, path, sizeof(path));
    }

    return utils::unifiedPath(std::string(path));

#else

    Dl_info dlInfo;
    dladdr(reinterpret_cast<void*>(symbol), &dlInfo);

    return utils::unifiedPath(std::string(dlInfo.dli_fname));

#endif
}

std::string getBundlePath()
{
    // Get directory where the executable is located
    std::string exeDir = utils::getDirectoryPath(getExecutablePath());
    std::replace(exeDir.begin(), exeDir.end(), '\\', '/');

    // Split path into components
    std::vector<std::string> components;
    utils::split(exeDir, '/', components);

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
            return utils::unifiedPath(utils::join(components, "/"));
        }
    }

    // No bundle
    return "";
}

std::string locatePath(const std::string & relPath, const std::string & systemDir, void * symbol)
{
    std::string libDir    = utils::getDirectoryPath(getLibraryPath(symbol));
    std::string exeDir    = utils::getDirectoryPath(getExecutablePath());
    std::string bundleDir = utils::getDirectoryPath(getBundlePath());

    for (int i=0; i<3; i++)
    {
        std::string path;
        std::string subdir;

        // Choose basedir
        const std::string & dir = (i == 0 ? libDir : (i == 1 ? exeDir : bundleDir) );
        if (dir.empty()) continue;

        // Check <basedir>/<relpath>
        subdir = dir;
        path = subdir + "/" + relPath;
        if (utils::fileExists(path)) return subdir;

        // Check <basedir>/../<relpath>
        subdir = dir + "/..";
        path = subdir + "/" + relPath;
        if (utils::fileExists(path)) return subdir;

        // Check <basedir>/../../<relpath>
        subdir = dir + "/../..";
        path = subdir + "/" + relPath;
        if (utils::fileExists(path)) return subdir;

        // Check if it is a system path
        std::string basePath = utils::getSystemBasePath(path);
        if (!basePath.empty() && !systemDir.empty())
        {
            subdir = basePath + "/" + systemDir;
            path = subdir + "/" + relPath;
            if (utils::fileExists(path)) return subdir;
        }
    }

    // Check app bundle resources
    if (!bundleDir.empty())
    {
        std::string subdir = bundleDir + "/Contents/Resources";
        std::string path = subdir + "/" + relPath;
        if (utils::fileExists(path)) return subdir;
    }

    // Could not find path
    return "";
}

std::string getModulePath()
{
    return utils::getDirectoryPath(getExecutablePath());
}

ModuleInfo findModule(const std::string & name)
{
    ModuleInfo info;

    // Search at current executable location
    if (utils::loadModule(getModulePath(), name, info))
    {
        return info;
    }

    // Search all paths in CPPLOCATE_PATH
    std::vector<std::string> paths;
    std::string cppLocatePath = utils::getEnv("CPPLOCATE_PATH");
    utils::getPaths(cppLocatePath, paths);

    for (const std::string & path : paths)
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
    std::string programFiles64 = utils::getEnv("programfiles");
    std::string programFiles32 = utils::getEnv("programfiles(x86)");

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
