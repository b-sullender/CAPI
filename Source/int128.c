
/******************************************************************************************
*
* Name: int128.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines 128-bit operations.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(void) capi_shl128(U128* pValue, U32 N)
{
	if (N == 0) return;

	if (N < 64)
	{
		pValue->Hi <<= N;
		pValue->Hi |= pValue->Lo >> (64 - N);
		pValue->Lo <<= N;
	}
	else if (N >= 128)
	{
		pValue->Lo = 0;
		pValue->Hi = 0;
	}
	else if (N > 64)
	{
		pValue->Hi = pValue->Lo << (N & 63);
		pValue->Lo = 0;
	}
	else
	{
		pValue->Hi = pValue->Lo;
		pValue->Lo = 0;
	}
}

CAPI_FUNC(void) capi_shr128(U128* pValue, U32 N)
{
	if (N == 0) return;

	if (N < 64)
	{
		pValue->Lo >>= N;
		pValue->Lo |= pValue->Hi << (64 - N);
		pValue->Hi >>= N;
	}
	else if (N >= 128)
	{
		pValue->Lo = 0;
		pValue->Hi = 0;
	}
	else if (N > 64)
	{
		pValue->Lo = pValue->Hi >> (N & 63);
		pValue->Hi = 0;
	}
	else
	{
		pValue->Lo = pValue->Hi;
		pValue->Hi = 0;
	}
}

CAPI_FUNC(void) capi_sar128(I128* pValue, U32 N)
{
	if (N == 0) return;

	if (N < 64)
	{
		pValue->Lo >>= N;
		pValue->Lo |= pValue->Hi << (64 - N);
		pValue->Hi >>= N;
	}
	else if (N >= 128)
	{
		pValue->Lo = (pValue->Hi >> 63);
		pValue->Hi = pValue->Lo;
	}
	else if (N > 64)
	{
		pValue->Lo = (pValue->Hi >> (N & 63));
		pValue->Hi >>= 63;
	}
	else
	{
		pValue->Lo = pValue->Hi;
		pValue->Hi >>= 63;
	}
}

CAPI_FUNC(void) capi_div128(U128* pQuotient, U128* pDividend, U128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pQuotient->Lo = DD.Lo;
		pQuotient->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)pQuotient, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		return;
	}

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit divide.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pQuotient->Hi = 0;
		pQuotient->Lo = DD.Lo / DV.Lo;

		return;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Set the Quotient to zero.

	pQuotient->Lo = 0;
	pQuotient->Hi = 0;

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		capi_shl128(pQuotient, 1);

		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			pQuotient->Lo |= 1;
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);
}

CAPI_FUNC(void) capi_idiv128(I128* pQuotient, I128* pDividend, I128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;
	U8 Neg;

	Neg = TRUE;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// if the signs are the same then: Neg = FALSE;

	if ((DD.Hi & 0x8000000000000000) == (DV.Hi & 0x8000000000000000)) Neg = FALSE;

	// Covert Dividend and Divisor to possitive if negative: (negate)

	if (DD.Hi & 0x8000000000000000) capi_Negate((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	if (DV.Hi & 0x8000000000000000) capi_Negate((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pQuotient->Lo = DD.Lo;
		pQuotient->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)pQuotient, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		goto idiv128_negate;
	}

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit divide.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pQuotient->Hi = 0;
		pQuotient->Lo = DD.Lo / DV.Lo;

		goto idiv128_negate;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Set the Quotient to zero.

	pQuotient->Lo = 0;
	pQuotient->Hi = 0;

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		capi_shl128((U128*)pQuotient, 1);

		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			pQuotient->Lo |= 1;
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);

	// Last operation is negate if needed.

idiv128_negate:

	if (Neg == TRUE)
	{
		capi_Negate((size_t*)pQuotient, 128 / CAPI_BIT_LENGTH);
	}
}

CAPI_FUNC(void) capi_dvrm128(U128* pQuotient, U128* pRemainder, U128* pDividend, U128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

#if CAPI_BIT_LENGTH != 64 // capi_DivReturnRemainder in arithmetic.c uses capi_dvrm128 in 64-Bit mode

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pRemainder->Lo = pQuotient->Lo = DD.Lo;
		pRemainder->Hi = pQuotient->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)pQuotient, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		DD.Lo = pQuotient->Lo;
		DD.Hi = pQuotient->Hi;

		capi_MulReturnOverflow((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);
		capi_SubReturnBorrowEx((size_t*)pRemainder, (size_t*)&DD, 128 / CAPI_BIT_LENGTH);

		return;
	}

#endif

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit divide.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pQuotient->Hi = 0;
		pRemainder->Hi = 0;

		pQuotient->Lo = DD.Lo / DV.Lo;
		pRemainder->Lo = DD.Lo % DV.Lo;

		return;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Set the Quotient to zero.

	pQuotient->Lo = 0;
	pQuotient->Hi = 0;

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		capi_shl128(pQuotient, 1);

		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			pQuotient->Lo |= 1;
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);

	// DD has the Remainder.

	pRemainder->Lo = DD.Lo;
	pRemainder->Hi = DD.Hi;
}

CAPI_FUNC(void) capi_idvrm128(I128* pQuotient, I128* pRemainder, I128* pDividend, I128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;
	U8 Neg;

	Neg = TRUE;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// if the signs are the same then: Neg = FALSE;

	if ((DD.Hi & 0x8000000000000000) == (DV.Hi & 0x8000000000000000)) Neg = FALSE;

	// Covert Dividend and Divisor to possitive if negative: (negate)

	if (DD.Hi & 0x8000000000000000) capi_Negate((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	if (DV.Hi & 0x8000000000000000) capi_Negate((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pRemainder->Lo = pQuotient->Lo = DD.Lo;
		pRemainder->Hi = pQuotient->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)pQuotient, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		DD.Lo = pQuotient->Lo;
		DD.Hi = pQuotient->Hi;

		capi_MulReturnOverflow((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);
		capi_SubReturnBorrowEx((size_t*)pRemainder, (size_t*)&DD, 128 / CAPI_BIT_LENGTH);

		goto idvrm128_negate;
	}

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit divide.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pQuotient->Hi = 0;
		pRemainder->Hi = 0;

		pQuotient->Lo = DD.Lo / DV.Lo;
		pRemainder->Lo = DD.Lo % DV.Lo;

		goto idvrm128_negate;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Set the Quotient to zero.

	pQuotient->Lo = 0;
	pQuotient->Hi = 0;

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		capi_shl128((U128*)pQuotient, 1);

		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			pQuotient->Lo |= 1;
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);

	// DD has the Remainder.

	pRemainder->Lo = DD.Lo;
	pRemainder->Hi = DD.Hi;

	// Last operation is negate if needed.

idvrm128_negate:

	if (Neg == TRUE)
	{
		capi_Negate((size_t*)pQuotient, 128 / CAPI_BIT_LENGTH);
		capi_Negate((size_t*)pRemainder, 128 / CAPI_BIT_LENGTH);
	}
}

CAPI_FUNC(void) capi_rem128(U128* pRemainder, U128* pDividend, U128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pRemainder->Lo = DD.Lo;
		pRemainder->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		capi_MulReturnOverflow((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);
		capi_SubReturnBorrowEx((size_t*)pRemainder, (size_t*)&DD, 128 / CAPI_BIT_LENGTH);

		return;
	}

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit remainder.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pRemainder->Hi = 0;
		pRemainder->Lo = DD.Lo % DV.Lo;

		return;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);

	// DD has the Remainder.

	pRemainder->Lo = DD.Lo;
	pRemainder->Hi = DD.Hi;
}

CAPI_FUNC(void) capi_irem128(I128* pRemainder, I128* pDividend, I128* pDivisor)
{
	U128 DD, DV;
	U32 T1, T2, Shift;
	U8 Neg;

	Neg = TRUE;

	// Use local DD and DV so Dividend and Divisor dont get currupted.

	DV.Lo = pDivisor->Lo;
	DV.Hi = pDivisor->Hi;
	DD.Lo = pDividend->Lo;
	DD.Hi = pDividend->Hi;

	// if the signs are the same then: Neg = FALSE;

	if ((DD.Hi & 0x8000000000000000) == (DV.Hi & 0x8000000000000000)) Neg = FALSE;

	// Covert Dividend and Divisor to possitive if negative: (negate)

	if (DD.Hi & 0x8000000000000000) capi_Negate((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	if (DV.Hi & 0x8000000000000000) capi_Negate((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	// If bits 32-127 of the Divisor are zero then we can use a simple algorithm.

	if ((((U32*)&DV)[1] | ((U32*)&DV)[2] | ((U32*)&DV)[3]) == 0)
	{
		pRemainder->Lo = DD.Lo;
		pRemainder->Hi = DD.Hi;

		capi_DivReturnRemainder((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);

		capi_MulReturnOverflow((size_t*)&DD, (size_t)DV.Lo, 128 / CAPI_BIT_LENGTH);
		capi_SubReturnBorrowEx((size_t*)pRemainder, (size_t*)&DD, 128 / CAPI_BIT_LENGTH);

		goto irem128_negate;
	}

	// If bits 64-127 of the Dividend and Divisor are zero then we can do a 64-bit remainder.

	if ((DD.Hi | DV.Hi) == 0)
	{
		pRemainder->Hi = 0;
		pRemainder->Lo = DD.Lo % DV.Lo;

		goto irem128_negate;
	}

	// Here, we do it the hard way using the Binary Shift Algorithm.

	// Count the leading zero's of the Dividend and Divisor. SUB the smaller count from the other.

	T1 = (U32)capi_CountLZ((size_t*)&DD, 128 / CAPI_BIT_LENGTH);
	T2 = (U32)capi_CountLZ((size_t*)&DV, 128 / CAPI_BIT_LENGTH);

	if (T1 < T2) Shift = T2 - T1;
	else Shift = T1 - T2;

	// Shift our Divisor left by our leading zero-bit count.

	capi_shl128(&DV, Shift);

	//  Finally, perform the binary shift!

	do
	{
		if (capi_Compare((size_t*)&DV, (size_t*)&DD, 128 / CAPI_BIT_LENGTH) != 1)
		{
			capi_SubReturnBorrowEx((size_t*)&DD, (size_t*)&DV, 128 / CAPI_BIT_LENGTH);
		}

		capi_shr128(&DV, 1);
	} while (Shift-- != 0);

	// DD has the Remainder.

	pRemainder->Lo = DD.Lo;
	pRemainder->Hi = DD.Hi;

	// Last operation is negate if needed.

irem128_negate:

	if (Neg == TRUE)
	{
		capi_Negate((size_t*)pRemainder, 128 / CAPI_BIT_LENGTH);
	}
}

CAPI_FUNC(void) capi_llmul128(U128* p128, U128* pA, U128* pB)
{
	U64 T;

	T = (pA->Lo * pB->Hi) + (pA->Hi * pB->Lo);
	capi_mul64(p128, pA->Lo, pB->Lo);
	p128->Hi += T;
}

CAPI_FUNC(void) capi_mul128(U256* p256, U128* pA, U128* pB)
{
	U64 T;
	U128 LoPart, HiPart, T1, T2;
	size_t CF;

	// If both HIQWORDs are zero then just multiply the LOQWORDs.

	if ((pA->Hi | pB->Hi) == 0)
	{
		capi_mul64(&p256->Lo, pA->Lo, pB->Lo);
		p256->Hi.Lo = p256->Hi.Hi = 0;

		return;
	}

	// Here, we do it the hard way.

	capi_mul64(&T1, pA->Lo, pB->Hi);
	capi_mul64(&T2, pA->Hi, pB->Lo);

	capi_mul64(&LoPart, pA->Lo, pB->Lo);
	capi_mul64(&HiPart, pA->Hi, pB->Hi);

	T = T1.Hi;
	CF = capi_AddReturnCarryEx((size_t*)&T, (size_t*)&T2.Hi, 64 / CAPI_BIT_LENGTH);
	CF += capi_AddReturnCarryEx((size_t*)&HiPart.Lo, (size_t*)&T, 64 / CAPI_BIT_LENGTH);
	HiPart.Hi += CF;

	T = T1.Lo;
	CF = capi_AddReturnCarryEx((size_t*)&T, (size_t*)&T2.Lo, 64 / CAPI_BIT_LENGTH);
	CF += capi_AddReturnCarryEx((size_t*)&LoPart.Hi, (size_t*)&T, 64 / CAPI_BIT_LENGTH);
	capi_AddReturnCarry((size_t*)&HiPart, CF, 128 / CAPI_BIT_LENGTH);

	p256->Lo = LoPart;
	p256->Hi = HiPart;
}

CAPI_FUNC(void) capi_imul128(I256* p256, I128* pA, I128* pB)
{
	capi_mul128((U256*)p256, (U128*)pA, (U128*)pB);

	if (pA->Hi < 0) capi_SubReturnBorrowEx((size_t*)&p256->Hi, (size_t*)pB, 128 / CAPI_BIT_LENGTH);
	if (pB->Hi < 0) capi_SubReturnBorrowEx((size_t*)&p256->Hi, (size_t*)pA, 128 / CAPI_BIT_LENGTH);
}
