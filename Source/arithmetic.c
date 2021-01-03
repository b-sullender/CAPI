
/******************************************************************************************
*
* Name: arithmetic.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines arithmetic functions for variables that have a large bit length.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(size_t) capi_AddReturnCarry(size_t* pAugend, size_t Addend, size_t Count)
{
	size_t I, CF;

	if ((pAugend[0] + Addend) < pAugend[0]) CF = 1;
	else CF = 0;

	pAugend[0] += Addend;

	for (I = 1; I < Count; I++)
	{
		if (CF == 0) break;
		else
		{
			if (pAugend[I] != -1LL) CF = 0;
			pAugend[I]++;
		}
	}

	return CF;
}

CAPI_FUNC(size_t) capi_AddReturnCarryEx(size_t* pAugend, size_t* pAddend, size_t Count)
{
	size_t I, CF, NextCF;

	CF = 0;

	for (I = 0; I < Count; I++)
	{
		NextCF = 0;

		if ((pAugend[I] + pAddend[I]) < pAugend[I]) NextCF = 1;
		pAugend[I] += pAddend[I];

		if ((pAugend[I] + CF) < pAugend[I]) NextCF = 1;
		pAugend[I] += CF;

		CF = NextCF;
	}

	return CF;
}

CAPI_FUNC(size_t) capi_SubReturnBorrow(size_t* pMinuend, size_t Subtrahend, size_t Count)
{
	size_t I, CF;

	if ((pMinuend[0] - Subtrahend) > pMinuend[0]) CF = 1;
	else CF = 0;

	pMinuend[0] -= Subtrahend;

	for (I = 1; I < Count; I++)
	{
		if (CF == 0) break;
		else
		{
			if (pMinuend[I] != 0) CF = 0;
			pMinuend[I]--;
		}
	}

	return CF;
}

CAPI_FUNC(size_t) capi_SubReturnBorrowEx(size_t* pMinuend, size_t* pSubtrahend, size_t Count)
{
	size_t I, CF, NextCF;

	CF = 0;

	for (I = 0; I < Count; I++)
	{
		NextCF = 0;

		if ((pMinuend[I] - pSubtrahend[I]) > pMinuend[I]) NextCF = 1;
		pMinuend[I] -= pSubtrahend[I];

		if ((pMinuend[I] - CF) > pMinuend[I]) NextCF = 1;
		pMinuend[I] -= CF;

		CF = NextCF;
	}

	return CF;
}

#if CAPI_BIT_LENGTH == 32
#define EXTENDED_SIZE U64

CAPI_FUNC(size_t) capi_DivReturnRemainder(size_t* pDividend, size_t Divisor, size_t Count)
{
	ptrdiff_t I;
	EXTENDED_SIZE ExDividend;
	size_t Remainder;

	ExDividend = Remainder = 0;

	for (I = Count - 1; I >= 0; I--)
	{
		ExDividend = Remainder;
		ExDividend <<= CAPI_BIT_LENGTH;
		ExDividend |= pDividend[I];
		pDividend[I] = (size_t)(ExDividend / Divisor);
		Remainder = ExDividend % Divisor;
	}

	return Remainder;
}

CAPI_FUNC(size_t) capi_MulReturnOverflow(size_t* pMultiplicand, size_t Multiplier, size_t Count)
{
	size_t I, Overflow;
	EXTENDED_SIZE Product;

	Overflow = 0;

	for (I = 0; I < Count; I++)
	{
		Product = (EXTENDED_SIZE)pMultiplicand[I] * (EXTENDED_SIZE)Multiplier;
		Product += Overflow;
		pMultiplicand[I] = (size_t)Product;
		Overflow = (size_t)(Product >> CAPI_BIT_LENGTH);
	}

	return Overflow;
}

#elif CAPI_BIT_LENGTH == 64
#define EXTENDED_SIZE U128

CAPI_FUNC(size_t) capi_DivReturnRemainder(size_t* pDividend, size_t Divisor, size_t Count)
{
	ptrdiff_t I;
	EXTENDED_SIZE ExDividend, ExDivisor, ExRemainder;
	size_t Remainder;

	ExDivisor.Lo = Divisor;
	ExDivisor.Hi = ExDividend.Lo = ExDividend.Hi = Remainder = 0;

	for (I = Count - 1; I >= 0; I--)
	{
		ExDividend.Hi = Remainder;
		ExDividend.Lo = pDividend[I];
		capi_dvrm128(&ExDividend, &ExRemainder, &ExDividend, &ExDivisor);
		pDividend[I] = ExDividend.Lo;
		Remainder = ExRemainder.Lo;
	}

	return Remainder;
}

CAPI_FUNC(size_t) capi_MulReturnOverflow(size_t* pMultiplicand, size_t Multiplier, size_t Count)
{
	size_t I, Overflow;
	EXTENDED_SIZE Product;

	Overflow = 0;

	for (I = 0; I < Count; I++)
	{
		capi_mul64(&Product, pMultiplicand[I], Multiplier);
		capi_AddReturnCarry((size_t*)&Product, Overflow, 128 / CAPI_BIT_LENGTH);
		pMultiplicand[I] = (size_t)Product.Lo;
		Overflow = (size_t)Product.Hi;
	}

	return Overflow;
}

#endif
