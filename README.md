# CAPI

Data and Image processing library

This source code package ships with the following 3rd party source code:  
```
jpeg-9d  
zlib-1.2.11  
```

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
  Visual Studio 2019 (Windows 10, x86 and x64)
  Visual Studio 2010 (Windows XP, x86 and x64)
  CodeBlocks - Unix Makefiles (Fedora 32)
```
On Linux the included TestApp program uses X11 for creating windows with a frame buffer.  
See the HELP.txt file for details about installing the needed X11 dev package.  


# Using CAPI
After building the project files, run the included TestApp program to test CAPI.  
TestApp has several commands for testing the CAPI library (see HELP.txt for details).  
You can include CAPI.h and link to CAPI.lib (Linux, libCAPI.so) in your C/C++ projects.  
Exported CAPI functions start with **capi_** to identify them.  
Some wrapper functions (E.g. zpipe.c functions) may not use the **capi_** identifier.  
C/C++ preprocessors may also use the **capi_** identifier for system compatibility.  

# Changelog for 3rd party source code:
  
jpeg-9d  
```
  changelog:
    change line # 248 of jmorecfg.h
      -#define EXTERN(type)		extern type
      +#define EXTERN(type)		extern LIBJPEG_EXPORT_API type
  notes:
    jpeg does not export its functions for dynamic libraries without a little modification to jmorecfg.h -
    the declaration for LIBJPEG_EXPORT_API is in Source/jconfig.h
```
zlib-1.2.11
```
  changelog:
    add to line # 5 of gzguts.h
      +#ifdef __GNUC__
      +#include <unistd.h>
      +#endif
  notes:
    Including unistd.h in gzguts.h for the gcc compiler prevents "implicit function declaration" -
    warnings/errors for system IO functions like lopen or lseek.
```
  
