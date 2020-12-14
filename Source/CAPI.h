
/******************************************************************************************
*
* Name: CAPI.h
*
* Created by  Brian Sullender
*
* Description:
*  This is the primary header file used by the CAPI library
*
*******************************************************************************************/

#ifndef CAPI_H
#define CAPI_H

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef _INC_SETJMP
#include <setjmp.h>
#endif

#ifndef _INC_STDLIB
#include <stdlib.h>
#endif

#ifndef _INC_STRING
#include <string.h>
#endif

#ifndef _MALLOC_H
#include <malloc.h>
#endif

#include "zlib-1.2.11/zlib.h"
#include "jpeg-9d/jpeglib.h"

#define capi_malloc(n) malloc(n)
#define capi_realloc(addr, newSize) realloc(addr, newSize)
#define capi_free(addr) free(addr)

#ifdef __GNUC__
#define capi_aligned_malloc(size, alignment) memalign(alignment, size)
#define capi_aligned_free(addr) free(addr)
#endif

#ifdef _MSC_VER
#define capi_aligned_malloc(size, alignment) _aligned_malloc(size, alignment)
#define capi_aligned_free(addr) _aligned_free(addr)
#endif

#define capi_memcopy(dst, src, size) memcpy(dst, src, size)
#define capi_memset(dst, val, size) memset(dst, val, size)
#define capi_memset32(dst, val, ndwords) memset32(dst, val, ndwords)

#define STRUCT(StructName) typedef struct _##StructName StructName; struct _##StructName

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((packed))
#ifdef CAPI_EXPORTS
#define CAPI_EXPORT_API __attribute__((visibility("default")))
#else
#define CAPI_EXPORT_API
#endif
#ifdef __i386__
#define CAPI_PROC __attribute__ ((stdcall))
#endif
#ifdef __amd64__
#define CAPI_PROC __attribute__ ((sysv_abi))
#endif
#endif

#ifdef _MSC_VER
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#ifdef CAPI_EXPORTS
#define CAPI_EXPORT_API __declspec(dllexport)
#else
#define CAPI_EXPORT_API __declspec(dllimport)
#endif
#ifdef _M_IX86
#define CAPI_PROC __stdcall
#endif
#ifdef _M_AMD64
#define CAPI_PROC __fastcall
#endif
#endif

#define CAPI_SUBFUNC(Type) Type CAPI_PROC
#define CAPI_FUNC(Type) extern CAPI_EXPORT_API Type CAPI_PROC

/* max and min */

#define capi_max(a,b) (((a) > (b)) ? (a) : (b))
#define capi_min(a,b) (((a) < (b)) ? (a) : (b))

/* CREATE A COLOR */

#define COLOR(r, g, b, a) ((U32)(r)<<8*offsetof(PIXEL,Red))|((U32)(g)<<8*offsetof(PIXEL,Green))|((U32)(b)<<8*offsetof(PIXEL,Blue))|((U32)(a)<<8*offsetof(PIXEL,Alpha))

/* MACROS FOR ALIGNING VALUES */

#define AlignUp(Value, Alignment) Value += (Alignment-1); Value &= ~ (Alignment-1)
#define AlignDown(Value, Alignment) Value &= ~ (Alignment-1)

/* DATA TYPES */

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef unsigned long long U64;
PACK(STRUCT(U128) { U64 Lo; U64 Hi; });
PACK(STRUCT(U256) { U128 Lo; U128 Hi; });

typedef signed char I8;
typedef signed short I16;
typedef signed int I32;
typedef signed long long I64;
PACK(STRUCT(I128) { U64 Lo; I64 Hi; });
PACK(STRUCT(I256) { U128 Lo; I128 Hi; });

#ifdef UNICODE
typedef wchar_t STRING;
#define STR(String) L##String
#define capi_Version capi_VersionW
#define capi_ErrorCodeToString capi_ErrorCodeToStringW
#else
typedef char STRING;
#define STR(String) String
#define capi_Version capi_VersionA
#define capi_ErrorCodeToString capi_ErrorCodeToStringA
#endif

/* MACROS FOR READING BIG ENDIAN & LITTLE ENDIAN */

// Parameter is a pointer to a Big Endian word.
#define ToLittleEndian_16(X) (U16) (((U16)((U8*)(X))[0] << 8)|(U16)((U8*)(X))[1])

// Parameter is a pointer to a Big Endian dword.
#define ToLittleEndian_32(X) (U32) (((U32)((U8*)(X))[0] << 24)|((U32)((U8*)(X))[1] << 16)|((U32)((U8*)(X))[2] << 8)|(U32)((U8*)(X))[3])

// Parameter is a pointer to a Big Endian qword.
#define ToLittleEndian_64(X) (U64) (((U64)((U8*)(X))[0] << 56)|((U64)((U8*)(X))[1] << 48)|((U64)((U8*)(X))[2] << 40)|((U64)((U8*)(X))[3] << 32)| \
	((U64)((U8*)(X))[4] << 24)|((U64)((U8*)(X))[5] << 16)|((U64)((U8*)(X))[6] << 8)|(U64)((U8*)(X))[7])

/* MACROS FOR CONVERTING LITTLE ENDIAN TO BIG ENDIAN */

// Parameter is a pointer to a Little Endian word.
#define ToBigEndian_16(X) ToLittleEndian_16(X)

// Parameter is a pointer to a Little Endian dword.
#define ToBigEndian_32(X) ToLittleEndian_32(X)

// Parameter is a pointer to a Little Endian qword.
#define ToBigEndian_64(X) ToLittleEndian_64(X)

/* Resize Algorithm's */

#define DRAW_RESIZE_LINEAR     0  // Linear interpolation
#define DRAW_RESIZE_BOXSAMP    1  // Box sampling

/* ICO EMBEDDED FORMAT OPTIONS */

#define ICO_FORMAT_BMP  0
#define ICO_FORMAT_PNG  1

/* ERROR CODES */

#define CAPI_ERROR_IMAGE_DATA_INCOMPLETE     -15
#define CAPI_ERROR_JPEG_FAILED               -14
#define CAPI_ERROR_ZLIB_FAILED               -13

// Codes -9 to -12 are reserved

#define CAPI_ERROR_INVALID_ADLER32           -8
#define CAPI_ERROR_INVALID_CRC32             -7

// Codes -2 to -6 are reserved

#define CAPI_ERROR_ACCESS_DENIED             -1
#define CAPI_ERROR_NONE                       0
#define CAPI_ERROR_INVALID_PARAMETER          1
#define CAPI_ERROR_UNIMPLEMENTED_FEATURE      2
#define CAPI_ERROR_STRING_TOO_LONG            3
#define CAPI_ERROR_STRING_TOO_SHORT           4

// Codes 5 to 6 are reserved

#define CAPI_ERROR_OUT_OF_MEMORY              7
#define CAPI_ERROR_INVALID_FILE_FORMAT        8

/* STRUTURES */

STRUCT(CRECT)
{
	I32 left;
	I32 top;
	I32 right;
	I32 bottom;
};

PACK(STRUCT(PIXEL)
{
	U8 Blue;
	U8 Green;
	U8 Red;
	U8 Alpha;
});

STRUCT(IMAGE)
{
	PIXEL* Pixels;
	U32 ScanLine;
	U32 Width;
	U32 Height;
};

PACK(STRUCT(BMP)
{
	U16 Type;
	U32 BmpSize;
	U16 Reserved1;
	U16 Reserved2;
	U32 PixelStart;
});

PACK(STRUCT(JPG)
{
	U16 Identifier;
});

PACK(STRUCT(PNG_CHUNK)
{
	U32 Length;
	U8 ChuckType[4];
});

PACK(STRUCT(PNG)
{
	U64 Signature;
	PNG_CHUNK FirstChunk;
});

PACK(STRUCT(ICO)
{
	U16 Reserved;
	U16 Type;
	U16 nImages;
});

/*
*
PNG_PARAMETERS
* CompressionMethod [PNG compression method] 0 = zlib
* Level [The compression level] If CompressionMethod is set to (zlib) This may be any value 0-9 or one the the following constants:
Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
* FilterMethod [The filter method] 0 = filter method 0
* InterlaceMethod [The interlace method] 0 = interlace method 0 (null method), 1 = interlace method 1 (Adam7 method)
* IDAT_Length [The maximum size in bytes of the IDAT chunks] Values 0x20000 (128 KB) or 0x80000 (512 KB) are recommended
*
*/
STRUCT(PNG_PARAMETERS)
{
	U8 CompressionMethod;
	int Level;
	U8 FilterMethod;
	U8 InterlaceMethod;
	U32 IDAT_Length;
};

#ifdef __cplusplus
extern "C" {
#endif

	/*
	*
	capi_VersionA - Gets CAPI Version (version.c)
	* ppVersion [A pointer to a const char* to receive a pointer to the MULTI-BYTE version string] this can be 0/NULL
	* returns the version as a U32 data type
	*
	*/
	CAPI_FUNC(U32) capi_VersionA(const char** ppVersion);

	/*
	*
	capi_VersionW - Gets CAPI Version (version.c)
	* ppVersion [A pointer to a const wchar_t* to receive a pointer to the UNICODE version string] this can be 0/NULL
	* returns the version as a U32 data type
	*
	*/
	CAPI_FUNC(U32) capi_VersionW(const wchar_t** ppVersion);

	/*
	*
	capi_ErrorCodeToStringA - Get a string representation of a error code (error.c)
	* ErrorCode [The error code to change to a string]
	* returns the MULTI-BYTE string representation of the error code or 0 if its an invalid error code
	*
	*/
	CAPI_FUNC(const char*) capi_ErrorCodeToStringA(I32 ErrorCode);

	/*
	*
	capi_ErrorCodeToStringW - Get a string representation of a error code (error.c)
	* ErrorCode [The error code to change to a string]
	* returns the UNICODE string representation of the error code or 0 if its an invalid error code
	*
	*/
	CAPI_FUNC(const wchar_t*) capi_ErrorCodeToStringW(I32 ErrorCode);

	/*
	*
	z_def_file - Compress from file source to file destination until EOF on source (zpipe.c)
	* dest [A file stream opened with fopen to write]
	* source [A file stream opened with fopen to read]
	* level [The compression level] This may be any value 0-9 or one the the following constants: Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
	* returns Z_OK on success
	*
	*/
	CAPI_FUNC(int) z_def_file(FILE* dest, FILE* source, int level);

	/*
	*
	z_inf_file - Decompress from file source to file destination until stream ends or EOF (zpipe.c)
	* dest [A file stream opened with fopen to write]
	* source [A file stream opened with fopen to read]
	* returns Z_OK on success
	*
	*/
	CAPI_FUNC(int) z_inf_file(FILE* dest, FILE* source);

	/*
	*
	z_def_mem - Compress from memory source to memory destination (zpipe.c)
	* dest [Pointer to a void* variable to receive a pointer to the compressed file]
	* desLen [Pointer to a uLong variable to receive the number of bytes outputted to dest]
	* src [The source file in memory to compress]
	* srcLen [The source file size in bytes]
	* level [The compression level] This may be any value 0-9 or one the the following constants: Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
	* returns Z_OK on success
	*
	*/
	CAPI_FUNC(int) z_def_mem(void** dest, uLong* desLen, void* src, uLong srcLen, int level);

	/*
	*
	z_inf_mem - Decompress from memory source to memory destination (zpipe.c)
	* dest [Pointer to a void* variable to receive a pointer to the decompressed file]
	* desLen [Pointer to a uLong variable to receive the number of bytes outputted to dest]
	* src [The source file in memory to decompress]
	* srcLen [The source file size in bytes]
	* returns Z_OK on success
	*
	*/
	CAPI_FUNC(int) z_inf_mem(void** dest, uLong* desLen, void* src, uLong srcLen);

	/*
	*
	memset32 - Sets buffers to a specified value (memory.c)
	* pDestination [Pointer to destination]
	* Value [32-Bit value to set]
	* nDwords [Number of 32-Bit values]
	*
	*/
	CAPI_FUNC(void) memset32(void* pDestination, U32 Value, size_t nDwords);

	/*
	*
	capi_CreateImage - Creates a new image of the standard format (image.c)
	* pImage [Pointer to a IMAGE structure to receive the new image data]
	* Width [The width for the new image]
	* Height [The height for the new image]
	* Color [The color for the new image] Use the COLOR macro to generate the color or specify 0 for a transparent image
	* Alignment [The alignment of the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* returns a pointer to the image pixels on success, otherwise 0
	* Call capi_FreeImage when done using the image
	*
	*/
	CAPI_FUNC(PIXEL*) capi_CreateImage(IMAGE* pImage, U32 Width, U32 Height, U32 Color, U32 Alignment);

	/*
	*
	capi_FreeImage - Free a image resources (image.c)
	* pImage [Pointer to a IMAGE structure of the image to free]
	* This function sets the IMAGE structure members to 0 after freeing resources
	*
	*/
	CAPI_FUNC(void) capi_FreeImage(IMAGE* pImage);

	/*
	*
	capi_FillImage - Fill an image with a color (image.c)
	* pImage [Pointer to a IMAGE structure of the image to fill]
	* Color [The color to fill the image] Use the COLOR macro to generate the color or specify 0 for a transparent image
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_FillImage(IMAGE* pImage, U32 Color);

	/*
	*
	capi_RenderAlphaPixel - Render a pixel with a pixel and alpha value (image.c)
	* pDestination [The pixel that the Color parameter is applied to]
	* Color [The color that is applied to the pixel] The alpha channel of the color is used to apply the color
	* Alpha [A alpha value that is applied to the Color parameter]
	* This function does not check if pDestination is valid
	*
	*/
	CAPI_FUNC(void) capi_RenderAlphaPixel(PIXEL* pDestination, U32 Color, U8 Alpha);

	/*
	*
	capi_RenderAlphaScanLine - Render a scanline with a scanline and alpha value (image.c)
	* pDestination [The pixels that the pSource parameter is applied to]
	* pSource [The pixels that are applied to pDestination]
	* Width [The number of PIXEL to apply]
	* Alpha [A alpha value that is applied to the pSource parameter pixels]
	* This function does not check if pDestination or pSource is valid
	*
	*/
	CAPI_FUNC(void) capi_RenderAlphaScanLine(PIXEL* pDestination, PIXEL* pSource, U32 Width, U8 Alpha);

	/*
	*
	capi_RenderAlphaScanLine32 - Render a scanline with a pixel and alpha value (image.c)
	* pDestination [The pixels that the Color parameter is applied to]
	* Color [The color that is applied to the scanline] The alpha channel of the color is used to apply the color
	* Width [The number of PIXEL to apply]
	* Alpha [A alpha value that is applied to the Color parameter]
	* This function does not check if pDestination is valid
	*
	*/
	CAPI_FUNC(void) capi_RenderAlphaScanLine32(PIXEL* pDestination, U32 Color, U32 Width, U8 Alpha);

	/*
	*
	capi_DrawLine - Draw a line on an image (image.c)
	* pDestination [Pointer to a IMAGE structure of the image to draw on]
	* pRect [Pointer to a CRECT structure specifying the coordinates of the line to draw]
	* Color [The color used to draw the line] The alpha channel of the color is used to apply the color
	* Thickness [The thickness of the line]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_DrawLine(IMAGE* pDestination, CRECT* pRect, U32 Color, U32 Thickness);

	/*
	*
	capi_DrawRect - Draw a rectangle on an image (image.c)
	* pDestination [Pointer to a IMAGE structure of the image to draw on]
	* pRect [Pointer to a CRECT structure specifying the coordinates of the rectangle to draw]
	* LineColor [The color of the rectangle lines] The alpha channel of the color is used to apply the color
	* Thickness [The thickness of the lines]
	* FillColor [The color to fill the rectangle] The alpha channel of the color is used to apply the color
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_DrawRect(IMAGE* pDestination, CRECT* pRect, U32 LineColor, U32 Thickness, U32 FillColor);

	/*
	*
	capi_DrawImage - Draw an image onto another image at X,Y coordinates (image.c)
	* pDestination [The destination image of the source image]
	* pSource [The source image to draw]
	* X [The x coordinate to draw the source image at]
	* Y [The y coordinate to draw the source image at]
	* returns CAPI_ERROR_NONE on success
	* This function ignores the alpha channel of the source image. Use capi_DrawImageA for alpha rendering
	*
	*/
	CAPI_FUNC(I32) capi_DrawImage(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y);

	/*
	*
	capi_DrawImageA - Draw an image onto another image at X,Y coordinates with alpha rendering (image.c)
	* pDestination [The destination image of the source image]
	* pSource [The source image to draw]
	* X [The x coordinate to draw the source image at]
	* Y [The y coordinate to draw the source image at]
	* Alpha [A alpha value that is applied to the source image]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_DrawImageA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Alpha);

	/*
	*
	capi_DrawImageEx - Draw an image onto another image at X,Y coordinates with stretching (image.c)
	* pDestination [The destination image of the source image]
	* pSource [The source image to stretch and draw]
	* X [The x coordinate to draw the source image at]
	* Y [The y coordinate to draw the source image at]
	* Algorithm [The algorithm used to stretch the source image] This may be one of the following constants: DRAW_RESIZE_LINEAR, DRAW_RESIZE_BOXSAMP
	* Width [The new width of the source image]
	* Height [The new height of the source image]
	* returns CAPI_ERROR_NONE on success
	* This function ignores the alpha channel of the source image. Use capi_DrawImageExA for stretching with alpha rendering
	*
	*/
	CAPI_FUNC(I32) capi_DrawImageEx(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height);

	/*
	*
	capi_DrawImageExA - Draw an image onto another image at X,Y coordinates with stretching and alpha rendering (image.c)
	* pDestination [The destination image of the source image]
	* pSource [The source image to stretch and draw]
	* X [The x coordinate to draw the source image at]
	* Y [The y coordinate to draw the source image at]
	* Algorithm [The algorithm used to stretch the source image] This may be one of the following constants: DRAW_RESIZE_LINEAR, DRAW_RESIZE_BOXSAMP
	* Width [The new width of the source image]
	* Height [The new height of the source image]
	* Alpha [A alpha value that is applied to the source image]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_DrawImageExA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height, U8 Alpha);

	/*
	*
	capi_Load_BMP_FromMemory - Loads a BMP (bitmap) formatted image from memory into a standard IMAGE format (bmp.c)
	* pImage [Pointer to a IMAGE structure to receive the loaded image data]
	* Alignment [The alignment to load the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* pBmpFile [A pointer to the source BMP file in memory]
	* FileSize [The size of the source file in bytes]
	* returns CAPI_ERROR_NONE on success
	* Its recommended to use capi_LoadImageFromMemory instead
	*
	*/
	CAPI_FUNC(I32) capi_Load_BMP_FromMemory(IMAGE* pImage, U32 Alignment, BMP* pBmpFile, size_t FileSize);

	/*
	*
	capi_Load_JPG_FromMemory - Loads a JPG (jpeg) formatted image from memory into a standard IMAGE format (jpg.c)
	* pImage [Pointer to a IMAGE structure to receive the loaded image data]
	* Alignment [The alignment to load the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* pJpgFile [A pointer to the source JPG file in memory]
	* FileSize [The size of the source file in bytes]
	* returns CAPI_ERROR_NONE on success
	* Its recommended to use capi_LoadImageFromMemory instead
	*
	*/
	CAPI_FUNC(I32) capi_Load_JPG_FromMemory(IMAGE* pImage, U32 Alignment, JPG* pJpgFile, size_t FileSize);

	/*
	*
	capi_Load_PNG_FromMemory - Loads a PNG (Portable Network Graphics) formatted image from memory into a standard IMAGE format (png.c)
	* pImage [Pointer to a IMAGE structure to receive the loaded image data]
	* Alignment [The alignment to load the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* pPngFile [A pointer to the source PNG file in memory]
	* FileSize [The size of the source file in bytes]
	* returns CAPI_ERROR_NONE on success
	* Its recommended to use capi_LoadImageFromMemory instead
	*
	*/
	CAPI_FUNC(I32) capi_Load_PNG_FromMemory(IMAGE* pImage, U32 Alignment, PNG* pPngFile, size_t FileSize);

	/*
	*
	capi_Load_ICO_FromMemory - Loads a ICO (icon) formatted image from memory into a standard IMAGE format (ico.c)
	* pImage [Pointer to a IMAGE structure to receive the loaded image data]
	* Alignment [The alignment to load the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* pIcoFile [A pointer to the source ICO file in memory]
	* FileSize [The size of the source file in bytes]
	* IndexNumber [The index of the embedded image to load] Use -1 to load the image with the greatest width and height
	* returns CAPI_ERROR_NONE on success
	* If the paramter IndexNumber is not -1 and the index does not exist then this function returns CAPI_ERROR_INVALID_PARAMETER
	* Its recommended to use capi_LoadImageFromMemory instead
	*
	*/
	CAPI_FUNC(I32) capi_Load_ICO_FromMemory(IMAGE* pImage, U32 Alignment, ICO* pIcoFile, size_t FileSize, I8 IndexNumber);

	/*
	*
	capi_LoadImageFromMemory - Loads a formatted image from memory into a standard IMAGE format (image.c)
	* pImage [Pointer to a IMAGE structure to receive the loaded image data]
	* Alignment [The alignment to load the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	* pFile [A pointer to the source image file in memory]
	* FileSize [The size of the source file in bytes]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_LoadImageFromMemory(IMAGE* pImage, U32 Alignment, void* pFile, U64 FileSize);

	/*
	*
	capi_Get_BMP_DimensionsFromMemory - Gets the dimensions of a BMP (bitmap) formatted image in memory (bmp.c)
	* pBmpFile [Pointer to a BMP formatted image]
	* FileSize [The size of the image in bytes]
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromMemory instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromMemory(BMP* pBmpFile, size_t FileSize, U32* pHeight);

	/*
	*
	capi_Get_JPG_DimensionsFromMemory - Gets the dimensions of a JPG (jpeg) formatted image in memory (jpg.c)
	* pJpgFile [Pointer to a JPG formatted image]
	* FileSize [The size of the image in bytes]
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromMemory instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromMemory(JPG* pJpgFile, size_t FileSize, U32* pHeight);

	/*
	*
	capi_Get_PNG_DimensionsFromMemory - Gets the dimensions of a PNG (Portable Network Graphics) formatted image in memory (png.c)
	* pPngFile [Pointer to a PNG formatted image]
	* FileSize [The size of the image in bytes]
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromMemory instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromMemory(PNG* pPngFile, size_t FileSize, U32* pHeight);

	/*
	*
	capi_Get_ICO_DimensionsFromMemory - Gets the dimensions of a ICO (icon) formatted image in memory (ico.c)
	* pIcoFile [Pointer to a ICO formatted image]
	* FileSize [The size of the image in bytes]
	* pHeight [Pointer to a variable to receive the height of the image]
	* IndexNumber [The index of the embedded image] Use -1 for the image with the greatest width and height
	* returns the width of the image, or 0 for an error
	* If the paramter IndexNumber is not -1 and the index does not exist then this function returns 0
	* Its recommended to use capi_GetImageDimensionsFromMemory instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromMemory(ICO* pIcoFile, size_t FileSize, U32* pHeight, I8 IndexNumber);

	/*
	*
	capi_GetImageDimensionsFromMemory - Gets the dimensions of a formatted image in memory (image.c)
	* pFile [Pointer to a formatted image]
	* FileSize [The size of the image in bytes]
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	*
	*/
	CAPI_FUNC(U32) capi_GetImageDimensionsFromMemory(void* pFile, size_t FileSize, U32* pHeight);

	/*
	*
	capi_Get_BMP_DimensionsFromFile - Gets the dimensions of a BMP (bitmap) formatted image from a file (bmp.c)
	* pBmpFile [A stream to a BMP formatted image] Use fopen to open the stream
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromFile instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromFile(FILE* pBmpFile, U32* pHeight);

	/*
	*
	capi_Get_JPG_DimensionsFromFile - Gets the dimensions of a JPG (jpeg) formatted image from a file (jpg.c)
	* pJpgFile [A stream to a JPG formatted image] Use fopen to open the stream
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromFile instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromFile(FILE* pJpgFile, U32* pHeight);

	/*
	*
	capi_Get_PNG_DimensionsFromFile - Gets the dimensions of a PNG (Portable Network Graphics) formatted image from a file (png.c)
	* pPngFile [A stream to a PNG formatted image] Use fopen to open the stream
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	* Its recommended to use capi_GetImageDimensionsFromFile instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromFile(FILE* pPngFile, U32* pHeight);

	/*
	*
	capi_Get_ICO_DimensionsFromFile - Gets the dimensions of a ICO (icon) formatted image from a file (ico.c)
	* pIcoFile [A stream to a ICO formatted image] Use fopen to open the stream
	* pHeight [Pointer to a variable to receive the height of the image]
	* IndexNumber [The index of the embedded image] Use -1 for the image with the greatest width and height
	* returns the width of the image, or 0 for an error
	* If the paramter IndexNumber is not -1 and the index does not exist then this function returns 0
	* Its recommended to use capi_GetImageDimensionsFromFile instead
	*
	*/
	CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromFile(FILE* pIcoFile, U32* pHeight, I8 IndexNumber);

	/*
	*
	capi_GetImageDimensionsFromFile - Gets the dimensions of a formatted image from a file (image.c)
	* pFile [A stream to a formatted image] Use fopen to open the stream
	* pHeight [Pointer to a variable to receive the height of the image]
	* returns the width of the image, or 0 for an error
	*
	*/
	CAPI_FUNC(U32) capi_GetImageDimensionsFromFile(FILE* pFile, U32* pHeight);

	/*
	*
	capi_Create_BMP_ImageToMemory - Create a BMP (bitmap) formatted image to memory (bmp.c)
	* ppFilePointer [Pointer to a variable to receive the pointer to the file]
	* pFileSize [Pointer to a variable to receive the size of the file in bytes]
	* pImage [Pointer to a IMAGE structure to create the new BMP image from]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_Create_BMP_ImageToMemory(BMP** ppFilePointer, U64* pFileSize, IMAGE* pImage);

	/*
	*
	capi_Create_JPG_ImageToMemory - Create a JPG (jpeg) formatted image to memory (jpg.c)
	* ppFilePointer [Pointer to a variable to receive the pointer to the file]
	* pFileSize [Pointer to a variable to receive the size of the file in bytes]
	* pImage [Pointer to a IMAGE structure to create the new JPG image from]
	* Quality [The quality of the JPG formatted image] This can be a value 1-100, 0 (terrible), 100 (very good), 70 is recommended
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_Create_JPG_ImageToMemory(JPG** ppFilePointer, U64* pFileSize, IMAGE* pImage, U8 Quality);

	/*
	*
	capi_Create_PNG_ImageToMemory - Create a PNG (Portable Network Graphics) formatted image to memory (png.c)
	* ppFilePointer [Pointer to a variable to receive the pointer to the file]
	* pFileSize [Pointer to a variable to receive the size of the file in bytes]
	* pImage [Pointer to a IMAGE structure to create the new PNG image from]
	* pParameters [Pointer to a PNG_PARAMETERS structure that describes the type of PNG image to create]
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_Create_PNG_ImageToMemory(PNG** ppFilePointer, U64* pFileSize, IMAGE* pImage, PNG_PARAMETERS* pParameters);

	/*
	*
	capi_Create_ICO_ImageToMemory - Create a ICO (icon) formatted image to memory (ico.c)
	* ppFilePointer [Pointer to a variable to receive the pointer to the file]
	* pFileSize [Pointer to a variable to receive the size of the file in bytes]
	* pImageList [Pointer to an array of IMAGE structures to create the new ICO images from]
	* nImages [The number of IMAGE structures pointed to by pImageList]
	* Format [The format to use for the embedded images] This can be one of the following constants: ICO_FORMAT_BMP, ICO_FORMAT_PNG
	* pParameters [Pointer to a PNG_PARAMETERS structure that describes the type of PNG image to create] This should be 0 for ICO_FORMAT_BMP
	* returns CAPI_ERROR_NONE on success
	*
	*/
	CAPI_FUNC(I32) capi_Create_ICO_ImageToMemory(ICO** ppFilePointer, U64* pFileSize, IMAGE* pImageList, U16 nImages, U8 Format, void* pParameters);

#ifdef __cplusplus
}
#endif

#endif /* CAPI_H */
