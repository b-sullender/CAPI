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
  