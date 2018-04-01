
#pragma once


#include <liblocate/liblocate_api.h>


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
*    Get value of environment variable
*
*  @param[in] name
*    Name of environment variable
*  @param[in] nameLength
*    The length of name
*  @param[out] value
*    The value of the referenced environment variable
*  @param[out] valueLength
*    The length of value
*
*  @return
*    Value of the environment variable
*/
LIBLOCATE_API void getEnv(const char * name, unsigned int nameLength, char ** value, unsigned int * valueLength);

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