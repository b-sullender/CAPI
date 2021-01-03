
/******************************************************************************************
*
* Name: int64.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines 64-bit operations.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(void) capi_mul64(U128* pResult, U64 A, U64 B)
{
	U64 Temp;

	// LO(A) * LO(B)

	pResult->Lo = ((A & 0xFFFFFFFF) * (B & 0xFFFFFFFF));

	// LO(A) * HI(B)

	Temp = ((A & 0xFFFFFFFF) * (B >> 32));
	Temp += (pResult->Lo >> 32);
	pResult->Lo = (pResult->Lo & 0xFFFFFFFF) | (Temp << 32);
	pResult->Hi = Temp >> 32;

	// HI(A) * HI(B)

	pResult->Hi += ((A >> 32) * (B >> 32));

	// HI(A) * LO(B)

	Temp = ((A >> 32) * (B & 0xFFFFFFFF));
	if ((pResult->Lo + (Temp << 32)) < pResult->Lo) pResult->Hi++;
	pResult->Lo += (Temp << 32);
	pResult->Hi += (Temp >> 32);
}

CAPI_FUNC(void) capi_imul64(I128* pResult, I64 A, I64 B)
{
	capi_mul64((U128*)pResult, A, B);

	if (A < 0) pResult->Hi -= B;
	if (B < 0) pResult->Hi -= A;
}
