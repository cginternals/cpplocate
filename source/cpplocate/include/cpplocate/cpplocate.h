
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
