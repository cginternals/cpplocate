
#pragma once


#include <string>

#include <cpplocate/cpplocate_api.h>


namespace cpplocate
{


/**
*  @brief
*    Get path to the current executable
*
*  @return
*    Path to executable (including filename)
*
*  @remark
*    The path is returned in native format, e.g., backslashes on Windows.
*
*  @remark
*    It is assumed the executable path is static throughout the process.
*/
CPPLOCATE_API std::string getExecutablePath();

/**
*  @brief
*    Get path to the current application bundle
*
*  @return
*    Path to bundle (including filename)
*
*  @remark
*    The path is returned in unified format (forward slashes).
*
*  @remark
*    If the current executable is part of a macOS application bundle,
*    this function returns the part to the bundle. Otherwise, an
*    empty string is returned.
*
*  @remark
*    It is assumed the bundle path is static throughout the process.
*/
CPPLOCATE_API std::string getBundlePath();

/**
*  @brief
*    Get path to the current module
*
*  @return
*    Path to module (directory in which the executable is located)
*
*  @remark
*    The path is returned in unified format (forward slashes).
*
*  @remark
*    It is assumed the executable name is static throughout the process.
*/
CPPLOCATE_API std::string getModulePath();

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
*  @remark
*    The path is returned in unified format (forward slashes).
*
*  @remark
*    If symbol is nullptr, an empty string is returned.
*/
CPPLOCATE_API std::string getLibraryPath(void * symbol);

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
*  @remark
*    This function tries to locate the named file or directory based
*    on the location of the current executable or library. If the
*    file or directory could be found, the base path from which the
*    relative path can be resolved is returned. Otherwise, an empty
*    string is returned.
*
*  @remark
*    The path is returned in unified format (forward slashes).
*/
CPPLOCATE_API std::string locatePath(const std::string & relPath, const std::string & systemDir, void * symbol);


} // namespace cpplocate
