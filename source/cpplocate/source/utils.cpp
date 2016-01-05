
#include <cpplocate/utils.h>

#include <sstream>

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
