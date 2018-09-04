
#include <cpplocate/cpplocate.h>

#include <stdlib.h>

#include <liblocate/liblocate.h>


namespace cpplocate
{


std::string getExecutablePath()
{
    char * path = nullptr;
    unsigned int length = 0;

    ::getExecutablePath(&path, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}

std::string getBundlePath()
{
    char * path = nullptr;
    unsigned int length = 0;

    ::getBundlePath(&path, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}

std::string getModulePath()
{
    char * path = nullptr;
    unsigned int length = 0;

    ::getModulePath(&path, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}

std::string getLibraryPath(void * symbol)
{
    char * path = nullptr;
    unsigned int length = 0;

    ::getLibraryPath(symbol, &path, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}

std::string locatePath(const std::string & relPath, const std::string & systemDir, void * symbol)
{
    char * path = nullptr;
    unsigned int length = 0;

    ::locatePath(&path, &length, relPath.c_str(), (unsigned int)relPath.size(), systemDir.c_str(), (unsigned int)systemDir.size(), symbol);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}

std::string pathSeperator()
{
    char sep;

    ::pathSeperator(&sep);

    return std::string(1, sep);
}

std::string libPrefix()
{
    char * prefix = nullptr;
    unsigned int length = 0;

    ::libPrefix(&prefix, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(prefix, length);
    }

    free(prefix);

    return result;
}

std::string libExtension()
{
    char * extension = nullptr;
    unsigned int length = 0;

    ::libExtension(&extension, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(extension, length);
    }

    free(extension);

    return result;
}

std::string homeDir()
{
    char * dir = nullptr;
    unsigned int length = 0;

    ::homeDir(&dir, &length);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(dir, length);
    }

    free(dir);

    return result;
}

std::string configDir(const std::string & application)
{
    char * dir = nullptr;
    unsigned int length = 0;

    ::configDir(&dir, &length, application.c_str(), (unsigned int)application.size());

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(dir, length);
    }

    free(dir);

    return result;
}


} // namespace cpplocate
