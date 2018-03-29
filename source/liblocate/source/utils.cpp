
#include <liblocate/utils.h>

#include <stdlib.h>
#include <string.h>

#ifdef SYSTEM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif


namespace
{


#ifdef SYSTEM_WINDOWS
    const char pathDelim = '\\';
    const char pathsDelim = ';';
#else
    const char pathDelim = '/';
    const char pathsDelim = ':';
#endif


} // namespace



void unifiedPath(char * path, unsigned int pathLength)
{
    const char * end = path + pathLength;

    for (char * iter = path; iter < end; ++iter)
    {
        if (*iter == '\\')
        {
            *iter = '/';
        }
    }
}

void getDirectoryPath(const char * fullpath, unsigned int length, unsigned int * newLength)
{
    if (*fullpath == 0x0 || length == 0)
    {
        newLength = 0;
        return;
    }

    const char * iter = fullpath + length - 1;
    while (*iter != '/' && *iter != '\\' && iter > fullpath)
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

    for (int i = 0; i < sizeof systemPaths; ++i)
    {
        const char * systemPath = systemPaths[i];
        unsigned int systemPathLength = systemPathLengths[i];

        const char * iter = path + pathLength - 1;
        const char * searchIter = systemPath + systemPathLength - 1;
        while (searchIter >= systemPath)
        {
            if (*iter == *searchIter) //
            {
                --iter;
                --searchIter;
            }
            else
            {
                searchIter = systemPath + systemPathLength - 1;
            }
        }

        if (searchIter < systemPath) // sub-systemPath-string found
        {
            *subLength = static_cast<unsigned int>(path - iter) + systemPathLength - systemPathSuffixesLength[i];
            return;
        }
    }

    *subLength = 0;
}

void getEnv(const char * name, unsigned int /*nameLength*/, char ** value, unsigned int * valueLength)
{
    const char * systemValue = getenv(name);
    unsigned int systemValueLength = strlen(systemValue);

    if (systemValue == nullptr || systemValueLength == 0)
    {
        *value = nullptr;
        *valueLength = 0;
        return;
    }

    *value = reinterpret_cast<char *>(malloc(sizeof(char) * systemValueLength));
    memcpy(*value, systemValue, systemValueLength);
    *valueLength = systemValueLength;
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
