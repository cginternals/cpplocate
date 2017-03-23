
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
*    The path is returned in native format, e.g., backslashes on Windows.
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
*    The path is returned in native format, e.g., backslashes on Windows.
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
*    If the current executable is part of a macOS application bundle,
*    this function returns the part to the bundle. Otherwise, an
*    empty string is returned.
*
*    The path is returned in native format, e.g., backslashes on Windows.
*/
CPPLOCATE_API std::string getBundlePath();

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
*    The path is returned in native format, e.g., backslashes on Windows.
*/
CPPLOCATE_API std::string locatePath(void * symbol);

/**
*  @brief
*    Get path to the current module
*
*  @return
*    Path to module (directory in which the executable is located)
*
*  @remarks
*    The path is returned in native format, e.g., backslashes on Windows,
*    and with no trailing slash.
*/
CPPLOCATE_API std::string getModulePath();

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
*    1. The current module path
*    2. All pathes contained in the enironment variable CPPLOCATE_PATH
*    2.a <path>/<name>.modinfo
*    2.b <path>/<name>/<name>.modinfo
*    3. Standard locations:
*    3.a C:\Program Files\<name>\<name>.modinfo
*    3.b /usr/share/<name>/<name>.modinfo
*    3.c /usr/local/share/<name>/<name>.modinfo
*/
CPPLOCATE_API ModuleInfo findModule(const std::string & name);


} // namespace cpplocate
