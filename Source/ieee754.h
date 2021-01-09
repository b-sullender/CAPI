
/******************************************************************************************
*
* Name: ieee754.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares IEEE 754 functions.
*
*******************************************************************************************/

#ifndef IEEE754_H
#define IEEE754_H

#include "CAPI.h"

CAPI_SUBFUNC(void) ieee754_SetValueFromSHL(U32* pDestination, U64 Value, I32 Shift, I32 Adjustment, U32 Count);

#endif /* IEEE754_H */
