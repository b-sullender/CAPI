
/******************************************************************************************
*
* Name: version.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines version functions.
*
*******************************************************************************************/

#include "CAPI.h"

/*
*
* version 2.00.00 includes:
*     zlib-1.2.11
*     jpeg-9d
*
*/

static const char strVersionA[] = "2.00.00";
static const wchar_t strVersionW[] = L"2.00.00";

CAPI_FUNC(U32) capi_VersionA(const char** ppVersion)
{
	if (ppVersion != 0)
	{
		*ppVersion = strVersionA;
	}

	return 0x20000;
}

CAPI_FUNC(U32) capi_VersionW(const wchar_t** ppVersion)
{
	if (ppVersion != 0)
	{
		*ppVersion = strVersionW;
	}

	return 0x20000;
}
