
/******************************************************************************************
*
* Name: ieee754.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines IEEE 754 functions.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_SUBFUNC(void) ieee754_SetValueFromSHL(U32* pDestination, U64 Value, I32 Shift, I32 Adjustment, U32 Count)
{
	U32 Index, SubShift;
	U64 HighPart;

	Index = Shift >> 5;
	SubShift = Shift & 31;

	Index += Adjustment;

	HighPart = Value >> (64 - SubShift);
	Value <<= SubShift;

	Index += 2;
	if (Index < Count)
	{
		pDestination[Index] = (U32)HighPart;
	}
	Index -= 1;
	if (Index < Count)
	{
		pDestination[Index] = (Value >> 32);
	}
	Index -= 1;

	if (Index < Count)
	{
		pDestination[Index] = (U32)Value;
	}
}
