
/******************************************************************************************
*
* Name: compare.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines compare functions for variables that have a large bit length.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(size_t) capi_TestForZero(const size_t* pData, size_t Count)
{
	size_t I, Result;

	Result = 0;

	for (I = 0; I < Count; I++)
	{
		Result |= pData[I];
		if (Result != 0) break;
	}

	return Result;
}

CAPI_FUNC(I8) capi_Compare(const size_t* pOperand1, const size_t* pOperand2, size_t Count)
{
	ptrdiff_t I;

	for (I = Count - 1; I >= 0; I--)
	{
		if (pOperand1[I] > pOperand2[I]) return 1;
		if (pOperand1[I] < pOperand2[I]) return -1;
	}

	return 0;
}

CAPI_FUNC(I8) capi_iCompare(const size_t* pOperand1, const size_t* pOperand2, size_t Count)
{
	ptrdiff_t I;
	I8 Op1Sign, Op2Sign;

	I = Count - 1;

	Op1Sign = pOperand1[I] >> (CAPI_BIT_LENGTH - 1);
	Op2Sign = pOperand2[I] >> (CAPI_BIT_LENGTH - 1);

	if ((Op1Sign == 0) && (Op2Sign == 1)) return 1;
	if ((Op1Sign == 1) && (Op2Sign == 0)) return -1;

	for (; I >= 0; I--)
	{
		if (pOperand1[I] > pOperand2[I]) return 1;
		if (pOperand1[I] < pOperand2[I]) return -1;
	}

	return 0;
}
