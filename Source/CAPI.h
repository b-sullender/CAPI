
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

#ifndef BIT
#define BIT(n) (1LL << n)
#endif

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

#define STRUCT(StructName) typedef struct _##StructName StructName; struct _##StructName

#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((packed))
#ifdef CAPI_EXPORTS
#define CAPI_EXPORT_API __attribute__((visibility("default")))
#else
#define CAPI_EXPORT_API
#endif
#ifdef __i386__
#define CAPI_BIT_LENGTH 32
#define CAPI_PROC __attribute__ ((stdcall))
#endif
#ifdef __amd64__
#define CAPI_BIT_LENGTH 64
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
#define CAPI_BIT_LENGTH 32
#define CAPI_PROC __stdcall
#endif
#ifdef _M_AMD64
#define CAPI_BIT_LENGTH 64
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

typedef char ASCII;   // ASCII Unit
typedef char UTF8;    // UTF8 Unit
#ifdef _MSC_VER
typedef wchar_t UTF16;  // UTF16 Unit
#else
typedef U16 UTF16;    // UTF16 Unit
#endif
#ifdef __GNUC__
typedef wchar_t UTF32;  // UTF32 Unit
#else
typedef U32 UTF32;    // UTF32 Unit
#endif

#ifdef UNICODE
typedef UTF16 STRING;
#define STR(String) L##String
#define capi_Version capi_VersionW
#define capi_ErrorCodeToString capi_ErrorCodeToStringW
#define capi_StrLen capi_StrLenW
#define capi_StrUnits capi_StrUnitsW
#define capi_StrCopy capi_StrCopyW
#define capi_StrAppend capi_StrAppendW
#define capi_StrCompare capi_StrCompareW
#define capi_StrCompareInsensitive capi_StrCompareInsensitiveW
#define capi_StrFind capi_StrFindW
#define capi_StrFindStr capi_StrFindStrW
#define capi_StrFindStrInsensitive capi_StrFindStrInsensitiveW
#define capi_StrSplit capi_StrSplitW
#define capi_StrReverse capi_StrReverseW
#define capi_PrintHex capi_PrintHexW
#define capi_PrintUnsigned capi_PrintUnsignedW
#define capi_PrintSigned capi_PrintSignedW
#else
typedef UTF8 STRING;
#define STR(String) String
#define capi_Version capi_VersionA
#define capi_ErrorCodeToString capi_ErrorCodeToStringA
#define capi_StrLen capi_StrLenU
#define capi_StrUnits capi_StrUnitsU
#define capi_StrCopy capi_StrCopyU
#define capi_StrAppend capi_StrAppendU
#define capi_StrCompare capi_StrCompareU
#define capi_StrCompareInsensitive capi_StrCompareInsensitiveU
#define capi_StrFind capi_StrFindU
#define capi_StrFindStr capi_StrFindStrU
#define capi_StrFindStrInsensitive capi_StrFindStrInsensitiveU
#define capi_StrSplit capi_StrSplitU
#define capi_StrReverse capi_StrReverseU
#define capi_PrintHex capi_PrintHexU
#define capi_PrintUnsigned capi_PrintUnsignedU
#define capi_PrintSigned capi_PrintSignedU
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

/* PRINT (TO STRING) FLAGS */

#define PRINT_POSITIVE      BIT(11)                       // Prints a plus sign ('+') if the value is positive
#define PRINT_HEX           BIT(12)                       // Print in hexadecimal format (signed values are printed as signed hexadecimal)
#define PRINT_x_PREFIX      BIT(13)                       // "0x" printed to indicate hexadecimal format
#define PRINT_X_PREFIX      (BIT(13)|BIT(14))             // "0X" printed to indicate hexadecimal format
#define PRINT_h_PREFIX      (BIT(13)|BIT(15))             // '0' printed, 'h' appended to indicate hexadecimal format
#define PRINT_H_PREFIX      (BIT(13)|BIT(14)|BIT(15))     // '0' printed, 'H' appended to indicate hexadecimal format
#define PRINT_UPPERCASE     BIT(16)                       // Hexadecimal characters a, b, c, d, e and f are printed in uppercase
#define PRINT_LZEROS        BIT(17)                       // Leading zeros of the hexadecimal format are printed
#define PRINT_COMMA         BIT(13)                       // Mark place of thousands with Comma
#define PRINT_PERIOD        BIT(14)                       // Mark place of thousands with Period
#define PRINT_SPACE         (BIT(13)|BIT(14))             // Mark place of thousands with Space
#define PRINT_FCAP          BIT(17)                       // "inf" and "qnan"/"snan" is capitalized
#define PRINT_PAYLOAD       BIT(18)                       // Print the mantissa in 2s complement form following a decimal point when qnan/snan is the output
#define PRINT_e_ENABLE      BIT(19)                       // Enable scientific notation printing with 'e' when possible
#define PRINT_E_ENABLE      (BIT(19)|BIT(20))             // Same as PRINT_e_ENABLE except 'E' is used instead of 'e'
#define PRINT_DP_COMMA      BIT(21)                       // Print the decimal point marker as a Comma instead of a Period. Thousands place marked with a period or space
#define PRINT_ZEROF         BIT(22)                       // Append ".0" if the fraction is zero. Ignored if PRINT_EXACT is used with a value of zero
#define PRINT_MAX(n)        (n << 24)                     // Truncate when fraction digits are greater than (n). Ignored when (n) is zero
#define PRINT_EXACT(n)      (BIT(23)|(n << 24))           // Append '0' until fraction digits equal (n). Truncate when fraction digits are greater than (n). No decimal point if (n) is zero

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

//  PNG_PARAMETERS
//    CompressionMethod [PNG compression method] 0 = zlib
//    Level [The compression level] If CompressionMethod is set to (zlib) This may be any value 0-9 or one of the following constants:
//        Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
//    FilterMethod [The filter method] 0 = filter method 0
//    InterlaceMethod [The interlace method] 0 = interlace method 0 (null method), 1 = interlace method 1 (Adam7 method)
//    IDAT_Size [The maximum size in bytes of the IDAT chunks] Values 0x20000 (128 KB) or 0x80000 (512 KB) are recommended
STRUCT(PNG_PARAMETERS)
{
	U8 CompressionMethod;
	int Level;
	U8 FilterMethod;
	U8 InterlaceMethod;
	U32 IDAT_Size;
};

#ifdef __cplusplus
extern "C" {
#endif

	//  capi_VersionA - Get CAPI Version (version.c)
	//      ppVersion [A pointer to a const char* to receive a pointer to the MULTI-BYTE version string] This can be 0
	//  returns the version as a U32 data type
	CAPI_FUNC(U32) capi_VersionA(const char** ppVersion);

	//  capi_VersionW - Get CAPI Version (version.c)
	//      ppVersion [A pointer to a const wchar_t* to receive a pointer to the UNICODE version string] This can be 0
	//  returns the version as a U32 data type
	CAPI_FUNC(U32) capi_VersionW(const wchar_t** ppVersion);

	//  capi_ErrorCodeToStringA - Get a string representation of a error code (error.c)
	//      ErrorCode [The error code to change to a string]
	//  returns the MULTI-BYTE string representation of the error code or 0 if its an invalid error code
	CAPI_FUNC(const char*) capi_ErrorCodeToStringA(I32 ErrorCode);

	//  capi_ErrorCodeToStringW - Get a string representation of a error code (error.c)
	//      ErrorCode [The error code to change to a string]
	//  returns the UNICODE string representation of the error code or 0 if its an invalid error code
	CAPI_FUNC(const wchar_t*) capi_ErrorCodeToStringW(I32 ErrorCode);

	//  z_def_file - Compress from file source to file destination until EOF on source (zpipe.c)
	//      dest [A file stream opened with fopen to write]
	//      source [A file stream opened with fopen to read]
	//      level [The compression level] This may be any value 0-9 or one of the following constants:
	//          Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
	//  returns Z_OK on success
	CAPI_FUNC(int) z_def_file(FILE* dest, FILE* source, int level);

	//  z_inf_file - Decompress from file source to file destination until stream ends or EOF (zpipe.c)
	//      dest [A file stream opened with fopen to write]
	//      source [A file stream opened with fopen to read]
	//  returns Z_OK on success
	CAPI_FUNC(int) z_inf_file(FILE* dest, FILE* source);

	//  z_def_mem - Compress from memory source to memory destination (zpipe.c)
	//      dest [Pointer to a void* variable to receive a pointer to the compressed file]
	//      desLen [Pointer to a uLong variable to receive the number of bytes outputted to dest]
	//      src [The source file in memory to compress]
	//      srcLen [The source file size in bytes]
	//      level [The compression level] This may be any value 0-9 or one of the following constants:
	//          Z_NO_COMPRESSION, Z_BEST_SPEED, Z_BEST_COMPRESSION, Z_DEFAULT_COMPRESSION
	//  returns Z_OK on success
	CAPI_FUNC(int) z_def_mem(void** dest, uLong* desLen, void* src, uLong srcLen, int level);

	//  z_inf_mem - Decompress from memory source to memory destination (zpipe.c)
	//      dest [Pointer to a void* variable to receive a pointer to the decompressed file]
	//      desLen [Pointer to a uLong variable to receive the number of bytes outputted to dest]
	//      src [The source file in memory to decompress]
	//      srcLen [The source file size in bytes]
	//  returns Z_OK on success
	CAPI_FUNC(int) z_inf_mem(void** dest, uLong* desLen, void* src, uLong srcLen);

	//  memset32 - Sets a buffer to a specified value (memory.c)
	//      pDestination [Pointer to destination]
	//      Value [32-Bit value to set]
	//      nDwords [Number of 32-Bit values]
	CAPI_FUNC(void) capi_memset32(void* pDestination, U32 Value, size_t nDwords);

	//  capi_AddReturnCarry - Add to a variable of flexible length (arithmetic.c)
	//      pAugend [Pointer to the Augend]
	//      Addend [Value to add]
	//      Count [Number of elements in the Augend array] This is the size of the variable
	//  returns the carry flag
	CAPI_FUNC(size_t) capi_AddReturnCarry(size_t* pAugend, size_t Addend, size_t Count);

	//  capi_AddReturnCarryEx - Add to a variable of flexible length with a variable of the same size (arithmetic.c)
	//      pAugend [Pointer to the Augend]
	//      pAddend [Pointer to the Addend]
	//      Count [Number of elements in the Augend array] This is the size of the variable
	//  returns the carry flag
	CAPI_FUNC(size_t) capi_AddReturnCarryEx(size_t* pAugend, size_t* pAddend, size_t Count);

	//  capi_SubReturnBorrow - Subtract from a variable of flexible length (arithmetic.c)
	//      pMinuend [Pointer to the Minuend]
	//      Subtrahend [Value to subtract]
	//      Count [Number of elements in the Minuend array] This is the size of the variable
	//  returns the borrow flag
	CAPI_FUNC(size_t) capi_SubReturnBorrow(size_t* pMinuend, size_t Subtrahend, size_t Count);

	//  capi_SubReturnBorrowEx - Subtract from a variable of flexible length with a variable of the same size (arithmetic.c)
	//      pMinuend [Pointer to the Minuend]
	//      pSubtrahend [Pointer to the Subtrahend]
	//      Count [Number of elements in the Minuend array] This is the size of the variable
	//  returns the borrow flag
	CAPI_FUNC(size_t) capi_SubReturnBorrowEx(size_t* pMinuend, size_t* pSubtrahend, size_t Count);

	//  capi_DivReturnRemainder - Divide a variable of flexible length (arithmetic.c)
	//      pDividend [Pointer to the Dividend]
	//      Divisor [The divisor to use]
	//      Count [Number of elements in the Dividend array] This is the size of the variable
	//  returns the remainder
	CAPI_FUNC(size_t) capi_DivReturnRemainder(size_t* pDividend, size_t Divisor, size_t Count);

	//  capi_MulReturnOverflow - Multiply a variable of flexible length (arithmetic.c)
	//      pMultiplicand [Pointer to the Multiplicand]
	//      Multiplier [The multiplier to use]
	//      Count [Number of elements in the Multiplicand array] This is the size of the variable
	//  returns the overflow
	CAPI_FUNC(size_t) capi_MulReturnOverflow(size_t* pMultiplicand, size_t Multiplier, size_t Count);

	//  capi_Negate - Negate a variable of flexible length (bitwise.c)
	//      pValue [Pointer to the variable to negate]
	//      Count [Number of elements in the variable array] This is the size of the variable
	CAPI_FUNC(void) capi_Negate(size_t* pValue, size_t Count);

	//  capi_CountLZ - Count the leading zeros of a variable of flexible length (bitwise.c)
	//      pValue [Pointer to the variable]
	//      Count [Number of elements in the variable array] This is the size of the variable
	//  returns the count of leading zeros
	CAPI_FUNC(size_t) capi_CountLZ(size_t* pValue, size_t Count);

	//  capi_CountL1 - Count the leading ones of a variable of flexible length (bitwise.c)
	//      pValue [Pointer to the variable]
	//      Count [Number of elements in the variable array] This is the size of the variable
	//  returns the count of leading ones
	CAPI_FUNC(size_t) capi_CountL1(size_t* pValue, size_t Count);

	//  capi_CountTZ - Count the trailing zeros of a variable of flexible length (bitwise.c)
	//      pValue [Pointer to the variable]
	//      Count [Number of elements in the variable array] This is the size of the variable
	//  returns the count of trailing zeros
	CAPI_FUNC(size_t) capi_CountTZ(size_t* pValue, size_t Count);

	//  capi_CountT1 - Count the trailing ones of a variable of flexible length (bitwise.c)
	//      pValue [Pointer to the variable]
	//      Count [Number of elements in the variable array] This is the size of the variable
	//  returns the count of trailing ones
	CAPI_FUNC(size_t) capi_CountT1(size_t* pValue, size_t Count);

	//  capi_TestForZero - Test a variable of flexible length for zero (compare.c)
	//      pData [Pointer to the variable]
	//      Count [Number of elements in the variable array] This is the size of the variable
	//  returns the first element in the array that is non-zero, or zero if all elements are zero
	CAPI_FUNC(size_t) capi_TestForZero(size_t* pData, size_t Count);

	//  capi_Compare - Compare variables of flexible length (compare.c)
	//      pOperand1 [Pointer to the first Operand]
	//      pOperand2 [Pointer to the second Operand]
	//      Count [Number of elements in the variables array] This is the size of the variables
	//  The return value indicates the ordinal relation of Operand1 to Operand2
	//      < 0  Operand1 is less than Operand2
	//        0  Operand1 is identical to Operand2
	//      > 0  Operand1 is greater than Operand2
	CAPI_FUNC(I8) capi_Compare(size_t* pOperand1, size_t* pOperand2, size_t Count);

	//  capi_ZeroExtend - Zero extend a variable to a variable of flexible length (extend.c)
	//      pDestination [Pointer to the destination of the extended variable]
	//      Count [Number of elements in the destination variable array] This is the size of the variable
	//      pSource [Pointer to the source variable to extend]
	//      nBytes [The size in bytes of the source variable]
	CAPI_FUNC(void) capi_ZeroExtend(size_t* pDestination, size_t Count, void* pSource, size_t nBytes);

	//  capi_SignExtend - Sign extend a variable to a variable of flexible length (extend.c)
	//      pDestination [Pointer to the destination of the extended variable]
	//      Count [Number of elements in the destination variable array] This is the size of the variable
	//      pSource [Pointer to the source variable to extend]
	//      nBytes [The size in bytes of the source variable]
	CAPI_FUNC(void) capi_SignExtend(size_t* pDestination, size_t Count, void* pSource, size_t nBytes);

	//  capi_mul64 - Perform a unsigned 64-bit multiply (int64.c)
	//      pResult [Pointer to a U128 variable to receive the result]
	//      A [Unsigned value to multiply]
	//      B [Unsigned value to multiply]
	CAPI_FUNC(void) capi_mul64(U128* pResult, U64 A, U64 B);

	//  capi_imul64 - Perform a signed 64-bit multiply (int64.c)
	//      pResult [Pointer to a I128 variable to receive the result]
	//      A [Signed value to multiply]
	//      B [Signed value to multiply]
	CAPI_FUNC(void) capi_imul64(I128* pResult, I64 A, I64 B);

	//  capi_shl128 - Perform a 128-bit shift left (signed and unsigned are identical) (int128.c)
	//      pValue [Pointer to a 128-bit variable to shift left]
	//      N [The number of bits to shift left by]
	CAPI_FUNC(void) capi_shl128(U128* pValue, U32 N);

	//  capi_shr128 - Perform a unsigned 128-bit shift right (int128.c)
	//      pValue [Pointer to a 128-bit variable to shift right]
	//      N [The number of bits to shift right by]
	CAPI_FUNC(void) capi_shr128(U128* pValue, U32 N);

	//  capi_sar128 - Perform a signed 128-bit shift right (int128.c)
	//      pValue [Pointer to a 128-bit variable to shift right]
	//      N [The number of bits to shift right by]
	CAPI_FUNC(void) capi_sar128(I128* pValue, U32 N);

	//  capi_div128 - Perform a unsigned 128-bit divide (int128.c)
	//      pQuotient [Pointer to a unsigned 128-bit variable to receive the quotient]
	//      pDividend [Pointer to a unsigned 128-bit dividend]
	//      pDivisor [Pointer to a unsigned 128-bit divisor]
	CAPI_FUNC(void) capi_div128(U128* pQuotient, U128* pDividend, U128* pDivisor);

	//  capi_idiv128 - Perform a signed 128-bit divide (int128.c)
	//      pQuotient [Pointer to a signed 128-bit variable to receive the quotient]
	//      pDividend [Pointer to a signed 128-bit dividend]
	//      pDivisor [Pointer to a signed 128-bit divisor]
	CAPI_FUNC(void) capi_idiv128(I128* pQuotient, I128* pDividend, I128* pDivisor);

	//  capi_dvrm128 - Perform a unsigned 128-bit divide with remainder (int128.c)
	//      pQuotient [Pointer to a unsigned 128-bit variable to receive the quotient]
	//      pRemainder [Pointer to a unsigned 128-bit variable to receive the remainder]
	//      pDividend [Pointer to a unsigned 128-bit dividend]
	//      pDivisor [Pointer to a unsigned 128-bit divisor]
	CAPI_FUNC(void) capi_dvrm128(U128* pQuotient, U128* pRemainder, U128* pDividend, U128* pDivisor);

	//  capi_idvrm128 - Perform a signed 128-bit divide with remainder (int128.c)
	//      pQuotient [Pointer to a signed 128-bit variable to receive the quotient]
	//      pRemainder [Pointer to a signed 128-bit variable to receive the remainder]
	//      pDividend [Pointer to a signed 128-bit dividend]
	//      pDivisor [Pointer to a signed 128-bit divisor]
	CAPI_FUNC(void) capi_idvrm128(I128* pQuotient, I128* pRemainder, I128* pDividend, I128* pDivisor);

	//  capi_rem128 - Perform a unsigned 128-bit remainder (int128.c)
	//      pRemainder [Pointer to a unsigned 128-bit variable to receive the remainder]
	//      pDividend [Pointer to a unsigned 128-bit dividend]
	//      pDivisor [Pointer to a unsigned 128-bit divisor]
	CAPI_FUNC(void) capi_rem128(U128* pRemainder, U128* pDividend, U128* pDivisor);

	//  capi_irem128 - Perform a signed 128-bit remainder (int128.c)
	//      pRemainder [Pointer to a signed 128-bit variable to receive the remainder]
	//      pDividend [Pointer to a signed 128-bit dividend]
	//      pDivisor [Pointer to a signed 128-bit divisor]
	CAPI_FUNC(void) capi_irem128(I128* pRemainder, I128* pDividend, I128* pDivisor);

	//  capi_llmul128 - Perform a 128-bit multiply (signed and unsigned are identical) (int128.c)
	//      p128 [Pointer to a 128-bit variable to receive the result]
	//      pA [Pointer to a 128-bit variable to multiply]
	//      pB [Pointer to a 128-bit variable to multiply]
	CAPI_FUNC(void) capi_llmul128(U128* p128, U128* pA, U128* pB);

	//  capi_mul128 - Perform a unsigned 128-bit multiply (int128.c)
	//      p256 [Pointer to a U256 variable to receive the result]
	//      pA [Pointer to a U128 variable to multiply]
	//      pB [Pointer to a U128 variable to multiply]
	CAPI_FUNC(void) capi_mul128(U256* p256, U128* pA, U128* pB);

	//  capi_imul128 - Perform a signed 128-bit multiply (int128.c)
	//      p256 [Pointer to a I256 variable to receive the result]
	//      pA [Pointer to a I128 variable to multiply]
	//      pB [Pointer to a I128 variable to multiply]
	CAPI_FUNC(void) capi_imul128(I256* p256, I128* pA, I128* pB);

	//  capi_CreateImage - Creates a new image of the standard format (image.c)
	//      pImage [Pointer to a IMAGE structure to receive the new image data]
	//      Width [The width for the new image]
	//      Height [The height for the new image]
	//      Color [The color for the new image] Use the COLOR macro to generate the color or specify 0 for a transparent image
	//      Alignment [The alignment of the image in memory and its scanlines] This value is the number of PIXEL, Not bytes
	//  returns a pointer to the image pixels on success, otherwise 0
	//      Call capi_FreeImage when done using the image
	CAPI_FUNC(PIXEL*) capi_CreateImage(IMAGE* pImage, U32 Width, U32 Height, U32 Color, U32 Alignment);

	//  capi_FreeImage - Free a image resources (image.c)
	//      pImage [Pointer to a IMAGE structure of the image to free]
	//  This function sets the IMAGE structure members to 0 after freeing resources
	CAPI_FUNC(void) capi_FreeImage(IMAGE* pImage);

	//  capi_FillImage - Fill an image with a color (image.c)
	//      pImage [Pointer to a IMAGE structure of the image to fill]
	//      Color [The color to fill the image] Use the COLOR macro to generate the color or specify 0 for a transparent image
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_FillImage(IMAGE* pImage, U32 Color);

	//  capi_RenderAlphaPixel - Render a pixel with a pixel and alpha value (image.c)
	//      pDestination [The pixel that the Color parameter is applied to]
	//      Color [The color that is applied to the pixel] The alpha channel of the color is used to apply the color
	//      Alpha [A alpha value that is applied to the Color parameter]
	//  This function does not check if pDestination is valid
	CAPI_FUNC(void) capi_RenderAlphaPixel(PIXEL* pDestination, U32 Color, U8 Alpha);

	//  capi_RenderAlphaScanLine - Render a scanline with a scanline and alpha value (image.c)
	//      pDestination [The pixels that the pSource parameter is applied to]
	//      pSource [The pixels that are applied to pDestination]
	//      Width [The number of PIXEL to apply]
	//      Alpha [A alpha value that is applied to the pSource parameter pixels]
	//  This function does not check if pDestination or pSource is valid
	CAPI_FUNC(void) capi_RenderAlphaScanLine(PIXEL* pDestination, PIXEL* pSource, U32 Width, U8 Alpha);

	//  capi_RenderAlphaScanLine32 - Render a scanline with a pixel and alpha value (image.c)
	//      pDestination [The pixels that the Color parameter is applied to]
	//      Color [The color that is applied to the scanline] The alpha channel of the color is used to apply the color
	//      Width [The number of PIXEL to apply]
	//      Alpha [A alpha value that is applied to the Color parameter]
	//  This function does not check if pDestination is valid
	CAPI_FUNC(void) capi_RenderAlphaScanLine32(PIXEL* pDestination, U32 Color, U32 Width, U8 Alpha);

	//  capi_DrawLine - Draw a line on an image (image.c)
	//      pDestination [Pointer to a IMAGE structure of the image to draw on]
	//      pRect [Pointer to a CRECT structure specifying the coordinates of the line to draw]
	//      Color [The color used to draw the line] The alpha channel of the color is used to apply the color
	//      Thickness [The thickness of the line]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_DrawLine(IMAGE* pDestination, CRECT* pRect, U32 Color, U32 Thickness);

	//  capi_DrawRect - Draw a rectangle on an image (image.c)
	//      pDestination [Pointer to a IMAGE structure of the image to draw on]
	//      pRect [Pointer to a CRECT structure specifying the coordinates of the rectangle to draw]
	//      LineColor [The color of the rectangle lines] The alpha channel of the color is used to apply the color
	//      Thickness [The thickness of the lines]
	//      FillColor [The color to fill the rectangle] The alpha channel of the color is used to apply the color
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_DrawRect(IMAGE* pDestination, CRECT* pRect, U32 LineColor, U32 Thickness, U32 FillColor);

	//  capi_DrawImage - Draw an image onto another image at X,Y coordinates (image.c)
	//      pDestination [The destination image of the source image]
	//      pSource [The source image to draw]
	//      X [The x coordinate to draw the source image at]
	//      Y [The y coordinate to draw the source image at]
	//  returns CAPI_ERROR_NONE on success
	//      This function ignores the alpha channel of the source image. Use capi_DrawImageA for alpha rendering
	CAPI_FUNC(I32) capi_DrawImage(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y);

	//  capi_DrawImageA - Draw an image onto another image at X,Y coordinates with alpha rendering (image.c)
	//      pDestination [The destination image of the source image]
	//      pSource [The source image to draw]
	//      X [The x coordinate to draw the source image at]
	//      Y [The y coordinate to draw the source image at]
	//      Alpha [A alpha value that is applied to the source image]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_DrawImageA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Alpha);

	//  capi_DrawImageEx - Draw an image onto another image at X,Y coordinates with stretching (image.c)
	//      pDestination [The destination image of the source image]
	//      pSource [The source image to stretch and draw]
	//      X [The x coordinate to draw the source image at]
	//      Y [The y coordinate to draw the source image at]
	//      Algorithm [The algorithm used to stretch the source image] This may be one of the following constants:
	//          DRAW_RESIZE_LINEAR, DRAW_RESIZE_BOXSAMP
	//      Width [The new width of the source image]
	//      Height [The new height of the source image]
	//  returns CAPI_ERROR_NONE on success
	//      This function ignores the alpha channel of the source image. Use capi_DrawImageExA for stretching with alpha rendering
	CAPI_FUNC(I32) capi_DrawImageEx(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height);

	//  capi_DrawImageExA - Draw an image onto another image at X,Y coordinates with stretching and alpha rendering (image.c)
	//      pDestination [The destination image of the source image]
	//      pSource [The source image to stretch and draw]
	//      X [The x coordinate to draw the source image at]
	//      Y [The y coordinate to draw the source image at]
	//      Algorithm [The algorithm used to stretch the source image] This may be one of the following constants:
	//          DRAW_RESIZE_LINEAR, DRAW_RESIZE_BOXSAMP
	//      Width [The new width of the source image]
	//      Height [The new height of the source image]
	//      Alpha [A alpha value that is applied to the source image]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_DrawImageExA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height, U8 Alpha);

	//  capi_Load_BMP_FromMemory - Loads a BMP (bitmap) formatted image from memory into a standard IMAGE format (bmp.c)
	//      pImage [Pointer to a IMAGE structure to receive the loaded image data]
	//      Alignment [The alignment to use for the image and its scanlines] This value is the number of PIXEL, Not bytes
	//      pBmpFile [A pointer to the source BMP file in memory]
	//      FileSize [The size of the source file in bytes]
	//  returns CAPI_ERROR_NONE on success
	//      Its recommended to use capi_LoadImageFromMemory instead
	CAPI_FUNC(I32) capi_Load_BMP_FromMemory(IMAGE* pImage, U32 Alignment, BMP* pBmpFile, size_t FileSize);

	//  capi_Load_JPG_FromMemory - Loads a JPG (jpeg) formatted image from memory into a standard IMAGE format (jpg.c)
	//      pImage [Pointer to a IMAGE structure to receive the loaded image data]
	//      Alignment [The alignment to use for the image and its scanlines] This value is the number of PIXEL, Not bytes
	//      pJpgFile [A pointer to the source JPG file in memory]
	//      FileSize [The size of the source file in bytes]
	//  returns CAPI_ERROR_NONE on success
	//      Its recommended to use capi_LoadImageFromMemory instead
	CAPI_FUNC(I32) capi_Load_JPG_FromMemory(IMAGE* pImage, U32 Alignment, JPG* pJpgFile, size_t FileSize);

	//  capi_Load_PNG_FromMemory - Loads a PNG (Portable Network Graphics) formatted image from memory into a standard IMAGE format (png.c)
	//      pImage [Pointer to a IMAGE structure to receive the loaded image data]
	//      Alignment [The alignment to use for the image and its scanlines] This value is the number of PIXEL, Not bytes
	//      pPngFile [A pointer to the source PNG file in memory]
	//      FileSize [The size of the source file in bytes]
	//  returns CAPI_ERROR_NONE on success
	//      Its recommended to use capi_LoadImageFromMemory instead
	CAPI_FUNC(I32) capi_Load_PNG_FromMemory(IMAGE* pImage, U32 Alignment, PNG* pPngFile, size_t FileSize);

	//  capi_Load_ICO_FromMemory - Loads a ICO (icon) formatted image from memory into a standard IMAGE format (ico.c)
	//      pImage [Pointer to a IMAGE structure to receive the loaded image data]
	//      Alignment [The alignment to use for the image and its scanlines] This value is the number of PIXEL, Not bytes
	//      pIcoFile [A pointer to the source ICO file in memory]
	//      FileSize [The size of the source file in bytes]
	//      IndexNumber [The index of the embedded image to load] Use -1 to load the image with the greatest width and height
	//  returns CAPI_ERROR_NONE on success
	//      If the paramter IndexNumber is not -1 and the index does not exist then this function returns CAPI_ERROR_INVALID_PARAMETER
	//      Its recommended to use capi_LoadImageFromMemory instead
	CAPI_FUNC(I32) capi_Load_ICO_FromMemory(IMAGE* pImage, U32 Alignment, ICO* pIcoFile, size_t FileSize, I8 IndexNumber);

	//  capi_LoadImageFromMemory - Loads a formatted image from memory into a standard IMAGE format (image.c)
	//      pImage [Pointer to a IMAGE structure to receive the loaded image data]
	//      Alignment [The alignment to use for the image and its scanlines] This value is the number of PIXEL, Not bytes
	//      pFile [A pointer to the source image file in memory]
	//      FileSize [The size of the source file in bytes]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_LoadImageFromMemory(IMAGE* pImage, U32 Alignment, void* pFile, U64 FileSize);

	//  capi_Get_BMP_DimensionsFromMemory - Gets the dimensions of a BMP (bitmap) formatted image in memory (bmp.c)
	//      pBmpFile [Pointer to a BMP formatted image]
	//      FileSize [The size of the image in bytes]
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromMemory instead
	CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromMemory(BMP* pBmpFile, size_t FileSize, U32* pHeight);

	//  capi_Get_JPG_DimensionsFromMemory - Gets the dimensions of a JPG (jpeg) formatted image in memory (jpg.c)
	//      pJpgFile [Pointer to a JPG formatted image]
	//      FileSize [The size of the image in bytes]
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromMemory instead
	CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromMemory(JPG* pJpgFile, size_t FileSize, U32* pHeight);

	//  capi_Get_PNG_DimensionsFromMemory - Gets the dimensions of a PNG (Portable Network Graphics) formatted image in memory (png.c)
	//      pPngFile [Pointer to a PNG formatted image]
	//      FileSize [The size of the image in bytes]
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromMemory instead
	CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromMemory(PNG* pPngFile, size_t FileSize, U32* pHeight);

	//  capi_Get_ICO_DimensionsFromMemory - Gets the dimensions of a ICO (icon) formatted image in memory (ico.c)
	//      pIcoFile [Pointer to a ICO formatted image]
	//      FileSize [The size of the image in bytes]
	//      pHeight [Pointer to a variable to receive the height of the image]
	//      IndexNumber [The index of the embedded image] Use -1 for the image with the greatest width and height
	//  returns the width of the image, or 0 for an error
	//      If the paramter IndexNumber is not -1 and the index does not exist then this function returns 0
	//      Its recommended to use capi_GetImageDimensionsFromMemory instead
	CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromMemory(ICO* pIcoFile, size_t FileSize, U32* pHeight, I8 IndexNumber);

	//  capi_GetImageDimensionsFromMemory - Gets the dimensions of a formatted image in memory (image.c)
	//      pFile [Pointer to a formatted image]
	//      FileSize [The size of the image in bytes]
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	CAPI_FUNC(U32) capi_GetImageDimensionsFromMemory(void* pFile, size_t FileSize, U32* pHeight);

	//  capi_Get_BMP_DimensionsFromFile - Gets the dimensions of a BMP (bitmap) formatted image from a file (bmp.c)
	//      pBmpFile [A stream to a BMP formatted image] Use fopen to open the stream
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromFile instead
	CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromFile(FILE* pBmpFile, U32* pHeight);

	//  capi_Get_JPG_DimensionsFromFile - Gets the dimensions of a JPG (jpeg) formatted image from a file (jpg.c)
	//      pJpgFile [A stream to a JPG formatted image] Use fopen to open the stream
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromFile instead
	CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromFile(FILE* pJpgFile, U32* pHeight);

	//  capi_Get_PNG_DimensionsFromFile - Gets the dimensions of a PNG (Portable Network Graphics) formatted image from a file (png.c)
	//      pPngFile [A stream to a PNG formatted image] Use fopen to open the stream
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	//      Its recommended to use capi_GetImageDimensionsFromFile instead
	CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromFile(FILE* pPngFile, U32* pHeight);

	//  capi_Get_ICO_DimensionsFromFile - Gets the dimensions of a ICO (icon) formatted image from a file (ico.c)
	//      pIcoFile [A stream to a ICO formatted image] Use fopen to open the stream
	//      pHeight [Pointer to a variable to receive the height of the image]
	//      IndexNumber [The index of the embedded image] Use -1 for the image with the greatest width and height
	//  returns the width of the image, or 0 for an error
	//      If the paramter IndexNumber is not -1 and the index does not exist then this function returns 0
	//      Its recommended to use capi_GetImageDimensionsFromFile instead
	CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromFile(FILE* pIcoFile, U32* pHeight, I8 IndexNumber);

	//  capi_GetImageDimensionsFromFile - Gets the dimensions of a formatted image from a file (image.c)
	//      pFile [A stream to a formatted image] Use fopen to open the stream
	//      pHeight [Pointer to a variable to receive the height of the image]
	//  returns the width of the image, or 0 for an error
	CAPI_FUNC(U32) capi_GetImageDimensionsFromFile(FILE* pFile, U32* pHeight);

	//  capi_Create_BMP_ToMemory - Create a BMP (bitmap) formatted image to memory (bmp.c)
	//      ppFilePointer [Pointer to a variable to receive the pointer to the file]
	//      pFileSize [Pointer to a variable to receive the size of the file in bytes]
	//      pImage [Pointer to a IMAGE structure to create the new BMP image from]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_Create_BMP_ToMemory(BMP** ppFilePointer, U64* pFileSize, IMAGE* pImage);

	//  capi_Create_JPG_ToMemory - Create a JPG (jpeg) formatted image to memory (jpg.c)
	//      ppFilePointer [Pointer to a variable to receive the pointer to the file]
	//      pFileSize [Pointer to a variable to receive the size of the file in bytes]
	//      pImage [Pointer to a IMAGE structure to create the new JPG image from]
	//      Quality [The quality of the JPG formatted image] This can be a value 1-100, 0 (terrible), 100 (very good), 74 is recommended
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_Create_JPG_ToMemory(JPG** ppFilePointer, U64* pFileSize, IMAGE* pImage, U8 Quality);

	//  capi_Create_PNG_ToMemory - Create a PNG (Portable Network Graphics) formatted image to memory (png.c)
	//      ppFilePointer [Pointer to a variable to receive the pointer to the file]
	//      pFileSize [Pointer to a variable to receive the size of the file in bytes]
	//      pImage [Pointer to a IMAGE structure to create the new PNG image from]
	//      pParameters [Pointer to a PNG_PARAMETERS structure that describes the type of PNG image to create]
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_Create_PNG_ToMemory(PNG** ppFilePointer, U64* pFileSize, IMAGE* pImage, PNG_PARAMETERS* pParameters);

	//  capi_Create_ICO_ToMemory - Create a ICO (icon) formatted image to memory (ico.c)
	//      ppFilePointer [Pointer to a variable to receive the pointer to the file]
	//      pFileSize [Pointer to a variable to receive the size of the file in bytes]
	//      pImageList [Pointer to an array of IMAGE structures to create the new ICO images from]
	//      nImages [The number of IMAGE structures pointed to by pImageList]
	//      Format [The format to use for the embedded images] This can be one of the following constants: ICO_FORMAT_BMP, ICO_FORMAT_PNG
	//      pParameters [Pointer to a PNG_PARAMETERS structure that describes the type of PNG image to create] This should be 0 for ICO_FORMAT_BMP
	//  returns CAPI_ERROR_NONE on success
	CAPI_FUNC(I32) capi_Create_ICO_ToMemory(ICO** ppFilePointer, U64* pFileSize, IMAGE* pImageList, U16 nImages, U8 Format, void* pParameters);

	//  capi_StrLenA - Get the length of a ASCII string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of characters in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrLenA(const ASCII* String);

	//  capi_StrCopyA - Copy a ASCII string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in ASCII units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters copied to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrCopyA(ASCII* Destination, size_t Length, const ASCII* Source);

	//  capi_StrAppendA - Append a ASCII string (strings.c)
	//      Destination [Null-terminated destination string buffer]
	//      Length [Length of the destination string buffer in ASCII units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters appended to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrAppendA(ASCII* Destination, size_t Length, const ASCII* Source);

	//  capi_StrCompareA - Compare ASCII strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareA(const ASCII* String1, const ASCII* String2);

	//  capi_StrCompareInsensitiveA - Perform a case-insensitive comparison of ASCII strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareInsensitiveA(const ASCII* String1, const ASCII* String2);

	//  capi_StrFindA - Find a character in a ASCII string (strings.c)
	//      String [Null-terminated source string to search]
	//      Delimit [Character to be located]
	//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
	CAPI_FUNC(ASCII*) capi_StrFindA(const ASCII* String, ASCII Delimit);

	//  capi_StrFindStrA - Find a ASCII string in a ASCII string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(ASCII*) capi_StrFindStrA(const ASCII* String, const ASCII* StrDelimit);

	//  capi_StrFindStrInsensitiveA - Perform a case-insensitive search for a ASCII string in a ASCII string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(ASCII*) capi_StrFindStrInsensitiveA(const ASCII* String, const ASCII* StrDelimit);

	//  capi_StrSplitA - Split a ASCII string (strings.c)
	//      String [Null-terminated source string to split]
	//      Delimit [Character to be located and replaced]
	//  returns a pointer to the ASCII string following the Delimit, or 0 if Delimit is not found
	//      When Delimit is found its set to 0
	CAPI_FUNC(ASCII*) capi_StrSplitA(ASCII* String, ASCII Delimit);

	//  capi_StrReverseA - Reverse a ASCII string (strings.c)
	//      String [Null-terminated source string to reverse]
	CAPI_FUNC(void) capi_StrReverseA(ASCII* String);

	//  capi_UTF8_GetCharSize - Get the number of bytes a UTF8 code-point encoding uses (strings.c)
	//      Code [The 1st UTF8 unit of the code-point encoding]
	//  returns the number of UTF8 units the code-point encoding uses, or 0 for an error
	CAPI_FUNC(U8) capi_UTF8_GetCharUnits(UTF8 Code);

	//  capi_UTF8_Encode_Unsafe - Encode a code-point using the UTF8 encoding (strings.c)
	//      String [The destination string buffer]
	//      CodePoint [The code-point to encode]
	//  returns the number of UTF8 units that was written to String, or 0 for an error
	//      This function does not check if String is valid and does not output any null-terminator character
	//      Consider using the safer version capi_UTF8_Encode
	CAPI_FUNC(U8) capi_UTF8_Encode_Unsafe(UTF8* String, U32 CodePoint);

	//  capi_UTF8_Encode - Encode a code-point using the UTF8 encoding (strings.c)
	//      String [The destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      CodePoint [The code-point to encode]
	//  returns the number of UTF8 units that was written to String, not including the terminating null character, or 0 for an error
	//      This function does not check if String is valid
	CAPI_FUNC(U8) capi_UTF8_Encode(UTF8* String, size_t Length, U32 CodePoint);

	//  capi_UTF8_Decode - Decode a UTF8 encoding into a code-point (strings.c)
	//      Units [The number of UTF8 units the code-point uses] Use capi_UTF8_GetCharUnits to get this value
	//      String [The source string to decode a code-point from]
	//  returns the decoded code-point, or 0 for an error
	//      This function does not check if String is valid
	CAPI_FUNC(U32) capi_UTF8_Decode(U8 Units, const UTF8* String);

	//  capi_StrLenU - Get the length of a UTF8 string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of characters in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrLenU(const UTF8* String);

	//  capi_StrUnitsU - Get the number of UTF8 units in a UTF8 string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of UTF8 units in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrUnitsU(const UTF8* String);

	//  capi_StrCopyU - Copy a UTF8 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF8 units copied to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrCopyU(UTF8* Destination, size_t Length, const UTF8* Source);

	//  capi_StrAppendU - Append a UTF8 string (strings.c)
	//      Destination [Null-terminated destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF8 units appended to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrAppendU(UTF8* Destination, size_t Length, const UTF8* Source);

	//  capi_StrCompareU - Compare UTF8 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareU(const UTF8* String1, const UTF8* String2);

	//  capi_StrCompareInsensitiveU - Perform a case-insensitive comparison of UTF8 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareInsensitiveU(const UTF8* String1, const UTF8* String2);

	//  capi_StrFindU - Find a character in a UTF8 string (strings.c)
	//      String [Null-terminated source string to search]
	//      Delimit [Character to be located]
	//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
	CAPI_FUNC(UTF8*) capi_StrFindU(const UTF8* String, U32 Delimit);

	//  capi_StrFindStrU - Find a UTF8 string in a UTF8 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF8*) capi_StrFindStrU(const UTF8* String, const UTF8* StrDelimit);

	//  capi_StrFindStrInsensitiveU - Perform a case-insensitive search for a UTF8 string in a UTF8 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF8*) capi_StrFindStrInsensitiveU(const UTF8* String, const UTF8* StrDelimit);

	//  capi_StrSplitU - Split a UTF8 string (strings.c)
	//      String [Null-terminated source string to split]
	//      Delimit [Character to be located and replaced]
	//  returns a pointer to the UTF8 string following the Delimit, or 0 if Delimit is not found
	//      When Delimit is found its set to 0
	CAPI_FUNC(UTF8*) capi_StrSplitU(UTF8* String, U32 Delimit);

	//  capi_StrReverseU - Reverse a UTF8 string (strings.c)
	//      String [Null-terminated source string to reverse]
	CAPI_FUNC(void) capi_StrReverseU(UTF8* String);

	//  capi_UTF16_GetCharUnits - Get the number of words a UTF16 code-point encoding uses (strings.c)
	//      Code [The 1st UTF16 unit of the code-point encoding]
	//  returns the number of UTF16 units the code-point encoding uses, or 0 for an error
	CAPI_FUNC(U8) capi_UTF16_GetCharUnits(UTF16 Code);

	//  capi_UTF16_Encode_Unsafe - Encode a code-point using the UTF16 encoding (strings.c)
	//      String [The destination string buffer]
	//      CodePoint [The code-point to encode]
	//  returns the number of UTF16 units that was written to String, or 0 for an error
	//      This function does not check if String is valid and does not output any null-terminator character
	//      Consider using the safer version capi_UTF16_Encode
	CAPI_FUNC(U8) capi_UTF16_Encode_Unsafe(UTF16* String, U32 CodePoint);

	//  capi_UTF16_Encode - Encode a code-point using the UTF16 encoding (strings.c)
	//      String [The destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      CodePoint [The code-point to encode]
	//  returns the number of UTF16 units that was written to String, not including the terminating null character, or 0 for an error
	//      This function does not check if String is valid
	CAPI_FUNC(U8) capi_UTF16_Encode(UTF16* String, size_t Length, U32 CodePoint);

	//  capi_UTF16_Decode - Decode a UTF16 encoding into a code-point (strings.c)
	//      Units [The number of UTF16 units the code-point uses] Use capi_UTF16_GetCharUnits to get this value
	//      String [The source string to decode a code-point from]
	//  returns the decoded code-point, or 0 for an error
	//      This function does not check if String is valid
	CAPI_FUNC(U32) capi_UTF16_Decode(U8 Units, const UTF16* String);

	//  capi_StrLenW - Get the length of a UTF16 string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of characters in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrLenW(const UTF16* String);

	//  capi_StrUnitsW - Get the number of UTF16 units in a UTF16 string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of UTF16 units in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	//      To get the size of the string in bytes, multiply the result by sizeof(UTF16)
	CAPI_FUNC(size_t) capi_StrUnitsW(const UTF16* String);

	//  capi_StrCopyW - Copy a UTF16 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF16 units copied to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrCopyW(UTF16* Destination, size_t Length, const UTF16* Source);

	//  capi_StrAppendW - Append a UTF16 string (strings.c)
	//      Destination [Null-terminated destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF16 units appended to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrAppendW(UTF16* Destination, size_t Length, const UTF16* Source);

	//  capi_StrCompareW - Compare UTF16 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareW(const UTF16* String1, const UTF16* String2);

	//  capi_StrCompareInsensitiveW - Perform a case-insensitive comparison of UTF16 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareInsensitiveW(const UTF16* String1, const UTF16* String2);

	//  capi_StrFindW - Find a character in a UTF16 string (strings.c)
	//      String [Null-terminated source string to search]
	//      Delimit [Character to be located]
	//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
	CAPI_FUNC(UTF16*) capi_StrFindW(const UTF16* String, U32 Delimit);

	//  capi_StrFindStrW - Find a UTF16 string in a UTF16 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF16*) capi_StrFindStrW(const UTF16* String, const UTF16* StrDelimit);

	//  capi_StrFindStrInsensitiveW - Perform a case-insensitive search for a UTF16 string in a UTF16 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF16*) capi_StrFindStrInsensitiveW(const UTF16* String, const UTF16* StrDelimit);

	//  capi_StrSplitW - Split a UTF16 string (strings.c)
	//      String [Null-terminated source string to split]
	//      Delimit [Character to be located and replaced]
	//  returns a pointer to the UTF16 string following the Delimit, or 0 if Delimit is not found
	//      When Delimit is found its set to 0
	CAPI_FUNC(UTF16*) capi_StrSplitW(UTF16* String, U32 Delimit);

	//  capi_StrReverseW - Reverse a UTF16 string (strings.c)
	//      String [Null-terminated source string to reverse]
	CAPI_FUNC(void) capi_StrReverseW(UTF16* String);

	//  capi_StrLenL - Get the length of a UTF32 string (strings.c)
	//      String [Pointer to a null-terminated string]
	//  returns the number of characters in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrLenL(const UTF32* String);

	//  capi_StrCopyL - Copy a UTF32 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF32 units copied to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrCopyL(UTF32* Destination, size_t Length, const UTF32* Source);

	//  capi_StrAppendL - Append a UTF32 string (strings.c)
	//      Destination [Null-terminated destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of UTF32 units appended to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_StrAppendL(UTF32* Destination, size_t Length, const UTF32* Source);

	//  capi_StrCompareL - Compare UTF32 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareL(const UTF32* String1, const UTF32* String2);

	//  capi_StrCompareInsensitiveL - Perform a case-insensitive comparison of UTF32 strings (strings.c)
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	CAPI_FUNC(I32) capi_StrCompareInsensitiveL(const UTF32* String1, const UTF32* String2);

	//  capi_StrFindL - Find a character in a UTF32 string (strings.c)
	//      String [Null-terminated source string to search]
	//      Delimit [Character to be located]
	//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
	CAPI_FUNC(UTF32*) capi_StrFindL(const UTF32* String, U32 Delimit);

	//  capi_StrFindStrL - Find a UTF32 string in a UTF32 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF32*) capi_StrFindStrL(const UTF32* String, const UTF32* StrDelimit);

	//  capi_StrFindStrInsensitiveL - Perform a case-insensitive search for a UTF32 string in a UTF32 string (strings.c)
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	CAPI_FUNC(UTF32*) capi_StrFindStrInsensitiveL(const UTF32* String, const UTF32* StrDelimit);

	//  capi_StrSplitL - Split a UTF32 string (strings.c)
	//      String [Null-terminated source string to split]
	//      Delimit [Character to be located and replaced]
	//  returns a pointer to the UTF32 string following the Delimit, or 0 if Delimit is not found
	//      When Delimit is found its set to 0
	CAPI_FUNC(UTF32*) capi_StrSplitL(UTF32* String, U32 Delimit);

	//  capi_StrReverseL - Reverse a UTF32 string (strings.c)
	//      String [Null-terminated source string to reverse]
	CAPI_FUNC(void) capi_StrReverseL(UTF32* String);

	//  capi_UTF8_To_UTF16 - Convert a UTF8 string to a UTF16 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF16 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF8_To_UTF16(UTF16* Destination, size_t Length, const UTF8* Source);

	//  capi_UTF8_To_UTF32 - Convert a UTF8 string to a UTF32 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF32 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF8_To_UTF32(UTF32* Destination, size_t Length, const UTF8* Source);

	//  capi_UTF16_To_UTF8 - Convert a UTF16 string to a UTF8 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF8 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF16_To_UTF8(UTF8* Destination, size_t Length, const UTF16* Source);

	//  capi_UTF16_To_UTF32 - Convert a UTF16 string to a UTF32 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF32 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF16_To_UTF32(UTF32* Destination, size_t Length, const UTF16* Source);

	//  capi_UTF32_To_UTF8 - Convert a UTF32 string to a UTF8 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF8 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF32_To_UTF8(UTF8* Destination, size_t Length, const UTF32* Source);

	//  capi_UTF32_To_UTF16 - Convert a UTF32 string to a UTF16 string (strings.c)
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of characters converted to UTF16 and put into the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_UTF32_To_UTF16(UTF16* Destination, size_t Length, const UTF32* Source);

	//  capi_PrintHexA - Convert a data variable to its hexadecimal ASCII string representation (hexadecimal.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in ASCII units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the hexadecimal string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintHexA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintHexU - Convert a data variable to its hexadecimal UTF8 string representation (hexadecimal.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the hexadecimal string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintHexU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintHexW - Convert a data variable to its hexadecimal UTF16 string representation (hexadecimal.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the hexadecimal string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintHexW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintHexL - Convert a data variable to its hexadecimal UTF32 string representation (hexadecimal.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the hexadecimal string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintHexL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintUnsignedA - Convert a data variable to a unsigned ASCII string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in ASCII units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the unsigned string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintUnsignedA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintSignedA - Convert a data variable to a signed ASCII string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in ASCII units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the signed string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintSignedA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintUnsignedU - Convert a data variable to a unsigned UTF8 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the unsigned string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintUnsignedU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintSignedU - Convert a data variable to a signed UTF8 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF8 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the signed string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintSignedU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintUnsignedW - Convert a data variable to a unsigned UTF16 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the unsigned string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintUnsignedW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintSignedW - Convert a data variable to a signed UTF16 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF16 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the signed string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintSignedW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintUnsignedL - Convert a data variable to a unsigned UTF32 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the unsigned string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintUnsignedL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

	//  capi_PrintSignedL - Convert a data variable to a signed UTF32 string representation (print.c)
	//      pBuffer [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in UTF32 units]
	//      pValue [Pointer to the data variable to convert]
	//      Format [Style of the signed string] This can a combination of the PRINT_xxx flags
	//      nBytes [The number of bytes pValue points to] Use the sizeof operator to get this value
	//  returns the number of characters written to the Destination buffer, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	CAPI_FUNC(size_t) capi_PrintSignedL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
struct String
{
	//  Get the length of a string
	//      String [Pointer to a null-terminated string]
	//  returns the number of characters in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	static size_t Length(const STRING* String)
	{
		return capi_StrLen(String);
	}

	//  Get the number of units in a string
	//      String [Pointer to a null-terminated string]
	//  returns the number of units in the string, not including the terminating null character
	//      -1 is returned for an invalid parameter
	//      To get the size of the string in bytes, multiply the result by sizeof(STRING)
	static size_t Units(const STRING* String)
	{
		return capi_StrUnits(String);
	}

	//  Copy a string
	//      Destination [Pointer to the destination string buffer]
	//      Length [Length of the destination string buffer in units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of units copied to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	static size_t Copy(STRING* Destination, size_t Length, const STRING* Source)
	{
		return capi_StrCopy(Destination, Length, Source);
	}

	//  Append a string
	//      Destination [Null-terminated destination string buffer]
	//      Length [Length of the destination string buffer in units]
	//      Source [Null-terminated source string buffer]
	//  returns the number of units appended to the destination string, not including the terminating null character
	//      If there is no null terminator within Length, then Length is returned to indicate the error condition
	//      -1 is returned for an invalid parameter
	static size_t Append(STRING* Destination, size_t Length, const STRING* Source)
	{
		return capi_StrAppend(Destination, Length, Source);
	}

	//  Compare strings
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	static I32 Compare(const STRING* String1, const STRING* String2)
	{
		return capi_StrCompare(String1, String2);
	}

	//  Perform a case-insensitive comparison of strings
	//      String1 [Null-terminated string to compare]
	//      String2 [Null-terminated string to compare]
	//  The return value indicates the ordinal relation of String1 to String2
	//      < 0  String1 is less than String2
	//        0  String1 is identical to String2
	//      > 0  String1 is greater than String2
	//      0x7FFFFFFF is returned for an invalid parameter
	static I32 CompareInsensitive(const STRING* String1, const STRING* String2)
	{
		return capi_StrCompareInsensitive(String1, String2);
	}

	//  Find a character in a string
	//      String [Null-terminated source string to search]
	//      Delimit [Character to be located]
	//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
	static STRING* Find(const STRING* String, STRING Delimit)
	{
		return capi_StrFind(String, Delimit);
	}

	//  Find a string in a string
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	static STRING* FindStr(const STRING* String, const STRING* StrDelimit)
	{
		return capi_StrFindStr(String, StrDelimit);
	}

	//  Perform a case-insensitive search for a string in a string
	//      String [Null-terminated source string to search]
	//      StrDelimit [String to be located]
	//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
	static STRING* FindStrInsensitive(const STRING* String, const STRING* StrDelimit)
	{
		return capi_StrFindStrInsensitive(String, StrDelimit);
	}

	//  Split a string
	//      String [Null-terminated source string to split]
	//      Delimit [Character to be located and replaced]
	//  returns a pointer to the string following the Delimit, or 0 if Delimit is not found
	//      When Delimit is found its set to 0
	static STRING* Split(STRING* String, STRING Delimit)
	{
		return capi_StrSplit(String, Delimit);
	}

	//  Reverse a string
	//      String [Null-terminated source string to reverse]
	static void Reverse(STRING* String)
	{
		capi_StrReverse(String);
	}

	struct Encoding
	{
		struct _ASCII_
		{
			//  Get the length of a ASCII string
			//      String [Pointer to a null-terminated string]
			//  returns the number of characters in the string, not including the terminating null character
			//      -1 is returned for an invalid parameter
			//      To get the size of the string in bytes, multiply the result by sizeof(ASCII)
			static size_t Length(const ASCII* String)
			{
				return capi_StrLenA(String);
			}

			//  Copy a ASCII string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in ASCII units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units copied to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Copy(ASCII* Destination, size_t Length, const ASCII* Source)
			{
				return capi_StrCopyA(Destination, Length, Source);
			}

			//  Append a ASCII string
			//      Destination [Null-terminated destination string buffer]
			//      Length [Length of the destination string buffer in ASCII units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units appended to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Append(ASCII* Destination, size_t Length, const ASCII* Source)
			{
				return capi_StrAppendA(Destination, Length, Source);
			}

			//  Compare ASCII strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 Compare(const ASCII* String1, const ASCII* String2)
			{
				return capi_StrCompareA(String1, String2);
			}

			//  Perform a case-insensitive comparison of ASCII strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 CompareInsensitive(const ASCII* String1, const ASCII* String2)
			{
				return capi_StrCompareInsensitiveA(String1, String2);
			}

			//  Find a character in a ASCII string
			//      String [Null-terminated source string to search]
			//      Delimit [Character to be located]
			//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
			static ASCII* Find(const ASCII* String, ASCII Delimit)
			{
				return capi_StrFindA(String, Delimit);
			}

			//  Find a ASCII string in a ASCII string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static ASCII* FindStr(const ASCII* String, const ASCII* StrDelimit)
			{
				return capi_StrFindStrA(String, StrDelimit);
			}

			//  Perform a case-insensitive search for a ASCII string in a ASCII string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static ASCII* FindStrInsensitive(const ASCII* String, const ASCII* StrDelimit)
			{
				return capi_StrFindStrInsensitiveA(String, StrDelimit);
			}

			//  Split a ASCII string
			//      String [Null-terminated source string to split]
			//      Delimit [Character to be located and replaced]
			//  returns a pointer to the string following the Delimit, or 0 if Delimit is not found
			//      When Delimit is found its set to 0
			static ASCII* Split(ASCII* String, ASCII Delimit)
			{
				return capi_StrSplitA(String, Delimit);
			}

			//  Reverse a ASCII string
			//      String [Null-terminated source string to reverse]
			static void Reverse(ASCII* String)
			{
				capi_StrReverseA(String);
			}
		};
		struct _UTF8_
		{
			//  Get the number of bytes (units) a UTF8 code-point encoding uses
			//      Code [The 1st UTF8 unit of the code-point encoding]
			//  returns the number of UTF8 units the code-point encoding uses, or 0 for an error
			static U8 GetCharUnits(UTF8 Code)
			{
				return capi_UTF8_GetCharUnits(Code);
			}

			//  Encode a code-point using the UTF8 encoding
			//      String [The destination string buffer]
			//      CodePoint [The code-point to encode]
			//  returns the number of UTF8 units that was written to String, or 0 for an error
			//      This function does not check if String is valid and does not output any null-terminator character
			//      Consider using the safer version String::Encoding::_UTF8_::Encode
			static U8 Encode_Unsafe(UTF8* String, U32 CodePoint)
			{
				return capi_UTF8_Encode_Unsafe(String, CodePoint);
			}

			//  Encode a code-point using the UTF8 encoding
			//      String [The destination string buffer]
			//      Length [Length of the destination string buffer in UTF8 units]
			//      CodePoint [The code-point to encode]
			//  returns the number of UTF8 units that was written to String, not including the terminating null character, or 0 for an error
			//      This function does not check if String is valid
			static U8 Encode(UTF8* String, size_t Length, U32 CodePoint)
			{
				return capi_UTF8_Encode(String, Length, CodePoint);
			}

			//  Decode a UTF8 encoding into a code-point
			//      Units [The number of UTF8 units the code-point uses] Use String::Encoding::_UTF8_::GetCharUnits to get this value
			//      String [The source string to decode a code-point from]
			//  returns the decoded code-point, or 0 for an error
			//      This function does not check if String is valid
			static U32 Decode(U8 Units, const UTF8* String)
			{
				return capi_UTF8_Decode(Units, String);
			}

			//  Get the length of a UTF8 string
			//      String [Pointer to a null-terminated string]
			//  returns the number of characters in the string, not including the terminating null character
			//      -1 is returned for an invalid parameter
			static size_t Length(const UTF8* String)
			{
				return capi_StrLenU(String);
			}

			//  Copy a UTF8 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF8 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units copied to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Copy(UTF8* Destination, size_t Length, const UTF8* Source)
			{
				return capi_StrCopyU(Destination, Length, Source);
			}

			//  Append a UTF8 string
			//      Destination [Null-terminated destination string buffer]
			//      Length [Length of the destination string buffer in UTF8 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units appended to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Append(UTF8* Destination, size_t Length, const UTF8* Source)
			{
				return capi_StrAppendU(Destination, Length, Source);
			}

			//  Compare UTF8 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 Compare(const UTF8* String1, const UTF8* String2)
			{
				return capi_StrCompareU(String1, String2);
			}

			//  Perform a case-insensitive comparison of UTF8 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 CompareInsensitive(const UTF8* String1, const UTF8* String2)
			{
				return capi_StrCompareInsensitiveU(String1, String2);
			}

			//  Find a character in a UTF8 string
			//      String [Null-terminated source string to search]
			//      Delimit [Character to be located]
			//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
			static UTF8* Find(const UTF8* String, UTF8 Delimit)
			{
				return capi_StrFindU(String, Delimit);
			}

			//  Find a UTF8 string in a UTF8 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF8* FindStr(const UTF8* String, const UTF8* StrDelimit)
			{
				return capi_StrFindStrU(String, StrDelimit);
			}

			//  Perform a case-insensitive search for a UTF8 string in a UTF8 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF8* FindStrInsensitive(const UTF8* String, const UTF8* StrDelimit)
			{
				return capi_StrFindStrInsensitiveU(String, StrDelimit);
			}

			//  Split a UTF8 string
			//      String [Null-terminated source string to split]
			//      Delimit [Character to be located and replaced]
			//  returns a pointer to the string following the Delimit, or 0 if Delimit is not found
			//      When Delimit is found its set to 0
			static UTF8* Split(UTF8* String, UTF8 Delimit)
			{
				return capi_StrSplitU(String, Delimit);
			}

			//  Reverse a UTF8 string
			//      String [Null-terminated source string to reverse]
			static void Reverse(UTF8* String)
			{
				capi_StrReverseU(String);
			}

			//  Convert a UTF8 string to a UTF16 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF16 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF16 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF16(UTF16* Destination, size_t Length, const UTF8* Source)
			{
				return capi_UTF8_To_UTF16(Destination, Length, Source);
			}

			//  Convert a UTF8 string to a UTF32 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF32 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF32 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF32(UTF32* Destination, size_t Length, const UTF8* Source)
			{
				return capi_UTF8_To_UTF32(Destination, Length, Source);
			}
		};
		struct _UTF16_
		{
			//  Get the number of words (units) a UTF16 code-point encoding uses
			//      Code [The 1st UTF16 unit of the code-point encoding]
			//  returns the number of UTF16 units the code-point encoding uses, or 0 for an error
			static U8 GetCharUnits(UTF16 Code)
			{
				return capi_UTF16_GetCharUnits(Code);
			}

			//  Encode a code-point using the UTF16 encoding
			//      String [The destination string buffer]
			//      CodePoint [The code-point to encode]
			//  returns the number of UTF16 units that was written to String, or 0 for an error
			//      This function does not check if String is valid and does not output any null-terminator character
			//      Consider using the safer version String::Encoding::_UTF16_::Encode
			static U8 Encode_Unsafe(UTF16* String, U32 CodePoint)
			{
				return capi_UTF16_Encode_Unsafe(String, CodePoint);
			}

			//  Encode a code-point using the UTF16 encoding
			//      String [The destination string buffer]
			//      Length [Length of the destination string buffer in UTF16 units]
			//      CodePoint [The code-point to encode]
			//  returns the number of UTF16 units that was written to String, not including the terminating null character, or 0 for an error
			//      This function does not check if String is valid
			static U8 Encode(UTF16* String, size_t Length, U32 CodePoint)
			{
				return capi_UTF16_Encode(String, Length, CodePoint);
			}

			//  Decode a UTF16 encoding into a code-point
			//      Units [The number of UTF16 units the code-point uses] Use String::Encoding::_UTF8_::GetCharUnits to get this value
			//      String [The source string to decode a code-point from]
			//  returns the decoded code-point, or 0 for an error
			//      This function does not check if String is valid
			static U32 Decode(U8 Units, const UTF16* String)
			{
				return capi_UTF16_Decode(Units, String);
			}

			//  Get the length of a UTF16 string
			//      String [Pointer to a null-terminated string]
			//  returns the number of characters in the string, not including the terminating null character
			//      -1 is returned for an invalid parameter
			static size_t Length(const UTF16* String)
			{
				return capi_StrLenW(String);
			}

			//  Copy a UTF16 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF16 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units copied to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Copy(UTF16* Destination, size_t Length, const UTF16* Source)
			{
				return capi_StrCopyW(Destination, Length, Source);
			}

			//  Append a UTF16 string
			//      Destination [Null-terminated destination string buffer]
			//      Length [Length of the destination string buffer in UTF16 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units appended to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Append(UTF16* Destination, size_t Length, const UTF16* Source)
			{
				return capi_StrAppendW(Destination, Length, Source);
			}

			//  Compare UTF16 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 Compare(const UTF16* String1, const UTF16* String2)
			{
				return capi_StrCompareW(String1, String2);
			}

			//  Perform a case-insensitive comparison of UTF16 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 CompareInsensitive(const UTF16* String1, const UTF16* String2)
			{
				return capi_StrCompareInsensitiveW(String1, String2);
			}

			//  Find a character in a UTF16 string
			//      String [Null-terminated source string to search]
			//      Delimit [Character to be located]
			//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
			static UTF16* Find(const UTF16* String, UTF16 Delimit)
			{
				return capi_StrFindW(String, Delimit);
			}

			//  Find a UTF16 string in a UTF16 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF16* FindStr(const UTF16* String, const UTF16* StrDelimit)
			{
				return capi_StrFindStrW(String, StrDelimit);
			}

			//  Perform a case-insensitive search for a UTF16 string in a UTF16 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF16* FindStrInsensitive(const UTF16* String, const UTF16* StrDelimit)
			{
				return capi_StrFindStrInsensitiveW(String, StrDelimit);
			}

			//  Split a UTF16 string
			//      String [Null-terminated source string to split]
			//      Delimit [Character to be located and replaced]
			//  returns a pointer to the string following the Delimit, or 0 if Delimit is not found
			//      When Delimit is found its set to 0
			static UTF16* Split(UTF16* String, UTF16 Delimit)
			{
				return capi_StrSplitW(String, Delimit);
			}

			//  Reverse a UTF16 string
			//      String [Null-terminated source string to reverse]
			static void Reverse(UTF16* String)
			{
				capi_StrReverseW(String);
			}

			//  Convert a UTF16 string to a UTF8 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF8 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF8 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF8(UTF8* Destination, size_t Length, const UTF16* Source)
			{
				return capi_UTF16_To_UTF8(Destination, Length, Source);
			}

			//  Convert a UTF16 string to a UTF32 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF32 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF32 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF32(UTF32* Destination, size_t Length, const UTF16* Source)
			{
				return capi_UTF16_To_UTF32(Destination, Length, Source);
			}
		};
		struct _UTF32_
		{
			//  Get the length of a UTF32 string
			//      String [Pointer to a null-terminated string]
			//  returns the number of characters in the string, not including the terminating null character
			//      -1 is returned for an invalid parameter
			static size_t Length(const UTF32* String)
			{
				return capi_StrLenL(String);
			}

			//  Copy a UTF32 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF32 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units copied to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Copy(UTF32* Destination, size_t Length, const UTF32* Source)
			{
				return capi_StrCopyL(Destination, Length, Source);
			}

			//  Append a UTF32 string
			//      Destination [Null-terminated destination string buffer]
			//      Length [Length of the destination string buffer in UTF32 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of units appended to the destination string, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t Append(UTF32* Destination, size_t Length, const UTF32* Source)
			{
				return capi_StrAppendL(Destination, Length, Source);
			}

			//  Compare UTF32 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 Compare(const UTF32* String1, const UTF32* String2)
			{
				return capi_StrCompareL(String1, String2);
			}

			//  Perform a case-insensitive comparison of UTF32 strings
			//      String1 [Null-terminated string to compare]
			//      String2 [Null-terminated string to compare]
			//  The return value indicates the ordinal relation of String1 to String2
			//      < 0  String1 is less than String2
			//        0  String1 is identical to String2
			//      > 0  String1 is greater than String2
			//      0x7FFFFFFF is returned for an invalid parameter
			static I32 CompareInsensitive(const UTF32* String1, const UTF32* String2)
			{
				return capi_StrCompareInsensitiveL(String1, String2);
			}

			//  Find a character in a UTF32 string
			//      String [Null-terminated source string to search]
			//      Delimit [Character to be located]
			//  returns a pointer to the first occurrence of Delimit in String, or 0 if Delimit is not found
			static UTF32* Find(const UTF32* String, UTF32 Delimit)
			{
				return capi_StrFindL(String, Delimit);
			}

			//  Find a UTF32 string in a UTF32 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF32* FindStr(const UTF32* String, const UTF32* StrDelimit)
			{
				return capi_StrFindStrL(String, StrDelimit);
			}

			//  Perform a case-insensitive search for a UTF32 string in a UTF32 string
			//      String [Null-terminated source string to search]
			//      StrDelimit [String to be located]
			//  returns a pointer to the first occurrence of StrDelimit in String, or 0 if StrDelimit is not found
			static UTF32* FindStrInsensitive(const UTF32* String, const UTF32* StrDelimit)
			{
				return capi_StrFindStrInsensitiveL(String, StrDelimit);
			}

			//  Split a UTF32 string
			//      String [Null-terminated source string to split]
			//      Delimit [Character to be located and replaced]
			//  returns a pointer to the string following the Delimit, or 0 if Delimit is not found
			//      When Delimit is found its set to 0
			static UTF32* Split(UTF32* String, UTF32 Delimit)
			{
				return capi_StrSplitL(String, Delimit);
			}

			//  Reverse a UTF32 string
			//      String [Null-terminated source string to reverse]
			static void Reverse(UTF32* String)
			{
				capi_StrReverseL(String);
			}

			//  Convert a UTF32 string to a UTF8 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF8 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF8 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF8(UTF8* Destination, size_t Length, const UTF32* Source)
			{
				return capi_UTF32_To_UTF8(Destination, Length, Source);
			}

			//  Convert a UTF32 string to a UTF16 string
			//      Destination [Pointer to the destination string buffer]
			//      Length [Length of the destination string buffer in UTF16 units]
			//      Source [Null-terminated source string buffer]
			//  returns the number of characters converted to UTF16 and put into the Destination buffer, not including the terminating null character
			//      If there is no null terminator within Length, then Length is returned to indicate the error condition
			//      -1 is returned for an invalid parameter
			static size_t To_UTF16(UTF16* Destination, size_t Length, const UTF32* Source)
			{
				return capi_UTF32_To_UTF16(Destination, Length, Source);
			}
		};
	};
};
#endif

#endif /* CAPI_H */
