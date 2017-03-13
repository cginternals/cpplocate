
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

    while (lastPos != std::string::npos)
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
    auto trimmed = std::string(path);

    trimmed.erase(0, trimmed.find_first_not_of(' '));
    trimmed.erase(trimmed.find_last_not_of(' ') + 1);
    trimmed.erase(trimmed.find_last_not_of(pathDelim) + 1);

    return trimmed;
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

void getPaths(const std::string & paths, std::vector<std::string> & values)
{
    split(paths, pathsDelim, values);
}

std::string getEnv(const std::string & name)
{
    const auto value = std::getenv(name.c_str());

    return value ? std::string(value) : std::string();
}

bool loadModule(const std::string & directory, const std::string & name, ModuleInfo & info)
{
    // Validate directory path
    const auto dir = trimPath(directory);

    // Try to load module
    const auto path = dir + pathDelim + name + ".modinfo";

    return info.load(path);
}


} // namespace utils


} // namespace cpplocate
