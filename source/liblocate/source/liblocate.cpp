
#include <liblocate/liblocate.h>

#include <string.h>

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

#include <liblocate/utils.h>


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

    std::string finalPath;
    if (_NSGetExecutablePath(exePath.data(), &len) == 0)
    {
        auto realPath = realpath(exePath.data(), nullptr);

        std::string finalPath;
        if (realPath)
        {
            finalPath = std::string(realPath);
            free(realPath);
        }

        return finalPath;
    }
    else
    {
        std::vector<char> longerExePath(len);

        if (_NSGetExecutablePath(longerExePath.data(), &len) != 0)
        {
            return "";
        }

        auto realPath = realpath(longerExePath.data(), nullptr);

        std::string finalPath;
        if (realPath)
        {
            finalPath = std::string(realPath);
            free(realPath);
        }

        return finalPath;
    }

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
    char * executablePath = nullptr;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);

    unsigned int executablePathDirectoryLength = 0;
    utils::getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

    utils::unifiedPath(executablePath, executablePathDirectoryLength);

    const auto exeDir = std::string(executablePath, executablePathDirectoryLength);

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
            // getExecutablePath() always returns an absolute path (at least on macOS), but
            // the leading slash got lost during split, so we need to add it back
            return "/" + utils::join(components, "/");
        }
    }

    // No bundle
    return "";
}


} // namespace


void getExecutablePath(char ** path, unsigned int * pathLength)
{
    static const auto executablePath = obtainExecutablePath();

    *path = const_cast<char *>(executablePath.data());
    *pathLength = executablePath.size();
}

void getBundlePath(const char ** path, unsigned int * pathLength)
{
    static const auto bundlePath = obtainBundlePath();

    *path = bundlePath.data();
    *pathLength = bundlePath.size();
}

void getModulePath(const char ** path, unsigned int * pathLength)
{
    char * executablePath = nullptr;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    utils::getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

    *path = executablePath;
    *pathLength = executablePathDirectoryLength;
}

void getLibraryPath(void * symbol, char ** path, unsigned int * pathLength)
{
    *path = nullptr;
    *pathLength = 0;

    if (!symbol)
    {
        return;
    }

#if defined CPPLOCATE_STATIC_DEFINE

    return;

#elif defined SYSTEM_WINDOWS

    char systemPath[MAX_PATH];
    systemPath[0] = '\0';

    HMODULE module;

    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(symbol),
            &module))
    {
        GetModuleFileNameA(module, systemPath, MAX_PATH);
    }

    *pathLength = strnlen_s(systemPath, MAX_PATH);
    *path = reinterpret_cast<char *>(malloc(sizeof(char) * *pathLength));
    memcpy(*path, systemPath, *pathLength);

#else

    Dl_info dlInfo;
    dladdr(reinterpret_cast<void*>(symbol), &dlInfo);

    if (!dlInfo.dli_fname)
    {
        return;
    }

    *pathLength = strlen(dlInfo.dli_fname);
    *path = reinterpret_cast<char *>(malloc(sizeof(char) * *pathLength));
    memcpy(*path, dlInfo.dli_fname, *pathLength);

#endif

    utils::unifiedPath(*path, *pathLength);
}

void locatePath(char ** path, unsigned int * pathLength, const char * relPath, unsigned int relPathLength,
    const char * systemDir, unsigned int systemDirLength, void * symbol)
{
    char * executablePath = nullptr;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    char * bundlePath = nullptr;
    unsigned int bundlePathLength = 0;
    getBundlePath(&bundlePath, &bundlePathLength);

    char * libraryPath = nullptr;
    unsigned int libraryPathLength = 0;
    getLibraryPath(symbol, &libraryPath, &libraryPathLength);
    unsigned int libraryPathDirectoryLength = 0;

    utils::getDirectoryPath(libraryPath, libraryPathLength, &libraryPathDirectoryLength);
    utils::getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

    unsigned int maxLength = 0;
    maxLength = maxLength > executablePathDirectoryLength ? maxLength : executablePathDirectoryLength;
    maxLength = maxLength > libraryPathDirectoryLength ? maxLength : libraryPathDirectoryLength;
    maxLength = maxLength > bundlePathLength + 19 ? maxLength : bundlePathLength; // for "/Contents/Resources"
    maxLength += relPathLength + 2; // for the extra path delimiter and null byte suffix

    const char * dirs[3] = { libraryPath, executablePath, bundlePath };
    const unsigned int lengths[3] = { libraryPathDirectoryLength, executablePathDirectoryLength, bundlePathLength };

    char * subdir = reinterpret_cast<char *>(malloc(sizeof(char) * maxLength));
    unsigned int subdirLength = 0;
    for (int i = 0; i < sizeof dirs; ++i)
    {
        const char * dir = dirs[i];
        const unsigned int length = lengths[i];

        if (length <= 0)
        {
            continue;
        }

        // Check <basedir>/<relpath>

        memcpy(subdir, dir, length);
        subdirLength = length;
        memcpy(subdir+subdirLength, "/", 1);
        subdirLength += 1;
        memcpy(subdir+subdirLength, relPath, relPathLength);
        subdirLength += relPathLength;
        subdir[subdirLength] = 0;
        if (utils::fileExists(subdir, subdirLength))
        {
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * subdirLength));
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            return;
        }

        // Check <basedir>/../<relpath>

        subdirLength = length;
        memcpy(subdir+subdirLength, "/../", 4);
        subdirLength += 4;
        memcpy(subdir+subdirLength, relPath, relPathLength);
        subdirLength += relPathLength;
        subdir[subdirLength] = 0;
        if (utils::fileExists(subdir, subdirLength))
        {
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * subdirLength));
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            return;
        }

        // Check <basedir>/../../<relpath>
        subdirLength = length;
        memcpy(subdir+subdirLength, "/../../", 7);
        subdirLength += 7;
        memcpy(subdir+subdirLength, relPath, relPathLength);
        subdirLength += relPathLength;
        subdir[subdirLength] = 0;
        if (utils::fileExists(subdir, subdirLength))
        {
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * subdirLength));
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            return;
        }

        if (systemDirLength <= 0)
            continue;

        // Check if it is a system path

        unsigned int systemBasePathLength;
        utils::getSystemBasePath(dir, length, &systemBasePathLength);

        if (systemBasePathLength <= 0)
            continue;

        subdirLength = systemBasePathLength;
        memcpy(subdir+subdirLength, "/", 1);
        subdirLength += 1;
        memcpy(subdir+subdirLength, systemDir, systemDirLength);
        subdirLength += systemDirLength;
        memcpy(subdir+subdirLength, "/", 1);
        subdirLength += 1;
        memcpy(subdir+subdirLength, relPath, relPathLength);
        subdirLength += relPathLength;
        subdir[subdirLength] = 0;
        if (utils::fileExists(subdir, subdirLength))
        {
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * subdirLength));
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            return;
        }

        // Check app bundle resources
        if (i == 2) // only for bundle path
        {
            subdirLength = length;
            memcpy(subdir+subdirLength, "/Contents/Resources/", 20);
            subdirLength += 20;
            memcpy(subdir+subdirLength, relPath, relPathLength);
            subdirLength += relPathLength;
            subdir[subdirLength] = 0;
            if (utils::fileExists(subdir, subdirLength))
            {
                *path = reinterpret_cast<char *>(malloc(sizeof(char) * subdirLength));
                *pathLength = subdirLength;
                memcpy(*path, subdir, subdirLength);

                return;
            }
        }
    }

    // Could not find path
    *path = nullptr;
    *pathLength = 0;
}
