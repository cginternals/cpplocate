
#include <cpplocate/utils.h>

#include <algorithm>
#include <sstream>

#ifdef SYSTEM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#else
    #include <sys/stat.h>
#endif

#include <cpplocate/ModuleInfo.h>


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
    // Replace all occurences in the string
    size_t startPos = 0;
    while ((startPos = str.find(original, startPos)) != std::string::npos) {
        str.replace(startPos, original.length(), substitute);
        startPos += substitute.length();
    }
}

void trim(std::string & str)
{
    str.erase(0, str.find_first_not_of(' '));
    str.erase(str.find_last_not_of(' ') + 1);
}

std::string trimPath(const std::string & path)
{
    std::string trimmed = path;

    trimmed.erase(0, trimmed.find_first_not_of(' '));
    trimmed.erase(trimmed.find_last_not_of(' ') + 1);
    trimmed.erase(trimmed.find_last_not_of(pathDelim) + 1);

    return trimmed;
}

std::string unifiedPath(const std::string & path)
{
    std::string str = path;
    std::replace(str.begin(), str.end(), '\\', '/');

    return str;
}

std::string getDirectoryPath(const std::string & fullpath)
{
    std::string dirPath = "";

    if (fullpath.size() > 0) {
        size_t pos     = fullpath.rfind("/");
        size_t posBack = fullpath.rfind("\\");
        if (pos == std::string::npos || (posBack != std::string::npos && posBack > pos)) {
            pos = posBack;
        }

        dirPath = fullpath.substr(0, pos);
    }

    return dirPath;
}

size_t posAfterString(const std::string & str, const std::string & substr)
{
    size_t pos = str.rfind(substr);

    if (pos != std::string::npos) {
        pos += substr.size();
    }

    return pos;
}

std::string getSystemBasePath(const std::string & path)
{
    size_t pos;

    if ((pos = posAfterString(path, "/usr/bin/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/local/bin/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/lib/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/lib32/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/lib64/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/local/lib/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/local/lib32/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    else if ((pos = posAfterString(path, "/usr/local/lib64/")) != std::string::npos) {
        return path.substr(0, pos);
    }

    return "";
}

void split(const std::string & str, char delim, std::vector<std::string> & values)
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
    std::string result = "";

    for (const std::string & value : values)
    {
        if (!result.empty()) {
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
    const char * value = std::getenv(name.c_str());
    if (value) {
        return std::string(value);
    } else {
        return "";
    }
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

bool loadModule(const std::string & directory, const std::string & name, ModuleInfo & info)
{
    // Validate directory path
    std::string dir = trimPath(directory);

    // Try to load module
    std::string path = dir + pathDelim + name + ".modinfo";
    return info.load(path);
}


} // namespace utils

} // namespace cpplocate
