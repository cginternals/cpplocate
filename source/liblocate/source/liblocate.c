
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

#include "utils.h"


void getExecutablePath(char ** path, unsigned int * pathLength)
{
    if (!checkStringOutParameter(path, pathLength))
    {
        return;
    }

#if defined SYSTEM_LINUX

    char exePath[PATH_MAX];

    int len = readlink("/proc/self/exe", exePath, PATH_MAX);

    if (len <= 0 || len == PATH_MAX)
    {
        invalidateStringOutParameter(path, pathLength);
        return;
    }

    copyToStringOutParameter(exePath, len, path, pathLength);

#elif defined SYSTEM_WINDOWS

    char exePath[MAX_PATH];

    unsigned int len = GetModuleFileNameA(GetModuleHandleA(0x0), exePath, MAX_PATH);
    if (len == 0)
    {
        *path = 0x0;
        if (pathLength != 0x0)
        {
            *pathLength = 0;
        }
        return;
    }

    copyToStringOutParameter(exePath, len, path, pathLength);

#elif defined SYSTEM_SOLARIS

    char exePath[PATH_MAX];

    if (realpath(getexecname(), exePath) == 0x0)
    {
        invalidateStringOutParameter(path, pathLength);
        return;
    }

    unsigned int len = strlen(exePath);
    copyToStringOutParameter(exePath, len, path, pathLength);

#elif defined SYSTEM_DARWIN

    char exePath[PATH_MAX];

    unsigned int len = (unsigned int)PATH_MAX;

    if (_NSGetExecutablePath(exePath, &len) == 0)
    {
        char * realPath = realpath(exePath, 0x0);

        if (realPath == 0x0)
        {
            invalidateStringOutParameter(path, pathLength);
            return;
        }

        unsigned int len = strlen(realPath);
        copyToStringOutParameter(realPath, len, path, pathLength);

        free(realPath);
    }
    else
    {
        char * intermediatePath = (char *)malloc(sizeof(char) * len);

        if (_NSGetExecutablePath(intermediatePath, &len) != 0)
        {
            free(intermediatePath);
            invalidateStringOutParameter(path, pathLength);
            return;
        }

        char * realPath = realpath(intermediatePath, 0x0);

        free(intermediatePath);

        if (realPath == 0x0)
        {
            invalidateStringOutParameter(path, pathLength);
            return;
        }

        unsigned int len = strlen(realPath);
        copyToStringOutParameter(realPath, len, path, pathLength);

        free(realPath);
    }

#elif defined SYSTEM_FREEBSD

    char exePath[2048];

    unsigned int len = 2048;

    int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

    if (sysctl(mib, 4, exePath, &len, 0x0, 0) != 0)
    {
        invalidateStringOutParameter(path, pathLength);
        return;
    }

    copyToStringOutParameter(exePath, len, path, pathLength);

#else

    invalidateStringOutParameter(path, pathLength);

#endif
}

void getBundlePath(char ** path, unsigned int * pathLength)
{
    if (!checkStringOutParameter(path, pathLength))
    {
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
        free(executablePath);
        invalidateStringOutParameter(path, pathLength);
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
    if (!checkStringOutParameter(path, pathLength))
    {
        return;
    }

    char * executablePath = 0x0;
    unsigned int executablePathLength = 0;
    getExecutablePath(&executablePath, &executablePathLength);
    unsigned int executablePathDirectoryLength = 0;

    getDirectoryPart(executablePath, executablePathLength, &executablePathDirectoryLength);

    copyToStringOutParameter(executablePath, executablePathDirectoryLength, path, pathLength);

    free(executablePath);
}

void getLibraryPath(void * symbol, char ** path, unsigned int * pathLength)
{
    if (!checkStringOutParameter(path, pathLength))
    {
        return;
    }

    if (!symbol)
    {
        invalidateStringOutParameter(path, pathLength);
        return;
    }

#if defined SYSTEM_WINDOWS

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

    unsigned int len = strnlen(systemPath, MAX_PATH);

    copyToStringOutParameter(systemPath, len, path, pathLength);

#else

    Dl_info dlInfo;
    dladdr(symbol, &dlInfo);

    if (!dlInfo.dli_fname)
    {
        invalidateStringOutParameter(path, pathLength);
        return;
    }

    unsigned int len = strlen(dlInfo.dli_fname);
    copyToStringOutParameter(dlInfo.dli_fname, len, path, pathLength);

#endif

    // unifyPathDelimiters(*path, *pathLength);
}

void locatePath(char ** path, unsigned int * pathLength, const char * relPath, unsigned int relPathLength,
    const char * systemDir, unsigned int systemDirLength, void * symbol)
{
    if (!checkStringOutParameter(path, pathLength))
    {
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

    unsigned int maxLength = executablePathDirectoryLength;
    maxLength = maxLength > libraryPathDirectoryLength ? maxLength : libraryPathDirectoryLength;
    maxLength = maxLength > bundlePathLength + 19 ? maxLength : bundlePathLength + 19; // for "/Contents/Resources"
    maxLength += relPathLength + 7 + 2; // for the extra upward path checks, the extra path delimiter and null byte suffix

    const char * dirs[] = { libraryPath, executablePath, bundlePath };
    const unsigned int lengths[] = { libraryPathDirectoryLength, executablePathDirectoryLength, bundlePathLength };

    char * subdir = (char *)malloc(sizeof(char) * maxLength);
    unsigned int subdirLength = 0;
    unsigned int resultdirLength = 0;
    for (unsigned char i = 0; i < 3; ++i)
    {
        const char * dir = dirs[i];
        const unsigned int length = lengths[i];

        if (length <= 0)
        {
            continue;
        }

        memcpy(subdir, dir, length);

        // Check <basedir>/<relpath>, <basedir>/../<relpath>, and <basedir>/../../<relpath>
        for (unsigned char j = 0; j < 3; ++j)
        {
            unsigned char relDirLength = j * 3 + 1;

            subdirLength = length;
            memcpy(subdir+subdirLength, "/../../", relDirLength);
            subdirLength += relDirLength;
            memcpy(subdir+subdirLength, relPath, relPathLength);
            resultdirLength = subdirLength;
            subdirLength += relPathLength;
            subdir[subdirLength] = 0;

            if (fileExists(subdir, subdirLength))
            {
                copyToStringOutParameter(subdir, resultdirLength, path, pathLength);

                goto out;
            }
        }

        if (systemDirLength <= 0)
            continue;

        // Check if it is a system path

        unsigned int systemBasePathLength;
        getSystemBasePath(dir, length, &systemBasePathLength);

        if (systemBasePathLength > 0)
        {
            subdirLength = systemBasePathLength;
            memcpy(subdir+subdirLength, "/", 1);
            subdirLength += 1;
            memcpy(subdir+subdirLength, systemDir, systemDirLength);
            subdirLength += systemDirLength;
            memcpy(subdir+subdirLength, "/", 1);
            subdirLength += 1;
            memcpy(subdir+subdirLength, relPath, relPathLength);
            resultdirLength = subdirLength;
            subdirLength += relPathLength;
            subdir[subdirLength] = 0;
            if (fileExists(subdir, subdirLength))
            {
                copyToStringOutParameter(subdir, resultdirLength, path, pathLength);

                goto out;
            }
        }

        // Check app bundle resources
        if (i == 2) // only for bundle path
        {
            subdirLength = length;
            memcpy(subdir+subdirLength, "/Contents/Resources/", 20);
            subdirLength += 20;
            memcpy(subdir+subdirLength, relPath, relPathLength);
            resultdirLength = subdirLength;
            subdirLength += relPathLength;
            subdir[subdirLength] = 0;
            if (fileExists(subdir, subdirLength))
            {
                copyToStringOutParameter(subdir, resultdirLength, path, pathLength);

                goto out;
            }
        }
    }

    // Could not find path
    invalidateStringOutParameter(path, pathLength);

out:
    free(libraryPath);
    free(executablePath);
    free(bundlePath);
    free(subdir);
}
