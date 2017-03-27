
#pragma once


#include <string>

#include <cpplocate/cpplocate_api.h>


namespace cpplocate
{


class ModuleInfo;


/**
*  @brief
*    Get path to the current executable
*
*  @return
*    Path to executable (including filename)
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*/
CPPLOCATE_API std::string getExecutablePath();

/**
*  @brief
*    Get path to dynamic library
*
*  @param[in] symbol
*    A symbol from the library, e.g., a function or variable pointer
*
*  @return
*    Path to library (including filename)
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*    If symbol is nullptr, an empty string is returned.
*/
CPPLOCATE_API std::string getLibraryPath(void * symbol);

/**
*  @brief
*    Get path to the current application bundle
*
*  @return
*    Path to bundle (including filename)
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*    If the current executable is part of a macOS application bundle,
*    this function returns the part to the bundle. Otherwise, an
*    empty string is returned.
*/
CPPLOCATE_API std::string getBundlePath();

/**
*  @brief
*    Locate path to a file or directory
*
*  @param[in] relPath
*    Relative path to a file or directory (e.g., 'data/logo.png')
*  @param[in] systemDir
*    Subdirectory for system installs (e.g., 'share/myappname')
*  @param[in] symbol
*    A symbol from the library, e.g., a function or variable pointer
*
*  @return
*    Path to file or directory
*
*  @remarks
*    This function tries to locate the named file or directory based
*    on the location of the current executable or library. If the
*    file or directory could be found, the base path from which the
*    relative path can be resolved is returned. Otherwise, an empty
*    string is returned.
*
*    The path is returned in unified format (forward slashes).
*/
CPPLOCATE_API std::string locatePath(const std::string & relPath, const std::string & systemDir = "", void * symbol = nullptr);

/**
*  @brief
*    Get path to the current module
*
*  @return
*    Path to module (directory in which the executable is located)
*
*  @remarks
*    The path is returned in unified format (forward slashes).
*/
CPPLOCATE_API std::string getModulePath();

static std::string getDllPath();

/**
*  @brief
*    Tries to locate a module
*
*  @param[in] name
*    Module name (e.g., "mymodule")
*
*  @return
*    Module information, empty on error
*
*  @remarks
*    This functions looks for the filename "<name>.modinfo".
*
*    It searches the following locations:
*    1. The current executable location
*    2. The current library location
*    3. All pathes contained in the enironment variable CPPLOCATE_PATH
*    3.a <path>/<name>.modinfo
*    3.b <path>/<name>/<name>.modinfo
*    4. Standard locations:
*    4.a C:\Program Files\<name>\<name>.modinfo
*    4.b /usr/share/<name>/<name>.modinfo
*    4.c /usr/local/share/<name>/<name>.modinfo
*/
static ModuleInfo findModule(const std::string & name);


CPPLOCATE_API ModuleInfo findModule(const std::string & name, const std::string & dllPath);


} // namespace cpplocate


#include <cpplocate/cpplocate.inl>
