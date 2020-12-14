
/******************************************************************************************
*
* Name: error.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines error functions.
*
*******************************************************************************************/

#include "CAPI.h"

static const char str_ERROR_IMAGE_DATA_INCOMPLETE_A[] = "ERROR_IMAGE_DATA_INCOMPLETE";
static const char str_ERROR_JPEG_FAILED_A[] = "ERROR_JPEG_FAILED";
static const char str_ERROR_ZLIB_FAILED_A[] = "ERROR_ZLIB_FAILED";
static const char str_ERROR_INVALID_ADLER32_A[] = "ERROR_INVALID_ADLER32";
static const char str_ERROR_INVALID_CRC32_A[] = "ERROR_INVALID_CRC32";
static const char str_ERROR_ACCESS_DENIED_A[] = "ERROR_ACCESS_DENIED";
static const char str_ERROR_NONE_A[] = "ERROR NONE";
static const char str_ERROR_INVALID_PARAMETER_A[] = "ERROR_INVALID_PARAMETER";
static const char str_ERROR_UNIMPLEMENTED_FEATURE_A[] = "ERROR_UNIMPLEMENTED_FEATURE";
static const char str_ERROR_STRING_TOO_LONG_A[] = "ERROR_STRING_TOO_LONG";
static const char str_ERROR_STRING_TOO_SHORT_A[] = "ERROR_STRING_TOO_SHORT";
static const char str_ERROR_OUT_OF_MEMORY_A[] = "ERROR_OUT_OF_MEMORY";
static const char str_ERROR_INVALID_FILE_FORMAT_A[] = "ERROR_INVALID_FILE_FORMAT";

static const wchar_t str_ERROR_IMAGE_DATA_INCOMPLETE_W[] = L"ERROR_IMAGE_DATA_INCOMPLETE";
static const wchar_t str_ERROR_JPEG_FAILED_W[] = L"ERROR_JPEG_FAILED";
static const wchar_t str_ERROR_ZLIB_FAILED_W[] = L"ERROR_ZLIB_FAILED";
static const wchar_t str_ERROR_INVALID_ADLER32_W[] = L"ERROR_INVALID_ADLER32";
static const wchar_t str_ERROR_INVALID_CRC32_W[] = L"ERROR_INVALID_CRC32";
static const wchar_t str_ERROR_ACCESS_DENIED_W[] = L"ERROR_ACCESS_DENIED";
static const wchar_t str_ERROR_NONE_W[] = L"ERROR NONE";
static const wchar_t str_ERROR_INVALID_PARAMETER_W[] = L"ERROR_INVALID_PARAMETER";
static const wchar_t str_ERROR_UNIMPLEMENTED_FEATURE_W[] = L"ERROR_UNIMPLEMENTED_FEATURE";
static const wchar_t str_ERROR_STRING_TOO_LONG_W[] = L"ERROR_STRING_TOO_LONG";
static const wchar_t str_ERROR_STRING_TOO_SHORT_W[] = L"ERROR_STRING_TOO_SHORT";
static const wchar_t str_ERROR_OUT_OF_MEMORY_W[] = L"ERROR_OUT_OF_MEMORY";
static const wchar_t str_ERROR_INVALID_FILE_FORMAT_W[] = L"ERROR_INVALID_FILE_FORMAT";

CAPI_FUNC(const char*) capi_ErrorCodeToStringA(I32 ErrorCode)
{
	switch (ErrorCode)
	{
	case CAPI_ERROR_IMAGE_DATA_INCOMPLETE:
		return str_ERROR_IMAGE_DATA_INCOMPLETE_A;
	case CAPI_ERROR_JPEG_FAILED:
		return str_ERROR_JPEG_FAILED_A;
	case CAPI_ERROR_ZLIB_FAILED:
		return str_ERROR_ZLIB_FAILED_A;
	case CAPI_ERROR_INVALID_ADLER32:
		return str_ERROR_INVALID_ADLER32_A;
	case CAPI_ERROR_INVALID_CRC32:
		return str_ERROR_INVALID_CRC32_A;
	case CAPI_ERROR_ACCESS_DENIED:
		return str_ERROR_ACCESS_DENIED_A;
	case CAPI_ERROR_NONE:
		return str_ERROR_NONE_A;
	case CAPI_ERROR_INVALID_PARAMETER:
		return str_ERROR_INVALID_PARAMETER_A;
	case CAPI_ERROR_UNIMPLEMENTED_FEATURE:
		return str_ERROR_UNIMPLEMENTED_FEATURE_A;
	case CAPI_ERROR_STRING_TOO_LONG:
		return str_ERROR_STRING_TOO_LONG_A;
	case CAPI_ERROR_STRING_TOO_SHORT:
		return str_ERROR_STRING_TOO_SHORT_A;
	case CAPI_ERROR_OUT_OF_MEMORY:
		return str_ERROR_OUT_OF_MEMORY_A;
	case CAPI_ERROR_INVALID_FILE_FORMAT:
		return str_ERROR_INVALID_FILE_FORMAT_A;
	default:
		break;
	}

	return 0;
}

CAPI_FUNC(const wchar_t*) capi_ErrorCodeToStringW(I32 ErrorCode)
{
	switch (ErrorCode)
	{
	case CAPI_ERROR_IMAGE_DATA_INCOMPLETE:
		return str_ERROR_IMAGE_DATA_INCOMPLETE_W;
	case CAPI_ERROR_JPEG_FAILED:
		return str_ERROR_JPEG_FAILED_W;
	case CAPI_ERROR_ZLIB_FAILED:
		return str_ERROR_ZLIB_FAILED_W;
	case CAPI_ERROR_INVALID_ADLER32:
		return str_ERROR_INVALID_ADLER32_W;
	case CAPI_ERROR_INVALID_CRC32:
		return str_ERROR_INVALID_CRC32_W;
	case CAPI_ERROR_ACCESS_DENIED:
		return str_ERROR_ACCESS_DENIED_W;
	case CAPI_ERROR_NONE:
		return str_ERROR_NONE_W;
	case CAPI_ERROR_INVALID_PARAMETER:
		return str_ERROR_INVALID_PARAMETER_W;
	case CAPI_ERROR_UNIMPLEMENTED_FEATURE:
		return str_ERROR_UNIMPLEMENTED_FEATURE_W;
	case CAPI_ERROR_STRING_TOO_LONG:
		return str_ERROR_STRING_TOO_LONG_W;
	case CAPI_ERROR_STRING_TOO_SHORT:
		return str_ERROR_STRING_TOO_SHORT_W;
	case CAPI_ERROR_OUT_OF_MEMORY:
		return str_ERROR_OUT_OF_MEMORY_W;
	case CAPI_ERROR_INVALID_FILE_FORMAT:
		return str_ERROR_INVALID_FILE_FORMAT_W;
	default:
		break;
	}

	return 0;
}
