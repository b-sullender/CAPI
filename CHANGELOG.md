
# Changelog for 3rd party source code
  
- jpeg-9d  
  - jmorecfg.h - **export jpeg-9d functions**, see Source/jconfig.h for LIBJPEG_EXPORT_API
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
  - gzguts.h - **fix for "implicit function declaration" errors for system IO functions**
```diff
@@ -3,6 +3,10 @@
  * For conditions of distribution and use, see copyright notice in zlib.h
  */

+#ifdef __GNUC__
+#include <unistd.h>
+#endif
+
 #ifdef _LARGEFILE64_SOURCE
 #  ifndef _LARGEFILE_SOURCE
 #    define _LARGEFILE_SOURCE 1
```
- zlib-1.2.11
  - gzread.c - **compiler warning fix**
```diff
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
  - gzwrite.c - **compiler warning fix**
```diff
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
