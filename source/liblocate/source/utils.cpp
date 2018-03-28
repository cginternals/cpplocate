
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


namespace cpplocate
{


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

std::string unifiedPath(const std::string & path)
{
    return unifiedPath(std::string(path));
}

std::string unifiedPath(std::string && path)
{
    auto str = std::move(path);

    std::replace(str.begin(), str.end(), '\\', '/');

    return str;
}

std::string getDirectoryPath(const std::string & fullpath)
{
    if (fullpath.empty())
    {
        return "";
    }

    auto pos           = fullpath.rfind("/");
    const auto posBack = fullpath.rfind("\\");

    if (pos == std::string::npos || (posBack != std::string::npos && posBack > pos))
    {
        pos = posBack;
    }

    return fullpath.substr(0, pos);
}

size_t posAfterString(const std::string & str, const std::string & substr)
{
    const auto pos = str.rfind(substr);

    return pos != std::string::npos ? pos + substr.size() : std::string::npos;
}

std::string getSystemBasePath(const std::string & path)
{
    static const std::vector<std::pair<std::string, int>> systemPaths = {
        { "/usr/bin/", 4 },
        { "/usr/local/bin/", 4 },
        { "/usr/lib/", 4 },
        { "/usr/lib32/", 6 },
        { "/usr/lib64/", 6 },
        { "/usr/local/lib/", 4 },
        { "/usr/local/lib32/", 6 },
        { "/usr/local/lib64/", 6 }
    };

    for (const auto & pair : systemPaths)
    {
        auto pos = std::string::npos;

        if ((pos = posAfterString(path, pair.first)) != std::string::npos)
        {
            return path.substr(0, pos - pair.second);
        }
    }

    return "";
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

bool fileExists(const std::string & path)
{
#ifdef SYSTEM_WINDOWS

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    return (GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &fileInfo) != 0);

#else

    struct stat fileInfo;
    return (stat(path.c_str(), &fileInfo) == 0);

#endif
}


} // namespace utils


} // namespace cpplocate
