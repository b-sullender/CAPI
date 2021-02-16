
/******************************************************************************************
*
* Name: extend.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines extend functions for variables that have a large bit length.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(void) capi_ZeroExtend(size_t* pDestination, size_t Count, const void* pSource, size_t nBytes)
{
	capi_memcopy(pDestination, pSource, nBytes);
	capi_memset(&((U8*)pDestination)[nBytes], 0, (Count * sizeof(size_t)) - nBytes);
}

CAPI_FUNC(void) capi_SignExtend(size_t* pDestination, size_t Count, const void* pSource, size_t nBytes)
{
	capi_memcopy(pDestination, pSource, nBytes);

	if (((U8*)pDestination)[nBytes - 1] >> 7)
	{
		capi_memset(&((U8*)pDestination)[nBytes], 0xFF, (Count * sizeof(size_t)) - nBytes);
	}
	else
	{
		capi_memset(&((U8*)pDestination)[nBytes], 0, (Count * sizeof(size_t)) - nBytes);
	}
}
