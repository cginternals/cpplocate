
#include <liblocate/liblocate.h>

#include <stdlib.h>


namespace liblocate
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

    ::locatePath(&path, &length, relPath.c_str(), relPath.size(), systemDir.c_str(), systemDir.size(), symbol);

    auto result = std::string();

    if (length > 0)
    {
        result = std::string(path, length);
    }

    free(path);

    return result;
}


} // namespace liblocate
