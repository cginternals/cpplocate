# C++ Locator Library

```cpplocate``` is a small C++ library that provides tools for applications to locate themselves,
their data assets, as well as dependend modules. It is cross-platform and designed to work
on all major platforms such as Linux, Windows, and OS X.

Especially in the context of cross-platform applications, locating data assets belonging
to an application or library is not an easy task. Depending on the situation, data assets can
occupy different locations on a system: in a development tree, data might be located relative
to the executable in a build directory, when installed into the system, they might be located
in a global directory such as /usr/share or C:\Users\...\AppData, while for self-contained
installs they might be located relative to the executable. When projects are deployed using
software installers or archives, the final location can be controlled by the user installing
the software, therefore it cannot be known at build-time. The situation gets even more complicated
when an application does not only need access to its own data assets, but depends on other modules,
such as dynamic libraries or even plugins, which bring their own data. However, a software should be
able to locate its data assets as easy as possible and still be relocatable for typical deployment
and installation routines to work as expected.

To help resolve these problems, ```cpplocate``` provides the following features:

## Get absolute path to the current executable

```cpplocate``` offers a cross-platform implementation to obtain the location of the current executable
in most cases. This can be used by the application itself to locate its data.

## Module information files

Module information files contain meta-information about a module, such as project name,
version, and vendor. They also provide the information of where to find data assets for
that module based on the location of the module information file itself, or, in some cases,
using an absolute path.

A module information file has the filename ```<projectname>.modinfo``` and contains an arbitrary
number of key/value-pairs, e.g.:

```cmake
  name: examplelib
  version: 1.0.0
  description: Example library
  author: Example organization
  dataPath: ${ModulePath}/data
```

The keys are purely conventional and can be used in any way as defined by the using application.
To express file paths relative to the module information file, the placeholder variable ```${ModulePath}```
can be used. When loading the module information, this variable will be replaced with the path
to the directory containing the module information file.

## CMake integration

A CMake module is provided for creating module information files automatically, taking into account
different situations such as build-time (finding modules and data in a development tree) and
install-time (finding modules and data from an installed location).

Use ```find_package(cpplocate)``` to find the cpplocate library, pointing ```CMAKE_PREFIX_PATH``` to the
directory that contains ```cpplocate```. This will locate the library and also include the necessary
cmake functions into your project.

The following cmake functions are provided by ```cpplocate```:

### ```generate_module_info```: Define a module information file.

```cmake
generate_module_info(<project_id>;
    VALUES
    [<key> <value>]*

    BUILD_VALUES
    [<key> <value>]*

    INSTALL_VALUES
    [<key> <value>]*
)
```

Define a module information file for a module named by ```project_id```. The module name can be chosen
arbitrarily and can, but does not need to, match the name of the project or a library. The
filename will be derived as ```<project_id>.modinfo```.

The generated module information file will contain all key/value-pairs specified after ```VALUES```.
When created into the build-directory of the current development tree, the values specified
after ```BUILD_VALUES``` will also be added. But when installed using the cmake ```INSTALL```-target, the
values after ```INSTALL_VALUES``` will be used instead. This allows for providing different values
in the development tree and in installed location, e.g., the data path might point to the
absolute path in the development tree, but be defined relative to the module file on install.

Example usage:

```cmake
generate_module_info(examplelib
    VALUES
    name        "examplelib"
    version     "1.0.0"
    description "Example library"
    author      "Example organization"

    BUILD_VALUES
    dataPath    "${PROJECT_SOURCE_DIR}/data"

    INSTALL_VALUES
    dataPath    "\${ModulePath}/data"
)
```

### ```export_module_info```: Create module information file in build directory.

```cmake
export_module_info(<project_id>
    TARGET <target>
    [FOLDER <folder>]
)
```

Creates the actual module information file for a module named by ```project_id``` in the build directory.
It uses the values from ```VALUES``` and ```BUILD_VALUES```. This is executed at build-time, providing a target
named ```<target>-modinfo```. If ```FOLDER``` is specified, the target is put into the UI folder named by folder
by setting the target property ```FOLDER``` accordingly.

Example usage:

```cmake
export_module_info(examplelib TARGET examplelib FOLDER "cmake")
```

### ```install_module_info```: Install modinfo file to target path.

```cmake
install_module_info(<project_id>
    DESTINATION <dest>
    [COMPONENT <component>]
)
```

Creates an installation rule to install a module information file named by ```project_id```. It uses
the values from ```VALUES``` and ```INSTALL_VALUES```. The destination location is specified by ```dest```. If
```COMPONENT``` is specified, the module information file is added to the specified installation component.

Example usage:

```cmake
install_module_info(examplelib DESTINATION "." COMPONENT dev)
```

### ```copy_module_info```: Copy module information file to a specific location at build-time.

```cmake
copy_module_info(<project_id> <dest>)
```

This function creates a module information file named by ```project_id``` at the location
specified by dest at build-time. It uses the values from ```VALUES``` and ```BUILD_VALUES```.

## Resolve dependend modules

Similar to a dynamic linker, ```cpplocate``` can resolve dependencies to other modules by locating
module information files. The search for modules is conducted in the following order:

1. in the directory of the current executable (not the working directory!)

2. in the directories provided by the environment variable ```CPPLOCATE_PATH```
   - ```<path>/<module>-info.modinfo```
   - ```<path>/<module>/<module>-info.modinfo```

3. in standard locations:
   - ```C:\Program Files\<module>\<module>-info.modinfo```
   - ```/usr/share/<module>/<module>-info.modinfo```
   - ```/usr/local/share/<module>/<module>-info.modinfo```

This functionality can be used by a library to locate its own data at runtime.
If it is used as a dependency for another project, such as an application or a
plugin using the library, it cannot rely on the data being relative to the
current executable or even the working directory. Therefore, the library can
use ```cpplocate``` to locate itself:

```C++
namespace examplelib
{

std::string g_dataPath;

std::string dataPath()
{
    if (g_dataPath.empty())
    {
        cpplocate::ModuleInfo modInfo = cpplocate::findModule("examplelib");
        if (!modInfo.empty()) {
            g_dataPath = modInfo.value("dataPath");
        } else {
            g_dataPath = "data";
        }
    }

    return g_dataPath;
}

}
```

And whenever data needs to be accessed, code like the following should be used:

```C++
std::string filename = dataPath() + "/textures/logo.png";
```
