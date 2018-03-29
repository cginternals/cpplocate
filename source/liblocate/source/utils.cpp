
#include <liblocate/utils.h>

#include <algorithm>
#include <sstream>

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


namespace utils
{


void replace(std::string & str, const std::string & original, const std::string & substitute)
{
    // Count number of substring occurrences
    auto pos = str.find(original, 0);
    auto count = size_t(0);

    while (pos != std::string::npos)
    {
        ++count;
        pos += original.size();

        pos = str.find(original, pos);
    }

    auto result = std::string(str.size() + count * (substitute.size() - original.size()), 0);

    // Build string with replacements
    auto lastPos = size_t(0);
    /*auto*/ pos = str.find(original, 0);
    auto current = result.begin();

    while (pos != std::string::npos)
    {
        std::copy(str.begin()+lastPos, str.begin()+pos, current);

        current += pos - lastPos;

        std::copy(substitute.begin(), substitute.end(), current);

        current += substitute.size();
        pos += original.size();
        lastPos = pos;

        pos = str.find(original, lastPos);
    }

    std::copy(str.begin()+lastPos, str.end(), current);

    // Swap
    std::swap(str, result);
}

void trim(std::string & str)
{
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
}

std::string trimPath(const std::string & path)
{
    return trimPath(std::string(path));
}

std::string trimPath(std::string && path)
{
    auto trimmed = std::move(path);

    trimmed.erase(0, trimmed.find_first_not_of(' '));
    trimmed.erase(trimmed.find_last_not_of(' ') + 1);
    trimmed.erase(trimmed.find_last_not_of(pathDelim) + 1);

    return trimmed;
}

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

void split(const std::string & str, const char delim, std::vector<std::string> & values)
{
    std::stringstream stream(str);

    std::string item;
    while (std::getline(stream, item, delim))
    {
        if (!item.empty())
        {
            values.push_back(item);
        }
    }
}

std::string join(const std::vector<std::string> & values, const std::string & delim)
{
    auto result = std::string("");

    for (const std::string & value : values)
    {
        if (!result.empty())
        {
            result += delim;
        }

        result += value;
    }

    return result;
}


void getPaths(const std::string & paths, std::vector<std::string> & values)
{
    split(paths, pathsDelim, values);
}

std::string getEnv(const std::string & name)
{
    const auto value = std::getenv(name.c_str());

    return value ? std::string(value) : std::string();
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


} // namespace utils
