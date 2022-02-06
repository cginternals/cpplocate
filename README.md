[//]: # (Comment)

<br><a href="https://github.com/cginternals/cpplocate/"><img src="https://raw.githubusercontent.com/cginternals/cpplocate/master/cpplocate-logo.svg?sanitize=true" width="50%"></a>

*cpplocate* is a cross-platform C++ library that provides tools for applications to locate their binary files and data assets, as well as those of dependent modules.

![GitHub release](https://img.shields.io/github/release/cginternals/cpplocate.svg)
[![C++ CI](https://github.com/cginternals/cpplocate/actions/workflows/ci.yml/badge.svg)](https://github.com/cginternals/cpplocate/actions/workflows/ci.yml)
[![Tokei](https://tokei.rs/b1/github/cginternals/cpplocate)](https://github.com/Aaronepower/tokei) 
[![Tokei](https://tokei.rs/b1/github/cginternals/cpplocate?category=comments)](https://github.com/Aaronepower/tokei) 

In the context of cross-platform applications, locating data assets belonging to an application or library is not an easy task.
Depending on the situation, data assets can occupy different locations on a system: in a development tree, data might be located relative to the executable in a build directory, when installed into the system, they might be located in a global directory such as ```/usr/share``` or ```C:\Users\...\AppData```, while for self-contained installs they might be located relative to the executable again.
When projects are deployed using installers or archives, the final location can be controlled by the user installing the software, therefore it cannot be known at build-time.
The situation gets even more complicated when an application does not only need access to its own data assets, but depends on other modules, such as dynamic libraries or even plugins, which bring their own data.
However, software should be able to locate its data assets as easy as possible and still be relocatable for typical deployment and installation routines to work as expected.

*cpplocate* provides interfaces to locate such data assets. Following, a short example on how [glbinding](https://github.com/cginternals/glbinding) examples locate their run-time assets:
```cpp
#include <cpplocate/cpplocate.h>

#include <glbinding/gl/gl.h>

const std::string assetPath = cpplocate::locatePath("data/cubescape", "share/glbinding", 
    reinterpret_cast<void *>(&gl::glCreateShader));
// assetPath now contains the path to the directory containing "data/cubescape"
```


# Resources

* [C Port of *cpplocate*: liblocate](#c-port-of-cpplocate-liblocate)

##### Installation and Development

* [Install Instructions](#install-instructions)
* [Build form Source](#build-instructions)
* [Tips for Linking](#tips-for-linking)

##### Feature Documentation and Code Snippets

* [Query Executable Path](#query-executable-path)
* [Query Library Path](#query-library-path)
* [Query Bundle Path](#query-bundle-path)
* [Query Module Path](#query-module-path)
* [Query Runtime Asset Path](#query-runtime-asset-path)


# Install Instructions

*cpplocate* is available for different platforms using different distribution channels.
You can either download the source and manually [compile](#build-instructions) it or use one of the [pre-compiled releases](https://github.com/cginternals/cpplocate/releases) of this repository.
For systems providing package managers, we generally strive to provide packages.

##### Windows

There is currently no precompiled package maintained. Please download the source code and commence [building from source](#build-instructions).

##### Ubuntu

*cpplocate* is provided on Ubuntu using PPAs.
For Ubuntu 15.04 (vivid) and more recent versions use the [current PPA](https://launchpad.net/~cginternals/+archive/ubuntu/ppa), for Ubuntu 14.04 (trusty) use the [backports PPA](https://launchpad.net/~cginternals/+archive/ubuntu/backports-ppa).
Using the current PPA as example, the following lines install *cpplocate*:

```shell
> sudo apt-add-repository ppa:cginternals/ppa
> sudo apt-get update
> sudo apt-get install libcpplocate
```

To use *cpplocate* as dependency, install the development package:

```shell
> sudo apt-get install libcpplocate-dev libcpplocate-dbg
```

Alternatively, download the source code and commence [building from source](#build-instructions).

##### OS X

There is currently no precompiled package maintained. Please download the source code and commence [building from source](#build-instructions).

##### Debian-based Systems

There is currently no precompiled package maintained. Please download the source code and commence [building from source](#build-instructions).


# Build Instructions

##### Prerequisites and Dependencies

The only mandatory run-time dependencies of *cpplocate* are the STL of the used compiler.
Building *cpplocate* utilizes:
* [CMake](https://cmake.org/) 3.0 or higher for building *cpplocate* from source (mandatory for any build from source)
* [git](https://git-scm.com/) for version control (optional)
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) 1.8 or higher for generating the documentation on your system (optional)
  * [graphviz](http://www.graphviz.org/) for generating diagrams (optional)

##### Compile Instructions

For compilation, a C++11 compliant compiler, e.g., GCC 4.8, Clang 3.3, MSVC 2013 **Update 3**, is required.

First, download the source code [as archive](https://github.com/cginternals/cpplocate/releases) or via git:

```bash
> git clone https://github.com/cginternals/cpplocate.git
> cd cpplocate
```
Then, depending on the version of *cpplocate* you want to build, choose the appropriate tag or branch, e.g., for the 2.0.0 release:

```bash
> git fetch --tags
> git checkout v2.0.0
```

The actual compilation can be done using CMake and your favorite compiler and IDE.

For building *cpplocate* CMake via command line can be used (should work on all systems):

First create a build directory (we do not recommend in-source builds):

```bash
> mkdir build
> cd build
```

Configure *cpplocate* with your prefered or default generator, e.g., for Visual Studio 2017 in x64 use
(note: some IDEs have integrated support for CMake projects, e.g., Qt Creator, and allow you to skip the manual project configuration):

```bash
> cmake .. -G "Visual Studio 15 2017 Win64"
```

In order to compile the project, either use you favorite Editor/IDE with the created project or use CMake as follows:

```bash
> cmake --build .
```


# Tips for Linking

For a smooth integration we suggest using [CMake](https://cmake.org/), the build system employed by *cpplocate*.
In this case, *cpplocate* provides a find configuration script that should be either installed into your system or at least accessible by CMake.
In your project's CMakeLists.txt, add one of the following lines:

```cmake
find_package(cpplocate QUIET) # if you want to check for existence
find_package(cpplocate REQUIRED) # if it is really required in your project
```
Finally, just link *cpplocate* to your own library or executable:

```cmake
target_link_libraries(${target} ... PUBLIC cpplocate::cpplocate)
```


# Examples and Documentation

*cpplocate* supports queries of different paths, depending on the type of the component in question (application, library, or application bundle). For the most basic use case, *cpplocate* is used to detect run-time data that is associated with a module, and we provide a convenience location function.

### Query Executable Path

```cpp
#include <cpplocate/cpplocate.h>

const std::string executablePath = cpplocate::getExecutablePath();
// executablePath now contains the path to the executable (including its filename)
```

### Query Module Path

```cpp
#include <cpplocate/cpplocate.h>

const std::string modulePath = cpplocate::getModulePath();
// modulePath now contains the path to the directory containing the executable
```

### Query Bundle Path

Querying application bundles is only defined for macOS application bundles.
For other platforms or non-bundle applications, empty strings are returned.

```cpp
#include <cpplocate/cpplocate.h>

const std::string bundlePath = cpplocate::getBundlePath();
// bundlePath now contains the path to the root of the application bundle
// (instead of the path to the executable within the bundle)
```

### Query Library Path

To query a library location, you have to specify a pointer to a symbol (e.g., a function pointer or a public static member) that is exposed by the library.

```cpp
#include <cpplocate/cpplocate.h>

#include <QtGlobal>

const std::string libraryPath = cpplocate::getLibraryPath(reinterpret_cast<void *>(&qVersion));
// libraryPath now contains the path to the QtCore library (including its filename)
```

### Query Runtime Asset Path

This function searches for a subdirectory pattern across available system paths, local run paths of the executable or alongside a library that is identified via an exported symbol. This function searches for a relative directory as passed as the first parameter along the executable or a library (local install) and in a number of system install locations (system install). In case of a system install, the relative path of the second parameter is assumed to prepend the first parameter. A library for which a run-time directory should get located can be identified through an exposed symbol.

Note that for Windows most projects deploy using local installs. Currently, we assume local installs for Windows.

```cpp
#include <cpplocate/cpplocate.h>

#include <glbinding/gl/gl.h>

const std::string assetPath = cpplocate::locatePath("data/cubescape", "share/glbinding", 
    reinterpret_cast<void *>(&gl::glCreateShader));
// assetPath now contains the path to the directory containing "data/cubescape"
```


# C Port of cpplocate: liblocate

Internally, *cpplocate* is implemented using plain C, providing a C++ interface for ease of use. For communities and software that don't want to use C++, the `liblocate` within this project can be used instead.

The adapted CMake integration is:

```cmake
find_package(liblocate QUIET) # if you want to check for existence
find_package(liblocate REQUIRED) # if it is really required in your project

# declare target

target_link_libraries(${target} ... PUBLIC cpplocate::liblocate)
```

The interfaces are as follows:

```cpp

#include <liblocate/liblocate.h>

// Get path to the current executable
void getExecutablePath(char ** path, unsigned int * pathLength);

// Get path to the current application bundle
void getBundlePath(char ** path, unsigned int * pathLength);

// Get path to the current module
void getModulePath(char ** path, unsigned int * pathLength);

// Get path to dynamic library
void getLibraryPath(void * symbol, char ** path, unsigned int * pathLength);

// Locate path to a file or directory
void locatePath(char ** path, unsigned int * pathLength, const char * relPath, unsigned int relPathLength, 
    const char * systemDir, unsigned int systemDirLength, void * symbol);
```
