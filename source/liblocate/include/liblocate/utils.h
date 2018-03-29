
#pragma once


#include <string>
#include <vector>

#include <liblocate/liblocate_api.h>


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
LIBLOCATE_API void replace(std::string & str, const std::string & original, const std::string & substitute);

/**
*  @brief
*    Remove spaces at the beginning and the end of a string
*
*  @param[in,out] str
*    String
*/
LIBLOCATE_API void trim(std::string & str);

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
LIBLOCATE_API std::string trimPath(const std::string & str);

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
LIBLOCATE_API std::string trimPath(std::string && str);

/**
*  @brief
*    Convert path into unified form (replace '\' with '/')
*
*  @param[inout] path
*    Path
*  @param[in] pathLength
*    The length of path
*/
LIBLOCATE_API void unifiedPath(char * path, unsigned int pathLength);


/**
*  @brief
*    Cut away filename portion of a path, get path to directory
*
*  @param[in] fullpath
*    Path (e.g., '/path/to/file.txt')
*  @param[in] length
*    Length of fullpath (excluding null byte)
*  @param[out] newLength
*    The end of the substring representing the directory path (e.g., '/path/to')
*/
LIBLOCATE_API void getDirectoryPath(const char * fullpath, unsigned int length, unsigned int * newLength);

/**
*  @brief
*    Get system base path for path to library or executable
*
*  @param[in] path
*    Path to library or executable (e.g., '/usr/bin/myapp')
*  @param[in] pathLength
*    The length of the path
*  @param[out] systemPathLength
*    The last position of the system part of the path (e.g., '/usr')
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
LIBLOCATE_API void getSystemBasePath(const char * path, unsigned int pathLength, unsigned int * systemPathLength);

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
LIBLOCATE_API void split(const std::string & str, char delim, std::vector<std::string> & values);

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
LIBLOCATE_API std::string join(const std::vector<std::string> & values, const std::string & delim);

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
LIBLOCATE_API void getPaths(const std::string & str, std::vector<std::string> & values);

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
LIBLOCATE_API std::string getEnv(const std::string & name);

/**
*  @brief
*    Check if file or directory exists
*
*  @param[in] path
*    Path to file or directory
*  @param[in] pathLength
*    Length of path
*
*  @return
*    'true' if it exists, else 'false'
*/
LIBLOCATE_API bool fileExists(const char * path, unsigned int pathLength);


} // namespace utils
