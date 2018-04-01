
#include <liblocate/utils.h>

#include <stdlib.h>
#include <string.h>

#ifdef SYSTEM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif


const char windowsPathDelim = '\\';
const char windowssPathDelim = ';';

const char unixPathDelim = '/';
const char unixPathsDelim = ':';

#ifdef SYSTEM_WINDOWS
    const char pathDelim = windowsPathDelim;
    const char pathsDelim = windowssPathDelim;
#else
    const char pathDelim = unixPathDelim;
    const char pathsDelim = unixPathsDelim;
#endif


void unifiedPath(char * path, unsigned int pathLength)
{
    if (path == 0x0 || pathLength == 0)
    {
        return;
    }

    const char * end = path + pathLength;

    for (char * iter = path; iter < end; ++iter)
    {
        if (*iter == windowsPathDelim)
        {
            *iter = unixPathDelim;
        }
    }
}

void getDirectoryPath(const char * fullpath, unsigned int length, unsigned int * newLength)
{
    if (newLength == 0x0)
    {
        return;
    }

    if (fullpath == 0x0 || length == 0)
    {
        *newLength = 0;
        return;
    }

    const char * iter = fullpath + length - 1;
    while (*iter != unixPathDelim && *iter != windowsPathDelim && iter > fullpath)
    {
        --iter;
    }

    if (iter > fullpath)
    {
        *newLength = iter - fullpath + 1;
    }
    else
    {
        *newLength = length;
    }
}

void getSystemBasePath(const char * path, unsigned int pathLength, unsigned int * subLength)
{
    static const char * systemPaths[8] = {
        "/usr/bin/",
        "/usr/local/bin/",
        "/usr/lib/",
        "/usr/lib32/",
        "/usr/lib64/",
        "/usr/local/lib/",
        "/usr/local/lib32/",
        "/usr/local/lib64/",
    };
    static const unsigned int systemPathLengths[8] = { 9, 15, 9, 11, 11, 15, 17, 17 };
    static const unsigned int systemPathSuffixesLength[8] = { 4, 4, 4, 6, 6, 4, 6, 6};

    if (subLength == 0x0)
    {
        return;
    }

    if (path == 0x0 || pathLength == 0)
    {
        *subLength = 0;
        return;
    }

    for (int i = 0; i < 8; ++i)
    {
        const char * systemPath = systemPaths[i];
        unsigned int systemPathLength = systemPathLengths[i];

        const char * iter = path + pathLength - 1;
        const char * searchIter = systemPath + systemPathLength - 1;
        while (searchIter >= systemPath && iter >= path)
        {
            if (*iter == *searchIter) //
            {
                --searchIter;
            }
            else
            {
                searchIter = systemPath + systemPathLength - 1;
            }

            --iter;
        }

        if (searchIter < systemPath) // sub-systemPath-string found
        {
            *subLength = static_cast<unsigned int>(iter - path) + systemPathLength - systemPathSuffixesLength[i] + 1;
            return;
        }
    }

    *subLength = 0;
}

void getEnv(const char * name, unsigned int /*nameLength*/, char ** value, unsigned int * valueLength)
{
    if (name == 0x0 || value == 0x0)
    {
        if (valueLength != 0x0)
        {
            *valueLength = 0;
        }

        return;
    }

    const char * systemValue = getenv(name);

    if (systemValue == 0x0)
    {
        *value = nullptr;
        if (valueLength != 0x0)
        {
            *valueLength = 0;
        }

        return;
    }

    unsigned int systemValueLength = strlen(systemValue);

    if (systemValue == nullptr || systemValueLength == 0)
    {
        *value = nullptr;
        if (valueLength != 0x0)
        {
            *valueLength = 0;
        }

        return;
    }

    *value = reinterpret_cast<char *>(malloc(sizeof(char) * systemValueLength));
    memcpy(*value, systemValue, systemValueLength);
    if (valueLength != 0x0)
    {
        *valueLength = systemValueLength;
    }
}

bool fileExists(const char * path, unsigned int /*pathLength*/)
{
#ifdef SYSTEM_WINDOWS

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    return (GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo) != 0);

#else

    struct stat fileInfo;
    return (stat(path, &fileInfo) == 0);

#endif
}
