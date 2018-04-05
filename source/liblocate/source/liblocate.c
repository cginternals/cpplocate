
#include <liblocate/liblocate.h>

#include <string.h>
#include <stdlib.h>

#if defined(SYSTEM_LINUX)
    #include <unistd.h>
    #include <limits.h>
    #include <linux/limits.h>
    #define _GNU_SOURCE
    #define __USE_GNU
    #include <dlfcn.h>
#elif defined(SYSTEM_WINDOWS)
    #include <Windows.h>
#elif defined(SYSTEM_SOLARIS)
    #include <stdlib.h>
    #include <limits.h>
    #include <dlfcn.h>
#elif defined(SYSTEM_DARWIN)
    #include <mach-o/dyld.h>
    #include <dlfcn.h>
    #include <sys/syslimits.h>
#elif defined(SYSTEM_FREEBSD)
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <dlfcn.h>
#endif

#include <liblocate/utils.h>


void getExecutablePath(char ** path, unsigned int * pathLength)
{
    if (path == 0x0)
    {
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        return;
    }

#if defined SYSTEM_LINUX

    char exePath[PATH_MAX];

    int len = readlink("/proc/self/exe", exePath, PATH_MAX);

    if (len == -1 || len == PATH_MAX || len == 0)
    {
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }
        return;
    }

    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, exePath, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#elif defined SYSTEM_WINDOWS

    char exePath[MAX_PATH];

    if (GetModuleFileNameA(GetModuleHandleA(0x0), exePath, MAX_PATH) == 0)
    {
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }
        return;
    }

    unsigned int len = strlen(exePath);
    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, exePath, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#elif defined SYSTEM_SOLARIS

    char exePath[PATH_MAX];

    if (realpath(getexecname(), exePath) == 0x0)
    {
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = len;
        }
        return;
    }

    unsigned int len = strlen(exePath);
    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, exePath, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#elif defined SYSTEM_DARWIN

    char exePath[PATH_MAX];

    unsigned int len = (unsigned int)PATH_MAX;

    if (_NSGetExecutablePath(exePath, &len) == 0)
    {
        char * realPath = realpath(exePath, 0x0);

        if (realPath == 0x0)
        {
            *path = 0x0;
            if (pathLength != 0x0)
            {
                *pathLength = len;
            }

            return;
        }

        unsigned int len = strlen(realPath);
        *path = (char *)malloc(sizeof(char) * len);
        memcpy(*path, realPath, len);
        if (pathLength != 0x0)
        {
            *pathLength = len;
        }

        free(realPath);
    }
    else
    {
        char * intermediatePath = (char *)malloc(sizeof(char) * len);

        if (_NSGetExecutablePath(intermediatePath, &len) != 0)
        {
            free(intermediatePath);
            *path = 0x0;
            if (pathLength != 0x0)
            {
                *pathLength = 0;
            }

            return;
        }

        char * realPath = realpath(intermediatePath, 0x0);

        free(intermediatePath);

        if (realPath == 0x0)
        {
            *path = 0x0;
            if (pathLength != 0x0)
            {
                *pathLength = 0;
            }

            return;
        }

        unsigned int len = strlen(realPath);
        *path = (char *)malloc(sizeof(char) * len);
        memcpy(*path, exePath, len);
        if (pathLength != 0x0)
        {
            *pathLength = len;
        }

        free(realPath);
    }

#elif defined SYSTEM_FREEBSD

    char exePath[2048];

    unsigned int len = 2048;

    int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

    if (sysctl(mib, 4, exePath, &len, 0x0, 0) != 0)
    {
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }
        return;
    }

    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, realPath, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#else

    *path = 0x0;
    if (pathLength != 0x0)
    {
        *pathLength = 0;
    }

#endif
}

void getBundlePath(char ** path, unsigned int * pathLength)
{
    if (path == 0x0)
    {
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        return;
    }

    // Get directory where the executable is located
    char * executablePath = 0x0;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);

    unsigned int executablePathDirectoryLength = 0;
    getDirectoryPart(executablePath, executablePathLength, &executablePathDirectoryLength);

    unifyPathDelimiters(executablePath, executablePathDirectoryLength);

    // check for /MacOS/Contents
    unsigned int bundlePathLength = 0;
    getBundlePart(executablePath, executablePathDirectoryLength, &bundlePathLength);

    if (bundlePathLength == 0)
    {
        // No bundle
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        free(executablePath);

        return;
    }

    *path = executablePath;
    if (pathLength != 0x0)
    {
        *pathLength = bundlePathLength;
    }
}

void getModulePath(char ** path, unsigned int * pathLength)
{
    if (path == 0x0)
    {
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        return;
    }

    char * executablePath = 0x0;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    getDirectoryPart(executablePath, executablePathLength, &executablePathDirectoryLength);

    *path = (char *)malloc(sizeof(char) * (executablePathLength+1));
    memcpy(*path, executablePath, executablePathDirectoryLength);
    (*path)[executablePathDirectoryLength] = 0;
    if (pathLength != 0x0)
    {
        *pathLength = executablePathDirectoryLength;
    }

    free(executablePath);
}

void getLibraryPath(void * symbol, char ** path, unsigned int * pathLength)
{
    if (path == 0x0)
    {
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        return;
    }

    *path = 0x0;
    if (pathLength != 0x0)
    {
        *pathLength = 0;
    }

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
            (LPCSTR)symbol,
            &module))
    {
        GetModuleFileNameA(module, systemPath, MAX_PATH);
    }

    unsigned int len = strnlen_s(systemPath, MAX_PATH);
    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, systemPath, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#else

    Dl_info dlInfo;
    dladdr(symbol, &dlInfo);

    if (!dlInfo.dli_fname)
    {
        return;
    }

    unsigned int len = strlen(dlInfo.dli_fname);
    *path = (char *)malloc(sizeof(char) * len);
    memcpy(*path, dlInfo.dli_fname, len);
    if (pathLength != 0x0)
    {
        *pathLength = len;
    }

#endif

    unifyPathDelimiters(*path, *pathLength);
}

void locatePath(char ** path, unsigned int * pathLength, const char * relPath, unsigned int relPathLength,
    const char * systemDir, unsigned int systemDirLength, void * symbol)
{
    if (path == 0x0)
    {
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }

        return;
    }

    char * executablePath = 0x0;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    char * bundlePath = 0x0;
    unsigned int bundlePathLength = 0;
    getBundlePath(&bundlePath, &bundlePathLength);

    char * libraryPath = 0x0;
    unsigned int libraryPathLength = 0;
    getLibraryPath(symbol, &libraryPath, &libraryPathLength);
    unsigned int libraryPathDirectoryLength = 0;

    getDirectoryPart(libraryPath, libraryPathLength, &libraryPathDirectoryLength);
    getDirectoryPart(executablePath, executablePathLength, &executablePathDirectoryLength);

    unsigned int maxLength = 0;
    maxLength = maxLength > executablePathDirectoryLength ? maxLength : executablePathDirectoryLength;
    maxLength = maxLength > libraryPathDirectoryLength ? maxLength : libraryPathDirectoryLength;
    maxLength = maxLength > bundlePathLength + 19 ? maxLength : bundlePathLength; // for "/Contents/Resources"
    maxLength += relPathLength + 2; // for the extra path delimiter and null byte suffix

    const char * dirs[3] = { libraryPath, executablePath, bundlePath };
    const unsigned int lengths[3] = { libraryPathDirectoryLength, executablePathDirectoryLength, bundlePathLength };

    char * subdir = (char *)malloc(sizeof(char) * maxLength);
    unsigned int subdirLength = 0;
    for (int i = 0; i < 3; ++i)
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
            *path = (char *)malloc(sizeof(char) * subdirLength);
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            free(libraryPath);
            free(executablePath);
            free(bundlePath);
            free(subdir);

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
            *path = (char *)malloc(sizeof(char) * subdirLength);
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            free(libraryPath);
            free(executablePath);
            free(bundlePath);
            free(subdir);

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
            *path = (char *)malloc(sizeof(char) * subdirLength);
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            free(libraryPath);
            free(executablePath);
            free(bundlePath);
            free(subdir);

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
            *path = (char *)malloc(sizeof(char) * subdirLength);
            *pathLength = subdirLength;
            memcpy(*path, subdir, subdirLength);

            free(libraryPath);
            free(executablePath);
            free(bundlePath);
            free(subdir);

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
                *path = (char *)malloc(sizeof(char) * subdirLength);
                *pathLength = subdirLength;
                memcpy(*path, subdir, subdirLength);

                free(libraryPath);
                free(executablePath);
                free(bundlePath);
                free(subdir);

                return;
            }
        }
    }

    free(libraryPath);
    free(executablePath);
    free(bundlePath);
    free(subdir);

    // Could not find path
    *path = 0x0;
    *pathLength = 0;
}
