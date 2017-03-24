
#pragma once


#include <string>
#include <vector>

#include <cpplocate/cpplocate_api.h>


namespace cpplocate
{


class ModuleInfo;


namespace utils
{


/**
*  @brief
*    Replace all occurences of a substring in string
*
*  @param[in,out] str
*    String
*  @param[in] original
*    Original string
*  @param[in] substitute
*    Substitute string
*/
CPPLOCATE_API void replace(std::string & str, const std::string & original, const std::string & substitute);

/**
*  @brief
*    Remove spaces at the beginning and the end of a string
*
*  @param[in,out] str
*    String
*/
CPPLOCATE_API void trim(std::string & str);

/**
*  @brief
*    Remove spaces and trailing slash/backslash from path
*
*  @param[in] path
*    Path
*
*  @return
*    Trimmed path
*/
CPPLOCATE_API std::string trimPath(const std::string & str);

/**
*  @brief
*    Convert path into unified form (replace '\' with '/')
*
*  @param[in] path
*    Path
*
*  @return
*    Path with forward slashes
*/
CPPLOCATE_API std::string unifiedPath(const std::string & path);

/**
*  @brief
*    Cut away filename portion of a path, get path to directory
*
*  @param[in] fullpath
*    Path (e.g., '/path/to/file.txt')
*
*  @return
*    Directory path (e.g., '/path/to')
*/
CPPLOCATE_API std::string getDirectoryPath(const std::string & fullpath);

/**
*  @brief
*    Get position right after given substring (searched from right to left)
*
*  @param[in] str
*    Full string
*  @param[in] substr
*    Substring to search for
*
*  @return
*    Position after substring if substring was found, else string::npos
*/
CPPLOCATE_API size_t posAfterString(const std::string & str, const std::string & substr);

/**
*  @brief
*    Get system base path for path to library or executable
*
*  @param[in] path
*    Path to library or executable (e.g., '/usr/bin/myapp')
*
*  @return
*    System path path (e.g., '/usr')
*
*  @remarks
*    This function returns the base path if the given file
*    is a system path. Otherwise, it returns an empty string.
*
*    Examples:
*      '/usr/bin/myapp' -> '/usr'
*      '/usr/local/bin/myapp' -> '/usr/local'
*      '/usr/lib/mylib.so' -> '/usr'
*      '/usr/lib64/mylib.so' -> '/usr'
*      '/usr/local/lib64/mylib.so' -> '/usr/local'
*      '/crosscompile/armv4/usr/lib/mylib.so.2' -> '/crosscompile/armv4/usr'
*/
CPPLOCATE_API std::string getSystemBasePath(const std::string & path);

/**
*  @brief
*    Split string into array of strings
*
*  @param[in] str
*    Input string
*  @param[in] delim
*    Delimiter used to split the string
*  @param[out] values
*    Output vector
*/
CPPLOCATE_API void split(const std::string & str, char delim, std::vector<std::string> & values);

/**
*  @brief
*    Join array of strings into a single string
*
*  @param[out] values
*    Input vector
*  @param[in] delim
*    Delimiter used to join the string
*
*  @return
*    Joined string
*/
CPPLOCATE_API std::string join(const std::vector<std::string> & values, const std::string & delim);

/**
*  @brief
*    Split list of paths separated by : or ; into list of paths
*
*  @param[in] paths
*    Input string
*  @param[out] values
*    Output vector
*
*  @remarks
*    On windows, the separator ';' is used, ':' on all other systems
*/
CPPLOCATE_API void getPaths(const std::string & str, std::vector<std::string> & values);

/**
*  @brief
*    Get value of environment variable
*
*  @param[in] name
*    Name of environment variable
*
*  @return
*    Value of the environment variable
*/
CPPLOCATE_API std::string getEnv(const std::string & name);

/**
*  @brief
*    Check if file or directory exists
*
*  @param[in] path
*    Path to file or directory
*
*  @return
*    'true' if it exists, else 'false'
*/
CPPLOCATE_API bool fileExists(const std::string & path);

/**
*  @brief
*    Load module information file
*
*  @param[in] directory
*    Path to directory
*  @param[in] name
*    Module name
*  @param[out] info
*    Module information
*
*  @return
*    'true' if module could be loaded successfully, else 'false'
*
*  @remarks
*    The filename is constructed as <directory>/<name>.modinfo
*/
CPPLOCATE_API bool loadModule(const std::string & directory, const std::string & name, ModuleInfo & info);


} // namespace utils

} // namespace cpplocate
