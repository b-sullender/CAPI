# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.8)
project ("CAPI")

set(CMAKE_BUILD_TYPE Debug ... FORCE)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
	if (UNIX)
		set(CMAKE_C_FLAGS "-g2")
#		set(CMAKE_CXX_FLAGS_DEBUG "-g")
		message("CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
	endif (UNIX)
endif ()

set(zlib_def_file "Source/zlib-1.2.11/win32/zlib.def")

file(GLOB capi_header_files
	"Source/CAPI.h"
	"Source/image.h"
	"Source/bmp.h"
	"Source/jpg.h"
	"Source/png.h"
	"Source/ico.h"
	"Source/ieee754.h"
)

file(GLOB capi_source_files
	"Source/version.c"
	"Source/error.c"
	"Source/zpipe.c"
	"Source/memory.c"
	"Source/image.c"
	"Source/bmp.c"
	"Source/jpg.c"
	"Source/png.c"
	"Source/ico.c"
	"Source/strings.c"
	"Source/arithmetic.c"
	"Source/bitwise.c"
	"Source/compare.c"
	"Source/extend.c"
	"Source/int64.c"
	"Source/int128.c"
	"Source/hexadecimal.c"
	"Source/print.c"
	"Source/scan.c"
	"Source/ieee754.c"
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

if (WIN32)
	add_executable(TestApp "Source/testapp.h" "Source/testapp.c")
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set_target_properties(TestApp PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TestApp)
	endif ()
	add_executable(ExampleApp "Source/example.cpp")
	set_target_properties(ExampleApp PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
	target_link_libraries(ExampleApp CAPI)
endif (WIN32)

if (UNIX)
	find_package(X11 REQUIRED)
	include_directories(${X11_INCLUDE_DIR})
	add_executable(TestApp "Source/testapp.h" "Source/testapp.c")
	target_link_libraries(TestApp ${X11_LIBRARIES})
	target_link_libraries(TestApp "-lpthread")
endif (UNIX)

target_link_libraries(TestApp CAPI)

set(test_images_path STR\(\"${CMAKE_SOURCE_DIR}/Images\"\))
target_compile_definitions(TestApp PRIVATE
	_CRT_SECURE_NO_WARNINGS
	IMAGES_SOURCE_PATH=${test_images_path}
)
