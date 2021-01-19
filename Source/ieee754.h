
/******************************************************************************************
*
* Name: ieee754.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares IEEE 754 structures and functions.
*
*******************************************************************************************/

#ifndef IEEE754_H
#define IEEE754_H

#include "CAPI.h"

#if CAPI_BIT_LENGTH == 32
#define ieee754_NUM_MAX_TENS 9
#define ieee754_MAX_TENS 1000000000
#elif CAPI_BIT_LENGTH == 64
#define ieee754_NUM_MAX_TENS 18
#define ieee754_MAX_TENS 1000000000000000000
#endif

PACK(STRUCT(ieee754_NUM512)
{
	size_t Num512[512 / CAPI_BIT_LENGTH];
});

PACK(STRUCT(ieee754_NUM3584)
{
	size_t Num3584[3584 / CAPI_BIT_LENGTH];
});

//  ieee754_Init_Tables - Generate internal ieee 754 lookup tables (ieee754.c)
//  returns CAPI_ERROR_NONE on success
//      This functions allocates 9536 bytes (9.3125 KB) for the single fixed point lookup table,
//        and 481152 bytes (469.875 KB) for the double fixed point lookup table.
CAPI_SUBFUNC(I32) ieee754_Init_Tables();

CAPI_SUBFUNC(U64) ieee754_GetValueFromSHR(size_t* pValue, U32 Shift, U32 Count);

CAPI_SUBFUNC(void) ieee754_SetValueFromSHL(U32* pDestination, U64 Value, I32 Shift, I32 Adjustment, U32 Count);

CAPI_SUBFUNC(U32) ieee754_GetIntPartFromSingle(U32 Value);

CAPI_SUBFUNC(U64) ieee754_GetIntPartFromDouble(U64 Value);

CAPI_SUBFUNC(void) ieee754_GetFixedFracFromSingle(size_t* pResult, U32 Value);

CAPI_SUBFUNC(void) ieee754_GetFixedFracFromDouble(size_t* pResult, U64 Value);

#endif /* IEEE754_H */
