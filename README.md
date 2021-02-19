# CAPI

Data and Image processing library for general C/C++ programming. This is a non-specific operating system library designed to integrate into any future operating system and their applications. To keep this library independent, required libraries are built into CAPI

This library ships with the following 3rd party library source code:  
```
jpeg-9d  
zlib-1.2.11  
```
See the [CHANGELOG](https://github.com/b-sullender/CAPI/blob/main/CHANGELOG.md) file for details about any changes to the source code.

## Key Features

**Cross-platform**

- Windows
- Linux
- macOS

**Image support**

- [BMP](https://wikipedia.org/wiki/BMP_file_format)
- [JPG](https://wikipedia.org/wiki/JPEG)
- [PNG](https://wikipedia.org/wiki/Portable_Network_Graphics)
- [ICO](https://wikipedia.org/wiki/ICO_(file_format))

**String support**

A string type that automatically handles string manipulation, conversion with data variables, and its buffer size.

Simple interface for manipulating strings of encoding types *ASCII*, *UTF8*, *UTF16* and *UTF32*. **ToString** and **FromString** functions for conversion between data variables and strings, with IEEE 754 support and customization of formatting.

LICENSE TERMS
=============
```
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  
  (1) If any part of the source code for this software is distributed, then this
      README file must be included, with this copyright and no-warranty notice
      unaltered; and any additions, deletions, or changes to the original files
      must be clearly indicated in accompanying documentation.
  (2) If only executable code is distributed, then the accompanying
      documentation must state that "this software is based in part on the work of
      the Independent JPEG Group".
  (3) Permission for use of this software is granted only if the user accepts
      full responsibility for any undesirable consequences; the authors accept
      NO LIABILITY for damages of any kind.
```

# Compiling CAPI
Use cmake to build a solution/project for your desired IDE (cmake-gui is tested and recommended)  
This project has been tested with the following:  
```
  Visual Studio 2019 (Windows 10, x86, x86-64)
  Visual Studio 2010 (Windows XP, x86, x86-64)
  Unix Makefiles (gcc/g++) (Ubuntu MATE, ARMv7-A ODROID-XU4)
  CodeBlocks - Unix Makefiles (gcc/g++, clang) (Fedora 32, x86-64)
  CodeBlocks - Unix Makefiles (gcc/g++) (Ubuntu 20.04.1, x86-64)
  Xcode (macOS Big Sur) (Graphics not tested, native graphical window will be added)
```
On Linux and macOS the included TestApp program uses X11 for creating windows with a frame buffer.  
See the [HELP](https://github.com/b-sullender/CAPI/blob/main/HELP.md) file for details about setting up a workstation & installing the needed X11 dev package.  

# Using CAPI
After building the project files, run the included TestApp program to test CAPI.  
TestApp has several commands for testing the CAPI library, see the [HELP](https://github.com/b-sullender/CAPI/blob/main/HELP.md) file for details.  
You can include CAPI.h and link to CAPI.lib (Linux, libCAPI.so) in your C/C++ projects.  
Exported CAPI functions start with **capi_** to identify them.  
Some wrapper functions (E.g. zpipe.c functions) may not use the **capi_** identifier.  
C/C++ preprocessors may also use the **capi_** identifier for system compatibility.  

# Contact & reporting problems
Simple "how do i...", "I'm having trouble", or "is this a bug" questions are best asked at [Discussions Q&A](https://github.com/b-sullender/CAPI/discussions/categories/q-a)  
Bugs, build problems, and discovered vulnerabilities that you are relatively certain is a legit problem in the code, and for which you can give clear instructions for how to reproduce, should be [reported as issues](https://github.com/b-sullender/CAPI/issues)

If confidentiality precludes a public question or issue, you may contact the project leader privately at capi.project.brian@gmail.com

# Contributing
CAPI welcomes code contributions. You can request to be a collaborator at [Contribute Request](https://github.com/b-sullender/CAPI/discussions/categories/contribute-request). We take code contributions via the usual GitHub pull request (PR) mechanism. Please see [CONTRIBUTING](https://github.com/b-sullender/CAPI/blob/main/CONTRIBUTING.md) for detailed instructions.
