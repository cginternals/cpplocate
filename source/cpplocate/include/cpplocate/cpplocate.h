
#pragma once


#include <string>

#include <cpplocate/cpplocate_api.h>


namespace cpplocate
{


CPPLOCATE_API std::string getExecutablePath();

CPPLOCATE_API std::string getBundlePath();

CPPLOCATE_API std::string getModulePath();

CPPLOCATE_API std::string getLibraryPath(void * symbol);

CPPLOCATE_API std::string locatePath(const std::string & relPath, const std::string & systemDir, void * symbol);


} // namespace cpplocate
