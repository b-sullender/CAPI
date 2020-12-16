
/******************************************************************************************
*
* Name: memory.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines memory functions.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(void) capi_memset32(void* pDestination, U32 Value, size_t nDwords)
{
	size_t I;

	if (pDestination != 0)
	{
		for (I = 0; I < nDwords; I++)
		{
			((U32*)pDestination)[I] = Value;
		}
	}
}
