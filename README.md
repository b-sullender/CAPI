# CAPI

Data and Image processing library for general C/C++ programming. This is a non-specific operating system library designed to integrate into any future operating system and their applications. To keep this library independent, required libraries are built into CAPI

This library ships with the following 3rd party library source code:  
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
  Visual Studio 2019 (Windows 10, x86, x86-64)
  Visual Studio 2010 (Windows XP, x86, x86-64)
  CodeBlocks - Unix Makefiles (Fedora 32, x86-64)
  CodeBlocks - Unix Makefiles (Ubuntu 20.04.1, x86-64)
```
On Linux the included TestApp program uses X11 for creating windows with a frame buffer.  
See the [HELP](https://github.com/b-sullender/CAPI/blob/main/HELP.md) file for details about installing the needed X11 dev package.  


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

# Changelog for 3rd party source code:
  
- jpeg-9d  
  - jmorecfg.h **This lets us export jpeg-9d functions for dynamic libraries**, the declaration for LIBJPEG_EXPORT_API is in Source/jconfig.h
```diff
@@ -245,7 +245,7 @@ typedef unsigned int JDIMENSION;
 /* a function referenced thru EXTERNs: */
 #define GLOBAL(type)		type
 /* a reference to a GLOBAL function: */
-#define EXTERN(type)		extern type
+#define EXTERN(type)		extern LIBJPEG_EXPORT_API type


 /* This macro is used to declare a "method", that is, a function pointer.
```
- zlib-1.2.11
  - gzguts.h
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
- zlib-1.2.11
  - gzread.c
```
@@ -316,7 +316,7 @@ local z_size_t gz_read(state, buf, len)
         /* set n to the maximum amount of len that fits in an unsigned int */
         n = -1;
         if (n > len)
-            n = len;
+            n = (unsigned int) len;

         /* first just try copying data from the output buffer */
         if (state->x.have) {
@@ -397,7 +397,7 @@ int ZEXPORT gzread(file, buf, len)
     }

     /* read len or fewer bytes to buf */
-    len = gz_read(state, buf, len);
+    len = (unsigned int) gz_read(state, buf, len);

     /* check for an error */
     if (len == 0 && state->err != Z_OK && state->err != Z_BUF_ERROR)
@@ -469,7 +469,7 @@ int ZEXPORT gzgetc(file)
     }

     /* nothing there -- try gz_read() */
-    ret = gz_read(state, buf, 1);
+    ret = (int) gz_read(state, buf, 1);
     return ret < 1 ? -1 : buf[0];
 }

```
- zlib-1.2.11
  - gzwrite.c
```
@@ -209,7 +209,7 @@ local z_size_t gz_write(state, buf, len)
                               state->in);
             copy = state->size - have;
             if (copy > len)
-                copy = len;
+                copy = (unsigned int) len;
             memcpy(state->in + have, buf, copy);
             state->strm.avail_in += copy;
             state->x.pos += copy;
@@ -229,7 +229,7 @@ local z_size_t gz_write(state, buf, len)
         do {
             unsigned n = (unsigned)-1;
             if (n > len)
-                n = len;
+                n = (unsigned int) len;
             state->strm.avail_in = n;
             state->x.pos += n;
             if (gz_comp(state, Z_NO_FLUSH) == -1)
@@ -368,7 +368,7 @@ int ZEXPORT gzputs(file, str)

     /* write string */
     len = strlen(str);
-    ret = gz_write(state, str, len);
+    ret = (int) gz_write(state, str, len);
     return ret == 0 && len != 0 ? -1 : ret;
 }

```
