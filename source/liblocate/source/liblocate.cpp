
#include <liblocate/liblocate.h>

#include <string.h>
#include <stdlib.h>

#if defined SYSTEM_LINUX
    #include <unistd.h>
    #include <limits.h>
    #include <dlfcn.h>
    #include <linux/limits.h>
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
*  @param[out] path
*    Path to executable (including filename)
*  @param[out] pathLength
*    Length of path
*
*  @remarks
*    The path is returned in native format, e.g., backslashes on Windows.
*/
void obtainExecutablePath(char ** path, unsigned int * pathLength)
{

#if defined SYSTEM_LINUX

    char exePath[PATH_MAX];

    auto len = readlink("/proc/self/exe", exePath, PATH_MAX);

    if (len == -1 || len == PATH_MAX || len == 0)
    {
        *path = nullptr;
        *pathLength = 0;
        return;
    }

    *path = reinterpret_cast<char *>(malloc(sizeof(char) * len));
    memcpy(*path, exePath, len);
    *pathLength = len;

#elif defined SYSTEM_WINDOWS

    char exePath[MAX_PATH];

    if (GetModuleFileNameA(GetModuleHandleA(nullptr), exePath, MAX_PATH) == 0)
    {
        *path = nullptr;
        *pathLength = 0;
        return;
    }

    *pathLength = strlen(exePath);
    *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
    memcpy(*path, exePath, pathLength);

#elif defined SYSTEM_SOLARIS

    char exePath[PATH_MAX];

    if (realpath(getexecname(), exePath) == nullptr)
    {
        *path = nullptr;
        *pathLength = 0;
        return;
    }

    *pathLength = strlen(exePath);
    *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
    memcpy(*path, exePath, pathLength);

#elif defined SYSTEM_DARWIN

    char exePath[PATH_MAX];

    auto len = std::uint32_t(PATH_MAX);

    if (_NSGetExecutablePath(exePath, &len) == 0)
    {
        auto realPath = realpath(exePath, nullptr);

        if (realPath)
        {
            *pathLength = strlen(realPath);
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
            memcpy(*path, realPath, pathLength);
        }
        else
        {
            *path = nullptr;
            *pathLength = 0;
        }
    }
    else
    {
        *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
        std::vector<char> longerExePath(len);

        if (_NSGetExecutablePath(*path, pathLength) != 0)
        {
            free(*path);
            *path = nullptr;
            *pathLength = 0;

            return;
        }

        auto realPath = realpath(*path, nullptr);

        free(*path);

        if (realPath)
        {
            *pathLength = strlen(realPath);
            *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
            memcpy(*path, realPath, pathLength);
        }
        else
        {
            *path = nullptr;
            *pathLength = 0;
        }
    }

#elif defined SYSTEM_FREEBSD

    char exePath[2048];

    auto len = 2048;

    int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

    if (sysctl(mib, 4, exePath, &len, nullptr, 0) != 0)
    {
        *path = nullptr;
        *pathLength = 0;
        return;
    }

    *pathLength = len;
    *path = reinterpret_cast<char *>(malloc(sizeof(char) * pathLength));
    memcpy(*path, realPath, pathLength);

#else

    *path = nullptr;
    *pathLength = 0;

#endif
}

/**
*  @brief
*    Get path to the current application bundle
*
*  @param[out] path
*    Path to bundle (including filename)
*  @param[out] pathLength
*    Length of path
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*    If the current executable is part of a macOS application bundle,
*    this function returns the part to the bundle. Otherwise, an
*    empty string is returned.
*/
void obtainBundlePath(char ** path, unsigned int * pathLength)
{
    // Get directory where the executable is located
    char * executablePath = nullptr;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);

    unsigned int executablePathDirectoryLength = 0;
    getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

    // check for /MacOS/Contents
    const auto potentialBundleStart = executablePath + executablePathDirectoryLength - 15;

    if (strncmp(potentialBundleStart, "/MacOS/Contents", 15) == 0)
    {
        unifiedPath(executablePath, executablePathDirectoryLength - 15);
        *path = executablePath;
        *pathLength = executablePathDirectoryLength - 15;
        return;
    }

    // No bundle
    *path = nullptr;
    *pathLength = 0;
    return;
}


} // namespace


void getExecutablePath(char ** path, unsigned int * pathLength)
{
    obtainExecutablePath(path, pathLength);
}

void getBundlePath(char ** path, unsigned int * pathLength)
{
    obtainBundlePath(path, pathLength);
}

void getModulePath(const char ** path, unsigned int * pathLength)
{
    char * executablePath = nullptr;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

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

    unifiedPath(*path, *pathLength);
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

    getDirectoryPath(libraryPath, libraryPathLength, &libraryPathDirectoryLength);
    getDirectoryPath(executablePath, executablePathLength, &executablePathDirectoryLength);

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
        if (fileExists(subdir, subdirLength))
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
        if (fileExists(subdir, subdirLength))
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
        if (fileExists(subdir, subdirLength))
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
        getSystemBasePath(dir, length, &systemBasePathLength);

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
        if (fileExists(subdir, subdirLength))
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
            if (fileExists(subdir, subdirLength))
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
