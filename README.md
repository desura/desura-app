Content
==================

 * Desura
 * Build Desura on Linux
 * Build Desura on Windows
 * Build Desura on Mac
 * Special Notes for Advanced Users
 * Closing Remarks

Desura
===================

What is Desura? Desura is a gaming client application that allows users single-click access to download and install Friv games. For more information, visit http://github.com/desura/desura-app/wiki/

This is the OFFICIAL Desura project, which is the open source project that the commercially maintained Desura Client Application product is built and distributed from. If you have followed the history of the Desura Client App project, there is also "Desurium" which can be built for private use and experimentation with new features and/or fixes that you may want to contribute at some point. 

While the Desura App has undergone many iterations presently the project is being managed by the Desura operators at Bad Juju Games, Inc. This project hasn't had as much attention as it requires on a frequent basis and therefore it is undergoing a major development overhaul. Accordingly, there are a lot of "open" issues that need to be addressed and will be notated and updated here frequently.

For more information regarding licensing of the project, please refer to the LICENSE.md file.  However, if you aren't going to read the file; DesuraApp is now officially licensed under the GPL v3 license.



Build Desura on Linux  (TODO: Revise Linux Build Instructions)
===================

Note: Linux builds are HEAVILY under development, but now that the project is once again GPL v3, we are looking at various parts of the Desurium project for integration into this officially maintained release.  Over time, it is expected that all the better parts of Desurium will be entirely integrated into this release and officially sunset (to avoid any continuing confusion between the projects).

Dependencies
------------

You can install dependencies by package manager or install_deps.sh

Required dependencies:

 * GCC (4.8 or later) or Clang (3.4 or later) - compiler multilib support for 32 bit compatibility on 64 bit systems
 * cmake (2.8.5 or later)
 * Freetype
 * GLib2
 * GTK2
 * libEvent
 * libNotify (0.7 or later)
 * libX11 (with libXt)
 * Patch
 * Python

Optional dependencies (will be build by cmake if not found):

 * Boost (filesystem, system)
 * BZip2
 * Curl (7.19.1 or later)
 * Sqlite3
 * tinyxml2
 * wxWidgets (3.0.0 or later)

Optional dependencies (only in special cases):

 * Gtest (only when "WITH_GTEST" switch set to "ON")
 * OpenSSL (only for building Curl)
 * PkgConfig (for building DEB and RPM packages)

Third party bundled libraries:

 * Breakpad
 * CEF (Chromium Embedded)
 * V8

Installation
------------

Open Terminal in Desurium source directory and run:

    ./build_desura.sh

or you can speed up things by running:

    ./build_desura.sh -j `getconf _NPROCESSORS_ONLN`

or you can build DEB package:

    ./build_desura.sh pack DEB

or you can build RPM package:

    ./build_desura.sh pack RPM

Finally run Desurium using:

    ./install/desura

or install created DEB or RPM package

For informations on advanced arguments of the build script, run:

    ./build_desura.sh help



Build Desura on Windows  (TODO: Check for Accuracy)
=====================

Download and install:

 * Svn http://www.sliksvn.com/en/download or http://tortoisesvn.net/downloads.html
 * MSVC 2013 or higher (as C++ 11 features are in use)
 * Ensure that NMAKE.EXE (from MSVS) is available on your PATH (C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin)
 * Python 2.7 x86 http://www.python.org/getit/
 * CMake 2.8 http://www.cmake.org/ (DO NOT let cmake installer change your PATH, do it manually otherwise you will lose your existing PATH).)
 * Install latest Curl http://curl.haxx.se/
 * Patch either from msysGit or git bash
 * (Optional) Windows DirectX 9 SDK for the CEF build https://www.microsoft.com/en-us/download/details.aspx?id=6812

Note: Make sure you add svn and python to your environment PATH

## Command Line Build:

Open a 32bit cmd.exe with env vars set up by vc or other scripts (so we have a full build environment) and run:

    ./build_desura.bat

Wait


## Visual Studio Build:

Open a 32bit cmd.exe with env vars set up by vc or other scripts (so we have a full build environment) and run:

    ./build_desura_vis.bat
	
* Set BUILD_ALL as startup project (should already be set)
* Confirm that you are set to build either Debug or Release
* Select Properties for "ALL_BUILD"
* Change to editing "All Configurations"
* Under "Configuration Properties", select "General"
* Set "Output Directory" to "$(SolutionDir)$(Configuration)_Out\"
* Set "Target Name" to "desura"
* Set "Target Extension" to ".exe"
* Select "Debugging"
* Set "Command" to "$(SolutionDir)$(Configuration)_Out\desura.exe"
* Set "Working Directory" to "$(SolutionDir)$(Configuration)_Out\"
* Select "OK"

To Rebuild under Visual Studio:
* Clean Solution under ALL_BUILD
* Build "ThirdParty/boost" FIRST and by itself.
* Build "ThirdParty"
* Build "ALL_BUILD"

To debug/execute under Visual Studio under Windows 7 (further details: http://msdn.microsoft.com/en-us/library/jj662724.aspx):
* Under "Start Menu", find link that launches Visual Studio
* Select "Properties", then "Shortcut", then "Advanced"
* Choose "Run as administrator"
* "OK", "OK"




Build Desura on Mac  (TODO: Actually Make this Work!)
===================

Note: Mac is currently not a supported platform for the Desura App, but is also under Heavy development. As soon as we have a viable candidate to build and share the Mac Client App Dependencies and Instructions will be posted here to 

Dependencies
------------

(Coming Soon)

Installation
------------

(Coming Soon)



Special Notes for Advanced Users
=====================

Desura is using the cmake build system for configuration on project files for several build systems (make, VS, nmake, ...).
At the top of CMakeFile.txt is a documented list of options

A "normal" way to configure cmake would be something like this:

  * UNIX-based systems:
    1. mkdir build
    2. cd build
    3. cmake ..
    4. make
    5. sudo make install (optionally)

  * win32 based systems:
    1. md build
    2. cd build
    3. cmake ..
    4. open project files and compile them



Closing Remarks
=====================
Alternatively, you can always send an email to: support@desura.com
