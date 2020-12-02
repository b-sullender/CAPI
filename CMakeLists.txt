﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.8)
project ("CAPI")

set(zlib_def_file "Source/zlib-1.2.11/win32/zlib.def")

file(GLOB capi_header_files
	"Source/CAPI.h"
	"Source/image.h"
	"Source/bmp.h"
	"Source/jpg.h"
	"Source/png.h"
	"Source/ico.h"
)

file(GLOB capi_source_files
	"Source/version.c"
	"Source/zpipe.c"
	"Source/memory.c"
	"Source/image.c"
	"Source/bmp.c"
	"Source/jpg.c"
	"Source/png.c"
	"Source/ico.c"
)

file(GLOB zlib_header_files
	"Source/zlib-1.2.11/crc32.h"
	"Source/zlib-1.2.11/deflate.h"
	"Source/zlib-1.2.11/gzguts.h"
	"Source/zlib-1.2.11/inffast.h"
	"Source/zlib-1.2.11/inffixed.h"
	"Source/zlib-1.2.11/inflate.h"
	"Source/zlib-1.2.11/inftrees.h"
	"Source/zlib-1.2.11/trees.h"
	"Source/zlib-1.2.11/zconf.h"
	"Source/zlib-1.2.11/zlib.h"
	"Source/zlib-1.2.11/zutil.h"
)

file(GLOB zlib_source_files
	"Source/zlib-1.2.11/adler32.c"
	"Source/zlib-1.2.11/compress.c"
	"Source/zlib-1.2.11/crc32.c"
	"Source/zlib-1.2.11/deflate.c"
	"Source/zlib-1.2.11/gzclose.c"
	"Source/zlib-1.2.11/gzlib.c"
	"Source/zlib-1.2.11/gzread.c"
	"Source/zlib-1.2.11/gzwrite.c"
	"Source/zlib-1.2.11/infback.c"
	"Source/zlib-1.2.11/inffast.c"
	"Source/zlib-1.2.11/inflate.c"
	"Source/zlib-1.2.11/inftrees.c"
	"Source/zlib-1.2.11/trees.c"
	"Source/zlib-1.2.11/uncompr.c"
	"Source/zlib-1.2.11/zutil.c"
)

file(GLOB jpeg_header_files
	"Source/jconfig.h"
	"Source/jpeg-9d/jdct.h"
	"Source/jpeg-9d/jerror.h"
	"Source/jpeg-9d/jinclude.h"
	"Source/jpeg-9d/jmemsys.h"
	"Source/jpeg-9d/jmorecfg.h"
	"Source/jpeg-9d/jpegint.h"
	"Source/jpeg-9d/jpeglib.h"
	"Source/jpeg-9d/jversion.h"
)

file(GLOB jpeg_source_files
	"Source/jpeg-9d/jaricom.c"
	"Source/jpeg-9d/jcapimin.c"
	"Source/jpeg-9d/jcapistd.c"
	"Source/jpeg-9d/jcarith.c"
	"Source/jpeg-9d/jccoefct.c"
	"Source/jpeg-9d/jccolor.c"
	"Source/jpeg-9d/jcdctmgr.c"
	"Source/jpeg-9d/jchuff.c"
	"Source/jpeg-9d/jcinit.c"
	"Source/jpeg-9d/jcmainct.c"
	"Source/jpeg-9d/jcmarker.c"
	"Source/jpeg-9d/jcmaster.c"
	"Source/jpeg-9d/jcomapi.c"
	"Source/jpeg-9d/jcparam.c"
	"Source/jpeg-9d/jcprepct.c"
	"Source/jpeg-9d/jcsample.c"
	"Source/jpeg-9d/jctrans.c"
	"Source/jpeg-9d/jdapimin.c"
	"Source/jpeg-9d/jdapistd.c"
	"Source/jpeg-9d/jdarith.c"
	"Source/jpeg-9d/jdatadst.c"
	"Source/jpeg-9d/jdatasrc.c"
	"Source/jpeg-9d/jdcoefct.c"
	"Source/jpeg-9d/jdcolor.c"
	"Source/jpeg-9d/jddctmgr.c"
	"Source/jpeg-9d/jdhuff.c"
	"Source/jpeg-9d/jdinput.c"
	"Source/jpeg-9d/jdmainct.c"
	"Source/jpeg-9d/jdmarker.c"
	"Source/jpeg-9d/jdmaster.c"
	"Source/jpeg-9d/jdmerge.c"
	"Source/jpeg-9d/jdpostct.c"
	"Source/jpeg-9d/jdsample.c"
	"Source/jpeg-9d/jdtrans.c"
	"Source/jpeg-9d/jerror.c"
	"Source/jpeg-9d/jfdctflt.c"
	"Source/jpeg-9d/jfdctfst.c"
	"Source/jpeg-9d/jfdctint.c"
	"Source/jpeg-9d/jidctflt.c"
	"Source/jpeg-9d/jidctfst.c"
	"Source/jpeg-9d/jidctint.c"
	"Source/jpeg-9d/jmemmgr.c"
	"Source/jpeg-9d/jmemnobs.c"
	"Source/jpeg-9d/jquant1.c"
	"Source/jpeg-9d/jquant2.c"
	"Source/jpeg-9d/jutils.c"
)

source_group("Header Files\\zlib-1.2.11" FILES ${zlib_header_files})
source_group("Header Files\\jpeg-9d" FILES ${jpeg_header_files})
source_group("Source Files\\zlib-1.2.11" FILES ${zlib_source_files})
source_group("Source Files\\jpeg-9d" FILES ${jpeg_source_files})

add_library(CAPI SHARED
	${zlib_def_file}
	${capi_header_files} ${capi_source_files}
	${zlib_header_files} ${zlib_source_files}
	${jpeg_header_files} ${jpeg_source_files})

target_compile_definitions(CAPI PRIVATE
	CAPI_EXPORTS
	LIBJPEG_EXPORTS
	_CRT_SECURE_NO_WARNINGS
	_CRT_SECURE_NO_DEPRECATE
	_CRT_NONSTDC_NO_DEPRECATE
	FT_DEBUG_LEVEL_ERROR
	FT_DEBUG_LEVEL_TRACE
	FT2_BUILD_LIBRARY
)

set_source_files_properties(zlib.def PROPERTIES HEADER_FILE_ONLY TRUE)