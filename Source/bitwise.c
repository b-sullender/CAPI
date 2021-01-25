
/******************************************************************************************
*
* Name: bitwise.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines bitwise functions for variables that have a large bit length.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(void) capi_Negate(size_t* pValue, size_t Count)
{
	size_t I, CF;

	CF = 1;

	for (I = 0; I < Count; I++)
	{
		pValue[I] = ~pValue[I];
		if (CF == 1)
		{
			if (pValue[I] != -1)
			{
				pValue[I]++;
				CF = 0;
			}
			else
			{
				pValue[I]++;
				CF = 1;
			}
		}
	}
}

CAPI_FUNC(size_t) capi_CountLZ(const size_t* pValue, size_t Count)
{
	ptrdiff_t I;
	size_t B, LzCnt;

	LzCnt = 0;

	for (I = Count - 1; I >= 0; I--)
	{
		if (pValue[I] == 0)
		{
			LzCnt += CAPI_BIT_LENGTH;
			continue;
		}
		else
		{
			for (B = 1LL << (CAPI_BIT_LENGTH - 1); B != 0; B >>= 1)
			{
				if (pValue[I] & B) return LzCnt;
				LzCnt++;
			}
		}
	}

	return LzCnt;
}

CAPI_FUNC(size_t) capi_CountL1(const size_t* pValue, size_t Count)
{
	ptrdiff_t I;
	size_t B, L1Cnt;

	L1Cnt = 0;

	for (I = Count - 1; I >= 0; I--)
	{
		if (pValue[I] == (~0LL))
		{
			L1Cnt += CAPI_BIT_LENGTH;
			continue;
		}
		else
		{
			for (B = 1LL << (CAPI_BIT_LENGTH - 1); B != 0; B >>= 1)
			{
				if ((pValue[I] & B) == 0) return L1Cnt;
				L1Cnt++;
			}
		}
	}

	return L1Cnt;
}

CAPI_FUNC(size_t) capi_CountTZ(const size_t* pValue, size_t Count)
{
	size_t I, B, TzCnt;

	TzCnt = 0;

	for (I = 0; I < Count; I++)
	{
		if (pValue[I] == 0)
		{
			TzCnt += CAPI_BIT_LENGTH;
			continue;
		}
		else
		{
			for (B = 1; B != 0; B <<= 1)
			{
				if (pValue[I] & B) return TzCnt;
				TzCnt++;
			}
		}
	}

	return TzCnt;
}

CAPI_FUNC(size_t) capi_CountT1(const size_t* pValue, size_t Count)
{
	size_t I, B, T1Cnt;

	T1Cnt = 0;

	for (I = 0; I < Count; I++)
	{
		if (pValue[I] == (~0LL))
		{
			T1Cnt += CAPI_BIT_LENGTH;
			continue;
		}
		else
		{
			for (B = 1; B != 0; B <<= 1)
			{
				if ((pValue[I] & B) == 0) return T1Cnt;
				T1Cnt++;
			}
		}
	}

	return T1Cnt;
}
