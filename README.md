# C++ Locator Library

cpplocate is a small C++ library that provides tools for applications to locate themselves,
their data assets, as well as dependend modules. It is cross-platform and designed to work
on all major platforms such as Linux, Windows, and MacOS X.

Especially in the context of cross-platform applications, locating data assets belonging
to an application or library is not an easy task. Depending on the situation, data assets can
occupy different locations on a system: in a development tree, data might be located relative
to the executable in a build directory, when installed into the system, they might be located
in a global directory such as /usr/share or C:\Users\...\AppData, while for self-contained
installs they might be located relative to the executable. When projects are deployed using
software installers or archives, the final location can be controlled by the user installing
the software, therefore it cannot be known at build time. The situation gets even more complicated
when an application does not only need access to its own data assets, but depends on other modules,
such as dynamic libraries or even plugins, which bring their own data. However, a software should be
able to locate its data assets as easy as possible and still be relocatable for typical deployment
and installation routines to work as expected.

To help resolve these problems, cpplocate provides the following features:

## Get absolute path to the current executable

cpplocate offers a cross-platform implementation to obtain the location of the current executable
in most cases. This can be used by the application itself to locate its data.

## Module information files

Module information files contain meta information about a module, such as project name, version, and vendor.
They also provide the information of where to find data assets for that module based on the location
of the module information file itself, or, in some cases, using an absolute path.

[TODO] Add specification for modinfo files

## CMake integration

A CMake module is provided for creating module information files automatically, taking into account
different situations such as build time (finding modules and data in a development tree) and
install time (finding modules and data from an installed location).

[TODO] Describe all provided cmake functions

## Resolve dependend modules

Similar to a dynamic linker, cpplocate can resolve dependencies to other modules by locating
module information files. The search for modules is conducted in the following order:

1. in the directory of the current executable (not the working directory!)

2. in the directories provided by the environment variable CPPLOCATE_PATH
   - <path>/<module>-info.modinfo
   - <path>/<module>/<module>-info.modinfo

3. in standard locations:
   - C:\Program Files\<module>\<module>-info.modinfo
   - /usr/share/<module>/<module>-info.modinfo
   - /usr/local/share/<module>/<module>-info.modinfo
