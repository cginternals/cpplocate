
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
