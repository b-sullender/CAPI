/* jconfig.h provided by the CAPI project */
/* see ReadMe.txt for more info about included librarys and sources */

/* We add LIBJPEG_EXPORT_API to export or import the jpeg functions */

#ifdef __GNUC__
#ifdef LIBJPEG_EXPORTS
#define LIBJPEG_EXPORT_API __attribute__((visibility("default")))
#else
#define LIBJPEG_EXPORT_API
#endif
#endif

#ifdef _MSC_VER
#ifdef LIBJPEG_EXPORTS
#define LIBJPEG_EXPORT_API __declspec(dllexport)
#else
#define LIBJPEG_EXPORT_API __declspec(dllimport)
#endif
#endif

/*
#if defined(_WIN32)
#  if defined(LIBJPEG_STATIC)
#    define LIBJPEG_EXPORT_API
#  else
#    if defined(LIBJPEG_EXPORTS)
#      define LIBJPEG_EXPORT_API __declspec(dllexport)
#    else
#      define LIBJPEG_EXPORT_API __declspec(dllimport)
#    endif
#  endif
#else
#  define LIBJPEG_EXPORT_API
#endif
*/

/* Our compiler supports function prototypes */
#define HAVE_PROTOTYPES

/* Our compiler supports the declaration "unsigned char" and "unsigned short" */
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT

/* We expect our compiler knows about type void */
/* #define void char */

/* We expect our compiler knows the "const" keyword */
/* #define const */

/* We expect signed chars */
#undef CHAR_IS_UNSIGNED

/* Our system has ANSI-conforming <stddef.h> and <stdlib.h> files */
#define HAVE_STDDEF_H
#define HAVE_STDLIB_H

/* Our system does have an ANSI/SysV <string.h> */
#undef NEED_BSD_STRINGS

/* typedef size_t is defined in one of the ANSI-standard places (stddef.h, stdlib.h, or stdio.h) */
#undef NEED_SYS_TYPES_H

/* We are using 386 or newer machines with 32-bit flat memory model or 64-bit AMD memory model */
#undef NEED_FAR_POINTERS

/* The linker does not need global names to be unique in less than the first 15 characters. */
#undef NEED_SHORT_EXTERNAL_NAMES

/* Only define this if you actually get "missing structure definition" warnings or errors while compiling the JPEG code. */
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char, not enum, on Windows systems. */
#ifdef _WIN32
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#ifndef FALSE			/* in case these macros already exist */
#define FALSE	0		/* values of boolean */
#endif
#ifndef TRUE
#define TRUE	1
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */
#endif

#ifdef JPEG_INTERNALS

/* We expect our compiler implements ">>" as a signed (arithmetic) shift (for signed values), which is the normal and rational definition. */
#undef RIGHT_SHIFT_IS_UNSIGNED


#endif /* JPEG_INTERNALS */
