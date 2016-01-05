
#include <cpplocate/cpplocate.h>

#if defined SYSTEM_LINUX
    #include <unistd.h>
    #include <limits.h>
#elif defined SYSTEM_WINDOWS
    #include <stdlib.h>
#elif defined SYSTEM_SOLARIS
    #include <stdlib.h>
    #include <limits.h>
#elif defined SYSTEM_DARWIN
    #include <mach-o/dyld.h>
#elif defined SYSTEM_FREEBSD
    #include <sys/types.h>
    #include <sys/sysctl.h>
#endif

#include <cstdlib>
#include <vector>
#include <string>

#include <cpplocate/utils.h>


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

    char * exePath;

    if (_get_pgmptr(&exePath) != 0) {
        exePath = "";
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

    return std::string(exePath);
}

std::string getModulePath()
{
    return utils::getDirectoryPath(getExecutablePath());
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
    std::vector<std::string> paths;
    std::string cppLocatePath = utils::getEnv("CPPLOCATE_PATH");
    utils::getPaths(cppLocatePath, paths);
    for (std::string path : paths)
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
