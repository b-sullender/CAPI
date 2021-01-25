
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
#include "ieee754.h"

static ieee754_NUM512* ieee754_Single_Lookup_Table = 0;
static size_t ieee754_Single_Scale[512 / CAPI_BIT_LENGTH];
static size_t ieee754_Single_OnesPlace[512 / CAPI_BIT_LENGTH];

static ieee754_NUM3584* ieee754_Double_Lookup_Table = 0;
static size_t ieee754_Double_Scale[3584 / CAPI_BIT_LENGTH];
static size_t ieee754_Double_OnesPlace[3584 / CAPI_BIT_LENGTH];

CAPI_SUBFUNC(I32) ieee754_Init_Tables()
{
	U32 I;
	size_t x[3584 / CAPI_BIT_LENGTH];

	// Create IEEE 754 Single Fixed-Point lookup table //

	ieee754_Single_Lookup_Table = (ieee754_NUM512*)capi_malloc((512 * 149) / 8);
	if (ieee754_Single_Lookup_Table == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	capi_memset(x, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
	capi_memset(ieee754_Single_Scale, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	x[0] = 5;
	ieee754_Single_Scale[0] = 1;

	for (I = 0; I < 148; I++)
	{
		capi_MulReturnOverflow(x, 10, 512 / CAPI_BIT_LENGTH);
		capi_MulReturnOverflow(ieee754_Single_Scale, 10, 512 / CAPI_BIT_LENGTH);
	}

	capi_memcopy(ieee754_Single_OnesPlace, ieee754_Single_Scale, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
	capi_MulReturnOverflow(ieee754_Single_OnesPlace, 10, 512 / CAPI_BIT_LENGTH);

	capi_memcopy(&ieee754_Single_Lookup_Table[0].Num512, x, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	for (I = 1; I < 149; I++)
	{
		capi_DivReturnRemainder(x, 2, 512 / CAPI_BIT_LENGTH);
		capi_memcopy(&ieee754_Single_Lookup_Table[I].Num512, x, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
	}

	// Create IEEE 754 Double Fixed-Point lookup table //

	ieee754_Double_Lookup_Table = (ieee754_NUM3584*)capi_malloc((3584 * 1074) / 8);
	if (ieee754_Double_Lookup_Table == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	capi_memset(x, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
	capi_memset(ieee754_Double_Scale, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	x[0] = 5;
	ieee754_Double_Scale[0] = 1;

	for (I = 0; I < 1073; I++)
	{
		capi_MulReturnOverflow(x, 10, 3584 / CAPI_BIT_LENGTH);
		capi_MulReturnOverflow(ieee754_Double_Scale, 10, 3584 / CAPI_BIT_LENGTH);
	}

	capi_memcopy(ieee754_Double_OnesPlace, ieee754_Double_Scale, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
	capi_MulReturnOverflow(ieee754_Double_OnesPlace, 10, 3584 / CAPI_BIT_LENGTH);

	capi_memcopy(&ieee754_Double_Lookup_Table[0].Num3584, x, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	for (I = 1; I < 1074; I++)
	{
		capi_DivReturnRemainder(x, 2, 3584 / CAPI_BIT_LENGTH);
		capi_memcopy(&ieee754_Double_Lookup_Table[I].Num3584, x, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
	}

	return CAPI_ERROR_NONE;
}

CAPI_SUBFUNC(U64) ieee754_GetValueFromSHR(size_t* pValue, U32 Shift, U32 Count)
{
	U32 Index, SubShift;
	U64 Result;

	Index = Shift / CAPI_BIT_LENGTH;
	SubShift = Shift % CAPI_BIT_LENGTH;

	Result = 0;

#if CAPI_BIT_LENGTH == 64
	Index += 1;
#elif CAPI_BIT_LENGTH == 32
	Index += 2;
	if (Index < Count)
	{
		Result |= ((U64)pValue[Index] << ((CAPI_BIT_LENGTH * 2) - SubShift));
	}
	Index -= 1;
#endif
	if (Index < Count)
	{
		Result |= ((U64)pValue[Index] << (CAPI_BIT_LENGTH - SubShift));
	}
	Index -= 1;

	Result |= (pValue[Index] >> SubShift);

	return Result;
}

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

CAPI_SUBFUNC(U32) ieee754_GetIntPartFromSingle(U32 Value)
{
	U32 Exponent, Mantissa, Shift;

	Exponent = Value >> 23;

	if (Exponent < 0x7F) return 0;

	Mantissa = Value & 0x7FFFFF;
	Shift = Exponent - 0x7F;

	return (Mantissa >> (23 - Shift)) | 1 << Shift;
}

CAPI_SUBFUNC(U64) ieee754_GetIntPartFromDouble(U64 Value)
{
	U32 Exponent, Shift;
	U64 Mantissa;

	Exponent = (Value >> 52);

	if (Exponent < 0x3FF) return 0;

	Mantissa = Value & 0xFFFFFFFFFFFFF;
	Shift = Exponent - 0x3FF;

	return (Mantissa >> (52 - Shift)) | 1LL << Shift;
}

CAPI_SUBFUNC(void) ieee754_GetFixedFracFromSingle(size_t* pResult, U32 Value)
{
	U32 Exponent, Mantissa, I, BitSet;

	capi_memset(pResult, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	Exponent = Value >> 23;
	Mantissa = Value & 0x7FFFFF;

	BitSet = 1 << 22;

	if (Exponent == 0)
	{
		I = 0x7E;
	}
	else if (Exponent < 0x7F)
	{
		I = 0x7E - Exponent;
		capi_AddReturnCarryEx(pResult, ieee754_Single_Lookup_Table[I].Num512, 512 / CAPI_BIT_LENGTH);
		I++;
	}
	else
	{
		I = 0;
		BitSet >>= (Exponent - 0x7F);
	}

	for (; BitSet != 0; BitSet >>= 1)
	{
		if ((Mantissa & BitSet) != 0)
		{
			capi_AddReturnCarryEx(pResult, ieee754_Single_Lookup_Table[I].Num512, 512 / CAPI_BIT_LENGTH);
		}
		I++;
	}
}

CAPI_SUBFUNC(void) ieee754_GetFixedFracFromDouble(size_t* pResult, U64 Value)
{
	U64 Exponent, Mantissa, I, BitSet;

	capi_memset(pResult, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	Exponent = Value >> 52;
	Mantissa = Value & 0xFFFFFFFFFFFFF;

	BitSet = 1LL << 51;

	if (Exponent == 0)
	{
		I = 0x3FE;
	}
	else if (Exponent < 0x3FF)
	{
		I = 0x3FE - Exponent;
		capi_AddReturnCarryEx(pResult, ieee754_Double_Lookup_Table[I].Num3584, 3584 / CAPI_BIT_LENGTH);
		I++;
	}
	else
	{
		I = 0;
		BitSet >>= (Exponent - 0x3FF);
	}

	for (; BitSet != 0; BitSet >>= 1)
	{
		if ((Mantissa & BitSet) != 0)
		{
			capi_AddReturnCarryEx(pResult, ieee754_Double_Lookup_Table[I].Num3584, 3584 / CAPI_BIT_LENGTH);
		}
		I++;
	}
}

CAPI_FUNC(I8) capi_ScanSingleA(float* pResult, const ASCII* pSource, U32 Flags, ASCII** ppNewPos)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[512 / CAPI_BIT_LENGTH];
	size_t TempUp[512 / CAPI_BIT_LENGTH];
	size_t TempDown[512 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[2];
	U32 MaxPayLoad;

	I8 RetStatus;
	I32 I, U;
	U32 CodePoint, Decimal, TpMarker, Shift, eCount, eSign;
	U32 Exponent, Mantissa, BitSet, TempResult, Sign;

	if (ieee754_Single_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Single_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	MaxIntPart[0] = 0;
	MaxIntPart[1] = 0xFFFFFF0000000000;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;
	}

	CodePoint = *pSource;

	while (CodePoint == 0x20)
	{
		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2D)
	{
		Sign = BIT(31);

		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2B)
	{
		if (Sign != 0) return 1;

		pSource++;
		CodePoint = *pSource;
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73)) // S
	{
		pSource++;
		CodePoint = *pSource;

		Mantissa = 1;
		MaxPayLoad = 0x7FFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71)) // Q
	{
		pSource++;
		CodePoint = *pSource;

		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		if (CodePoint == Decimal)
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource++;
			CodePoint = *pSource;

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7F800000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69)) // I
	{
		pSource++;
		CodePoint = *pSource;

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7F800000;

		goto exit_func;
	}

	U = -1;

	while ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource++;
		CodePoint = *pSource;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;
				CodePoint = *pSource;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					U++;

					pSource++;
					CodePoint = *pSource;
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while (CodePoint == TpMarker)
		{
			pSource++;
			CodePoint = *pSource;

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				U++;

				pSource++;
				CodePoint = *pSource;
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
		}
	}

	I = -1;

	if (CodePoint == Decimal)
	{
	get_FracPart:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (148 - I) % ieee754_NUM_MAX_TENS;
	I = (148 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 512 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45))
	{
		pSource++;
		eSign = *pSource;
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if (eSign == 0x2B)
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if (eSign == 0x2D)
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 512 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 128 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 23;
		goto skip_IntPart;
	}

	Shift = 128 - (U32)capi_CountLZ(IntPart, 128 / CAPI_BIT_LENGTH);
	Exponent = Shift + 0x7E;
	Shift += 8;

	if (Shift <= 32)
	{
		Shift = (32 - Shift);
		Mantissa = (U32)IntPart[0] << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x7F;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1 << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1 << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromSingle(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 512 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromSingle(TempUp, TempResult + 1);
				if (IntPart[0] != ieee754_GetIntPartFromSingle(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 512 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 512 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 512 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 32;

		Mantissa = (U32)ieee754_GetValueFromSHR(IntPart, Shift, 128 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				if (capi_Compare(TempDown, IntPart, 128 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				ieee754_SetValueFromSHL((U32*)TempUp, (U64)Mantissa + 1, Shift, 0, 4);

				capi_SubReturnBorrowEx(TempUp, IntPart, 128 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 128 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 128 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U32*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (ASCII*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanDoubleA(double* pResult, const ASCII* pSource, U32 Flags, ASCII** ppNewPos)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[3584 / CAPI_BIT_LENGTH];
	size_t TempUp[3584 / CAPI_BIT_LENGTH];
	size_t TempDown[3584 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[16];
	U64 MaxPayLoad;

	I8 RetStatus;
	I32 I, U;
	U32 CodePoint, Decimal, TpMarker, Shift, eCount, eSign;
	U64 Exponent, Mantissa, BitSet, TempResult, Sign;

	if (ieee754_Double_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Double_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(MaxIntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	MaxIntPart[15] = 0xFFFFFFFFFFFFF800;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;
	}

	CodePoint = *pSource;

	while (CodePoint == 0x20)
	{
		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2D)
	{
		Sign = BIT(63);

		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2B)
	{
		if (Sign != 0) return 1;

		pSource++;
		CodePoint = *pSource;
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73)) // S
	{
		pSource++;
		CodePoint = *pSource;

		Mantissa = 1;
		MaxPayLoad = 0xFFFFFFFFFFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71)) // Q
	{
		pSource++;
		CodePoint = *pSource;

		MaxPayLoad = 0x7FFFFFFFFFFFF;
		TempResult = 0x8000000000000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x8000000000000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		if (CodePoint == Decimal)
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource++;
			CodePoint = *pSource;

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7FF0000000000000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69)) // I
	{
		pSource++;
		CodePoint = *pSource;

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7FF0000000000000;

		goto exit_func;
	}

	U = -1;

	while ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource++;
		CodePoint = *pSource;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;
				CodePoint = *pSource;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					U++;

					pSource++;
					CodePoint = *pSource;
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while (CodePoint == TpMarker)
		{
			pSource++;
			CodePoint = *pSource;

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				U++;

				pSource++;
				CodePoint = *pSource;
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
		}
	}

	I = -1;

	if (CodePoint == Decimal)
	{
	get_FracPart:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (1073 - I) % ieee754_NUM_MAX_TENS;
	I = (1073 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 3584 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45))
	{
		pSource++;
		eSign = *pSource;
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if (eSign == 0x2B)
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if (eSign == 0x2D)
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 1024 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 52;
		goto skip_IntPart;
	}

	Shift = 1024 - (U32)capi_CountLZ(IntPart, 1024 / CAPI_BIT_LENGTH);
	Exponent = (U64)Shift + 0x3FE;
	Shift += 11;

	if (Shift <= 64)
	{
		Shift = (64 - Shift);
		Mantissa = *(U64*)&IntPart << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x3FF;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1LL << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1LL << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromDouble(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 3584 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromDouble(TempUp, TempResult + 1);
				if (*(U64*)&IntPart != ieee754_GetIntPartFromDouble(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 3584 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 3584 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 3584 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 64;

		Mantissa = ieee754_GetValueFromSHR(IntPart, Shift, 1024 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				if (capi_Compare(TempDown, IntPart, 1024 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				ieee754_SetValueFromSHL((U32*)TempUp, Mantissa + 1, Shift, 0, 32);

				capi_SubReturnBorrowEx(TempUp, IntPart, 1024 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 1024 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 1024 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U64*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (ASCII*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanSingleU(float* pResult, const UTF8* pSource, U32 Flags, UTF8** ppNewPos)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[512 / CAPI_BIT_LENGTH];
	size_t TempUp[512 / CAPI_BIT_LENGTH];
	size_t TempDown[512 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[2];
	U32 MaxPayLoad;

	I8 RetStatus;
	U8 CharUnits, NextCharUnits;
	I32 I, U;
	U32 CodePoint, NextCodePoint, Decimal, Fullwidth_Decimal, TpMarker, Fullwidth_TpMarker, Shift, eCount, eSign;
	U32 Exponent, Mantissa, BitSet, TempResult, Sign;
	const UTF8* pNextChar;

	if (ieee754_Single_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Single_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	MaxIntPart[0] = 0;
	MaxIntPart[1] = 0xFFFFFF0000000000;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;

		Fullwidth_Decimal = 0xFF0E;
		Fullwidth_TpMarker = 0xFF0C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;

		Fullwidth_Decimal = 0xFF0C;
		Fullwidth_TpMarker = 0xFF0E;
	}

	CharUnits = capi_UTF8_GetCharUnits(*pSource);
	CodePoint = capi_UTF8_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2D) || (CodePoint == 0x2212) || (CodePoint == 0xFE63) || (CodePoint == 0xFF0D))
	{
		Sign = BIT(31);

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (Sign != 0) return 1;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73) || (CodePoint == 0xFF33) || (CodePoint == 0xFF53)) // S
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		Mantissa = 1;
		MaxPayLoad = 0x7FFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71) || (CodePoint == 0xFF31) || (CodePoint == 0xFF51)) // Q
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E) || (CodePoint == 0xFF2E) || (CodePoint == 0xFF4E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
		case 0xFF41:
		case 0xFF21:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0xFF10;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7F800000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69) || (CodePoint == 0xFF29) || (CodePoint == 0xFF49)) // I
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
		case 0xFF26:
		case 0xFF46:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
		case 0xFF4E:
		case 0xFF2E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
		case 0xFF54:
		case 0xFF34:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
		case 0xFF59:
		case 0xFF39:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7F800000;

		goto exit_func;
	}

	U = -1;

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0xFF10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

		if (((NextCodePoint >= 0x30) && (NextCodePoint <= 0x39)) || ((NextCodePoint >= 0xFF10) && (NextCodePoint <= 0xFF19)))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				pNextChar = pSource + CharUnits;
				NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
				NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

				if ((NextCodePoint < 0x30) || (NextCodePoint > 0xFF19)) break;
				if ((NextCodePoint > 0x39) && (NextCodePoint < 0xFF10)) break;

				pSource = pNextChar;
				CharUnits = NextCharUnits;
				CodePoint = NextCodePoint;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
					else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
					else return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					U++;

					pSource += CharUnits;
					CharUnits = capi_UTF8_GetCharUnits(*pSource);
					CodePoint = capi_UTF8_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_COMMA) && (TpMarker == 0xFF0C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0xFF0E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint, 128 / CAPI_BIT_LENGTH) != 0) return -1;

				U++;

				pSource += CharUnits;
				CharUnits = capi_UTF8_GetCharUnits(*pSource);
				CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
		}
	}

	I = -1;

	if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
	{
	get_FracPart:
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0xFF10, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (148 - I) % ieee754_NUM_MAX_TENS;
	I = (148 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 512 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45) || (CodePoint == 0xFF45) || (CodePoint == 0xFF25))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		eSign = capi_UTF8_Decode(CharUnits, pSource);
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			eCount *= 10;
			eCount += CodePoint - 0xFF10;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if ((eSign == 0x2B) || (eSign == 0xFE62) || (eSign == 0xFF0B))
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if ((eSign == 0x2D) || (eSign == 0x2212) || (eSign == 0xFE63) || (eSign == 0xFF0D))
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 512 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 128 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 23;
		goto skip_IntPart;
	}

	Shift = 128 - (U32)capi_CountLZ(IntPart, 128 / CAPI_BIT_LENGTH);
	Exponent = Shift + 0x7E;
	Shift += 8;

	if (Shift <= 32)
	{
		Shift = (32 - Shift);
		Mantissa = (U32)IntPart[0] << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x7F;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1 << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1 << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromSingle(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 512 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromSingle(TempUp, TempResult + 1);
				if (IntPart[0] != ieee754_GetIntPartFromSingle(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 512 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 512 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 512 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 32;

		Mantissa = (U32)ieee754_GetValueFromSHR(IntPart, Shift, 128 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				if (capi_Compare(TempDown, IntPart, 128 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				ieee754_SetValueFromSHL((U32*)TempUp, (U64)Mantissa + 1, Shift, 0, 4);

				capi_SubReturnBorrowEx(TempUp, IntPart, 128 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 128 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 128 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U32*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF8*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanDoubleU(double* pResult, const UTF8* pSource, U32 Flags, UTF8** ppNewPos)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[3584 / CAPI_BIT_LENGTH];
	size_t TempUp[3584 / CAPI_BIT_LENGTH];
	size_t TempDown[3584 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[16];
	U64 MaxPayLoad;

	I8 RetStatus;
	U8 CharUnits, NextCharUnits;
	I32 I, U;
	U32 CodePoint, NextCodePoint, Decimal, Fullwidth_Decimal, TpMarker, Fullwidth_TpMarker, Shift, eCount, eSign;
	U64 Exponent, Mantissa, BitSet, TempResult, Sign;
	const UTF8* pNextChar;

	if (ieee754_Double_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Double_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(MaxIntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	MaxIntPart[15] = 0xFFFFFFFFFFFFF800;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;

		Fullwidth_Decimal = 0xFF0E;
		Fullwidth_TpMarker = 0xFF0C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;

		Fullwidth_Decimal = 0xFF0C;
		Fullwidth_TpMarker = 0xFF0E;
	}

	CharUnits = capi_UTF8_GetCharUnits(*pSource);
	CodePoint = capi_UTF8_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2D) || (CodePoint == 0x2212) || (CodePoint == 0xFE63) || (CodePoint == 0xFF0D))
	{
		Sign = BIT(63);

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (Sign != 0) return 1;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73) || (CodePoint == 0xFF33) || (CodePoint == 0xFF53)) // S
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		Mantissa = 1;
		MaxPayLoad = 0xFFFFFFFFFFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71) || (CodePoint == 0xFF31) || (CodePoint == 0xFF51)) // Q
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		MaxPayLoad = 0x7FFFFFFFFFFFF;
		TempResult = 0x8000000000000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E) || (CodePoint == 0xFF2E) || (CodePoint == 0xFF4E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x8000000000000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
		case 0xFF41:
		case 0xFF21:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0xFF10;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7FF0000000000000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69) || (CodePoint == 0xFF29) || (CodePoint == 0xFF49)) // I
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
		case 0xFF26:
		case 0xFF46:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
		case 0xFF4E:
		case 0xFF2E:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
		case 0xFF54:
		case 0xFF34:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
		case 0xFF59:
		case 0xFF39:
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7FF0000000000000;

		goto exit_func;
	}

	U = -1;

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0xFF10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

		if (((NextCodePoint >= 0x30) && (NextCodePoint <= 0x39)) || ((NextCodePoint >= 0xFF10) && (NextCodePoint <= 0xFF19)))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				pNextChar = pSource + CharUnits;
				NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
				NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

				if ((NextCodePoint < 0x30) || (NextCodePoint > 0xFF19)) break;
				if ((NextCodePoint > 0x39) && (NextCodePoint < 0xFF10)) break;

				pSource = pNextChar;
				CharUnits = NextCharUnits;
				CodePoint = NextCodePoint;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
					else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
					else return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					U++;

					pSource += CharUnits;
					CharUnits = capi_UTF8_GetCharUnits(*pSource);
					CodePoint = capi_UTF8_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_COMMA) && (TpMarker == 0xFF0C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0xFF0E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

				U++;

				pSource += CharUnits;
				CharUnits = capi_UTF8_GetCharUnits(*pSource);
				CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
		}
	}

	I = -1;

	if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
	{
	get_FracPart:
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0xFF10, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (1073 - I) % ieee754_NUM_MAX_TENS;
	I = (1073 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 3584 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45) || (CodePoint == 0xFF45) || (CodePoint == 0xFF25))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		eSign = capi_UTF8_Decode(CharUnits, pSource);
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			eCount *= 10;
			eCount += CodePoint - 0xFF10;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if ((eSign == 0x2B) || (eSign == 0xFE62) || (eSign == 0xFF0B))
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if ((eSign == 0x2D) || (eSign == 0x2212) || (eSign == 0xFE63) || (eSign == 0xFF0D))
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 1024 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 52;
		goto skip_IntPart;
	}

	Shift = 1024 - (U32)capi_CountLZ(IntPart, 1024 / CAPI_BIT_LENGTH);
	Exponent = (U64)Shift + 0x3FE;
	Shift += 11;

	if (Shift <= 64)
	{
		Shift = (64 - Shift);
		Mantissa = *(U64*)&IntPart << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x3FF;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1LL << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1LL << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromDouble(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 3584 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromDouble(TempUp, TempResult + 1);
				if (*(U64*)&IntPart != ieee754_GetIntPartFromDouble(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 3584 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 3584 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 3584 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 64;

		Mantissa = ieee754_GetValueFromSHR(IntPart, Shift, 1024 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				if (capi_Compare(TempDown, IntPart, 1024 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				ieee754_SetValueFromSHL((U32*)TempUp, Mantissa + 1, Shift, 0, 32);

				capi_SubReturnBorrowEx(TempUp, IntPart, 1024 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 1024 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 1024 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U64*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF8*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanSingleW(float* pResult, const UTF16* pSource, U32 Flags, UTF16** ppNewPos)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[512 / CAPI_BIT_LENGTH];
	size_t TempUp[512 / CAPI_BIT_LENGTH];
	size_t TempDown[512 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[2];
	U32 MaxPayLoad;

	I8 RetStatus;
	U8 CharUnits, NextCharUnits;
	I32 I, U;
	U32 CodePoint, NextCodePoint, Decimal, Fullwidth_Decimal, TpMarker, Fullwidth_TpMarker, Shift, eCount, eSign;
	U32 Exponent, Mantissa, BitSet, TempResult, Sign;
	const UTF16* pNextChar;

	if (ieee754_Single_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Single_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	MaxIntPart[0] = 0;
	MaxIntPart[1] = 0xFFFFFF0000000000;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;

		Fullwidth_Decimal = 0xFF0E;
		Fullwidth_TpMarker = 0xFF0C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;

		Fullwidth_Decimal = 0xFF0C;
		Fullwidth_TpMarker = 0xFF0E;
	}

	CharUnits = capi_UTF16_GetCharUnits(*pSource);
	CodePoint = capi_UTF16_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2D) || (CodePoint == 0x2212) || (CodePoint == 0xFE63) || (CodePoint == 0xFF0D))
	{
		Sign = BIT(31);

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (Sign != 0) return 1;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73) || (CodePoint == 0xFF33) || (CodePoint == 0xFF53)) // S
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		Mantissa = 1;
		MaxPayLoad = 0x7FFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71) || (CodePoint == 0xFF31) || (CodePoint == 0xFF51)) // Q
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E) || (CodePoint == 0xFF2E) || (CodePoint == 0xFF4E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
		case 0xFF41:
		case 0xFF21:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0xFF10;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7F800000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69) || (CodePoint == 0xFF29) || (CodePoint == 0xFF49)) // I
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
		case 0xFF26:
		case 0xFF46:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
		case 0xFF4E:
		case 0xFF2E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
		case 0xFF54:
		case 0xFF34:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
		case 0xFF59:
		case 0xFF39:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7F800000;

		goto exit_func;
	}

	U = -1;

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0xFF10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

		if (((NextCodePoint >= 0x30) && (NextCodePoint <= 0x39)) || ((NextCodePoint >= 0xFF10) && (NextCodePoint <= 0xFF19)))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				pNextChar = pSource + CharUnits;
				NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
				NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

				if ((NextCodePoint < 0x30) || (NextCodePoint > 0xFF19)) break;
				if ((NextCodePoint > 0x39) && (NextCodePoint < 0xFF10)) break;

				pSource = pNextChar;
				CharUnits = NextCharUnits;
				CodePoint = NextCodePoint;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
					else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
					else return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					U++;

					pSource += CharUnits;
					CharUnits = capi_UTF16_GetCharUnits(*pSource);
					CodePoint = capi_UTF16_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_COMMA) && (TpMarker == 0xFF0C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0xFF0E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint, 128 / CAPI_BIT_LENGTH) != 0) return -1;

				U++;

				pSource += CharUnits;
				CharUnits = capi_UTF16_GetCharUnits(*pSource);
				CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
		}
	}

	I = -1;

	if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
	{
	get_FracPart:
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0xFF10, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (148 - I) % ieee754_NUM_MAX_TENS;
	I = (148 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 512 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45) || (CodePoint == 0xFF45) || (CodePoint == 0xFF25))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		eSign = capi_UTF16_Decode(CharUnits, pSource);
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			eCount *= 10;
			eCount += CodePoint - 0xFF10;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if ((eSign == 0x2B) || (eSign == 0xFE62) || (eSign == 0xFF0B))
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if ((eSign == 0x2D) || (eSign == 0x2212) || (eSign == 0xFE63) || (eSign == 0xFF0D))
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 512 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 128 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 23;
		goto skip_IntPart;
	}

	Shift = 128 - (U32)capi_CountLZ(IntPart, 128 / CAPI_BIT_LENGTH);
	Exponent = Shift + 0x7E;
	Shift += 8;

	if (Shift <= 32)
	{
		Shift = (32 - Shift);
		Mantissa = (U32)IntPart[0] << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x7F;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1 << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1 << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromSingle(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 512 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromSingle(TempUp, TempResult + 1);
				if (IntPart[0] != ieee754_GetIntPartFromSingle(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 512 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 512 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 512 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 32;

		Mantissa = (U32)ieee754_GetValueFromSHR(IntPart, Shift, 128 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				if (capi_Compare(TempDown, IntPart, 128 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				ieee754_SetValueFromSHL((U32*)TempUp, (U64)Mantissa + 1, Shift, 0, 4);

				capi_SubReturnBorrowEx(TempUp, IntPart, 128 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 128 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 128 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U32*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF16*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanDoubleW(double* pResult, const UTF16* pSource, U32 Flags, UTF16** ppNewPos)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[3584 / CAPI_BIT_LENGTH];
	size_t TempUp[3584 / CAPI_BIT_LENGTH];
	size_t TempDown[3584 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[16];
	U64 MaxPayLoad;

	I8 RetStatus;
	U8 CharUnits, NextCharUnits;
	I32 I, U;
	U32 CodePoint, NextCodePoint, Decimal, Fullwidth_Decimal, TpMarker, Fullwidth_TpMarker, Shift, eCount, eSign;
	U64 Exponent, Mantissa, BitSet, TempResult, Sign;
	const UTF16* pNextChar;

	if (ieee754_Double_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Double_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(MaxIntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	MaxIntPart[15] = 0xFFFFFFFFFFFFF800;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;

		Fullwidth_Decimal = 0xFF0E;
		Fullwidth_TpMarker = 0xFF0C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;

		Fullwidth_Decimal = 0xFF0C;
		Fullwidth_TpMarker = 0xFF0E;
	}

	CharUnits = capi_UTF16_GetCharUnits(*pSource);
	CodePoint = capi_UTF16_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2D) || (CodePoint == 0x2212) || (CodePoint == 0xFE63) || (CodePoint == 0xFF0D))
	{
		Sign = BIT(63);

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (Sign != 0) return 1;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73) || (CodePoint == 0xFF33) || (CodePoint == 0xFF53)) // S
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		Mantissa = 1;
		MaxPayLoad = 0xFFFFFFFFFFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71) || (CodePoint == 0xFF31) || (CodePoint == 0xFF51)) // Q
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		MaxPayLoad = 0x7FFFFFFFFFFFF;
		TempResult = 0x8000000000000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E) || (CodePoint == 0xFF2E) || (CodePoint == 0xFF4E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x8000000000000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
		case 0xFF41:
		case 0xFF21:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0xFF10;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7FF0000000000000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69) || (CodePoint == 0xFF29) || (CodePoint == 0xFF49)) // I
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
		case 0xFF2E:
		case 0xFF4E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
		case 0xFF26:
		case 0xFF46:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
		case 0xFF4E:
		case 0xFF2E:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
		case 0xFF49:
		case 0xFF29:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
		case 0xFF54:
		case 0xFF34:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
		case 0xFF59:
		case 0xFF39:
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7FF0000000000000;

		goto exit_func;
	}

	U = -1;

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0xFF10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

		if (((NextCodePoint >= 0x30) && (NextCodePoint <= 0x39)) || ((NextCodePoint >= 0xFF10) && (NextCodePoint <= 0xFF19)))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				pNextChar = pSource + CharUnits;
				NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
				NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

				if ((NextCodePoint < 0x30) || (NextCodePoint > 0xFF19)) break;
				if ((NextCodePoint > 0x39) && (NextCodePoint < 0xFF10)) break;

				pSource = pNextChar;
				CharUnits = NextCharUnits;
				CodePoint = NextCodePoint;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
					else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
					else return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					U++;

					pSource += CharUnits;
					CharUnits = capi_UTF16_GetCharUnits(*pSource);
					CodePoint = capi_UTF16_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_COMMA) && (TpMarker == 0xFF0C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0xFF0E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

				U++;

				pSource += CharUnits;
				CharUnits = capi_UTF16_GetCharUnits(*pSource);
				CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) return 1;
		}
	}

	I = -1;

	if ((CodePoint == Decimal) || (CodePoint == Fullwidth_Decimal))
	{
	get_FracPart:
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0xFF10, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (1073 - I) % ieee754_NUM_MAX_TENS;
	I = (1073 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 3584 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45) || (CodePoint == 0xFF45) || (CodePoint == 0xFF25))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		eSign = capi_UTF16_Decode(CharUnits, pSource);
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
		{
			eCount *= 10;
			eCount += CodePoint - 0xFF10;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if ((eSign == 0x2B) || (eSign == 0xFE62) || (eSign == 0xFF0B))
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if ((eSign == 0x2D) || (eSign == 0x2212) || (eSign == 0xFE63) || (eSign == 0xFF0D))
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 1024 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 52;
		goto skip_IntPart;
	}

	Shift = 1024 - (U32)capi_CountLZ(IntPart, 1024 / CAPI_BIT_LENGTH);
	Exponent = (U64)Shift + 0x3FE;
	Shift += 11;

	if (Shift <= 64)
	{
		Shift = (64 - Shift);
		Mantissa = *(U64*)&IntPart << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x3FF;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1LL << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1LL << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromDouble(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 3584 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromDouble(TempUp, TempResult + 1);
				if (*(U64*)&IntPart != ieee754_GetIntPartFromDouble(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 3584 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 3584 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 3584 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 64;

		Mantissa = ieee754_GetValueFromSHR(IntPart, Shift, 1024 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				if (capi_Compare(TempDown, IntPart, 1024 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				ieee754_SetValueFromSHL((U32*)TempUp, Mantissa + 1, Shift, 0, 32);

				capi_SubReturnBorrowEx(TempUp, IntPart, 1024 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 1024 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 1024 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U64*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF16*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanSingleL(float* pResult, const UTF32* pSource, U32 Flags, UTF32** ppNewPos)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[512 / CAPI_BIT_LENGTH];
	size_t TempUp[512 / CAPI_BIT_LENGTH];
	size_t TempDown[512 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[2];
	U32 MaxPayLoad;

	I8 RetStatus;
	I32 I, U;
	U32 CodePoint, Decimal, TpMarker, Shift, eCount, eSign;
	U32 Exponent, Mantissa, BitSet, TempResult, Sign;

	if (ieee754_Single_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Single_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

	MaxIntPart[0] = 0;
	MaxIntPart[1] = 0xFFFFFF0000000000;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;
	}

	CodePoint = *pSource;

	while (CodePoint == 0x20)
	{
		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2D)
	{
		Sign = BIT(31);

		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2B)
	{
		if (Sign != 0) return 1;

		pSource++;
		CodePoint = *pSource;
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73)) // S
	{
		pSource++;
		CodePoint = *pSource;

		Mantissa = 1;
		MaxPayLoad = 0x7FFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71)) // Q
	{
		pSource++;
		CodePoint = *pSource;

		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x400000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		if (CodePoint == Decimal)
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource++;
			CodePoint = *pSource;

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7F800000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69)) // I
	{
		pSource++;
		CodePoint = *pSource;

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7F800000;

		goto exit_func;
	}

	U = -1;

	while ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource++;
		CodePoint = *pSource;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;
				CodePoint = *pSource;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					U++;

					pSource++;
					CodePoint = *pSource;
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while (CodePoint == TpMarker)
		{
			pSource++;
			CodePoint = *pSource;

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 128 / CAPI_BIT_LENGTH) != 0) return -1;
				U++;

				pSource++;
				CodePoint = *pSource;
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
		}
	}

	I = -1;

	if (CodePoint == Decimal)
	{
	get_FracPart:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 148)
			{
				capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 512 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (148 - I) % ieee754_NUM_MAX_TENS;
	I = (148 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 512 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45))
	{
		pSource++;
		eSign = *pSource;
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 255) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if (eSign == 0x2B)
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 512 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 128 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 128 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if (eSign == 0x2D)
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 512 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Single_Scale, 512 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 128 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 23;
		goto skip_IntPart;
	}

	Shift = 128 - (U32)capi_CountLZ(IntPart, 128 / CAPI_BIT_LENGTH);
	Exponent = Shift + 0x7E;
	Shift += 8;

	if (Shift <= 32)
	{
		Shift = (32 - Shift);
		Mantissa = (U32)IntPart[0] << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x7F;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1 << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1 << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 512 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Single_OnesPlace, FracPart, 512 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (512 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromSingle(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 512 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromSingle(TempUp, TempResult + 1);
				if (IntPart[0] != ieee754_GetIntPartFromSingle(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Single_OnesPlace, 512 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 512 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 512 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 512 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 32;

		Mantissa = (U32)ieee754_GetValueFromSHR(IntPart, Shift, 128 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0xFF) << 23) | (Mantissa & 0x7FFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				if (capi_Compare(TempDown, IntPart, 128 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 4);
				ieee754_SetValueFromSHL((U32*)TempUp, (U64)Mantissa + 1, Shift, 0, 4);

				capi_SubReturnBorrowEx(TempUp, IntPart, 128 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 128 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 128 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U32*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF32*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanDoubleL(double* pResult, const UTF32* pSource, U32 Flags, UTF32** ppNewPos)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[3584 / CAPI_BIT_LENGTH];
	size_t TempUp[3584 / CAPI_BIT_LENGTH];
	size_t TempDown[3584 / CAPI_BIT_LENGTH];

	U64 MaxIntPart[16];
	U64 MaxPayLoad;

	I8 RetStatus;
	I32 I, U;
	U32 CodePoint, Decimal, TpMarker, Shift, eCount, eSign;
	U64 Exponent, Mantissa, BitSet, TempResult, Sign;

	if (ieee754_Double_Lookup_Table == 0)
	{
		ieee754_Init_Tables();
		if (ieee754_Double_Lookup_Table == 0) return 2;
	}

	if ((pResult == 0) || (pSource == 0)) return 2;

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(MaxIntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

	MaxIntPart[15] = 0xFFFFFFFFFFFFF800;

	Sign = 0;
	Mantissa = 0;

	if ((Flags & SCAN_DP_COMMA) != SCAN_DP_COMMA)
	{
		Decimal = 0x2E;
		TpMarker = 0x2C;
	}
	else
	{
		Decimal = 0x2C;
		TpMarker = 0x2E;
	}

	CodePoint = *pSource;

	while (CodePoint == 0x20)
	{
		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2D)
	{
		Sign = BIT(63);

		pSource++;
		CodePoint = *pSource;
	}

	if (CodePoint == 0x2B)
	{
		if (Sign != 0) return 1;

		pSource++;
		CodePoint = *pSource;
	}

	if ((CodePoint == 0x53) || (CodePoint == 0x73)) // S
	{
		pSource++;
		CodePoint = *pSource;

		Mantissa = 1;
		MaxPayLoad = 0xFFFFFFFFFFFFF;
		TempResult = 0;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x51) || (CodePoint == 0x71)) // Q
	{
		pSource++;
		CodePoint = *pSource;

		MaxPayLoad = 0x7FFFFFFFFFFFF;
		TempResult = 0x8000000000000;

		goto test_for_NaN;
	}

	if ((CodePoint == 0x4E) || (CodePoint == 0x6E)) // N
	{
		MaxPayLoad = 0x3FFFFF;
		TempResult = 0x8000000000000;

	test_for_NaN:

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // A
		{
		case 0x61:
		case 0x41:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		if (CodePoint == Decimal)
		{
			Mantissa = 0;

		get_NaN_payload:
			pSource++;
			CodePoint = *pSource;

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
			{
				Mantissa *= 10;
				Mantissa += CodePoint - 0x30;

				if (Mantissa > MaxPayLoad) return -1;

				goto get_NaN_payload;
			}

			if ((TempResult == 0) && (Mantissa == 0)) return 1;
		}

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult |= 0x7FF0000000000000 | Mantissa;

		goto exit_func;
	}

	if ((CodePoint == 0x49) || (CodePoint == 0x69)) // I
	{
		pSource++;
		CodePoint = *pSource;

		switch (CodePoint) // N
		{
		case 0x4E:
		case 0x6E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // F
		{
		case 0x46:
		case 0x66:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			goto inf_test_end_string;
		}

		switch (CodePoint) // N
		{
		case 0x6E:
		case 0x4E:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // I
		{
		case 0x69:
		case 0x49:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // T
		{
		case 0x74:
		case 0x54:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

		switch (CodePoint) // Y
		{
		case 0x79:
		case 0x59:
			pSource++;
			CodePoint = *pSource;
			break;
		default:
			return 1;
		}

	inf_test_end_string:

		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;

		TempResult = 0x7FF0000000000000;

		goto exit_func;
	}

	U = -1;

	while ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
		U++;

		pSource++;
		CodePoint = *pSource;
	}

	if ((Flags & SCAN_SPACE) && (CodePoint == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

			while (CodePoint == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;
				CodePoint = *pSource;

				for (I = 0; I < 3; I++)
				{
					if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					U++;

					pSource++;
					CodePoint = *pSource;
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (TpMarker == 0x2C) && (CodePoint == TpMarker)) ||
		((Flags & SCAN_PERIOD) && (TpMarker == 0x2E) && (CodePoint == TpMarker)))
	{
		if (((U + 1) == 0) || ((U + 1) > 3)) return 1;

		while (CodePoint == TpMarker)
		{
			pSource++;
			CodePoint = *pSource;

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint < 0x30) || (CodePoint > 0x39)) return 1;

				if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				if (capi_AddReturnCarry(IntPart, CodePoint - 0x30, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
				U++;

				pSource++;
				CodePoint = *pSource;
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) return 1;
		}
	}

	I = -1;

	if (CodePoint == Decimal)
	{
	get_FracPart:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			if (I < 1073)
			{
				capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
				capi_AddReturnCarry(FracPart, CodePoint - 0x30, 3584 / CAPI_BIT_LENGTH);
				I++;
			}
			goto get_FracPart;
		}
	}

	if ((U < 0) && (I < 0)) return 1;

	if (I < 0) goto skip_frac_scale;

	U = (1073 - I) % ieee754_NUM_MAX_TENS;
	I = (1073 - I) / ieee754_NUM_MAX_TENS;

	for (; U > 0; U--) capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);
	for (; I > 0; I--) capi_MulReturnOverflow(FracPart, ieee754_MAX_TENS, 3584 / CAPI_BIT_LENGTH);

skip_frac_scale:

	if ((CodePoint == 0x65) || (CodePoint == 0x45))
	{
		pSource++;
		eSign = *pSource;
		if (eSign == 0) return 1;

		eCount = 0x80000000;

	get_notation:
		pSource++;
		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
		{
			eCount *= 10;
			eCount += CodePoint - 0x30;
			if (eCount > 2048) return -1;
			goto get_notation;
		}

		if (eCount == 0x80000000) return 1;

		if (eSign == 0x2B)
		{
			if (I < 0)
			{
				while (eCount > 0)
				{
					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					eCount -= 1;
				}
			}
			else
			{
				while (eCount > 0)
				{
					U = 0;

					while (capi_Compare(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH) == -1)
					{
						capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
						U++;
					}

					capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
					capi_MulReturnOverflow(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

					if (capi_MulReturnOverflow(IntPart, 10, 1024 / CAPI_BIT_LENGTH) != 0) return -1;
					if (capi_AddReturnCarry(IntPart, (size_t)10 - U, 1024 / CAPI_BIT_LENGTH) != 0) return -1;

					eCount -= 1;
				}
			}
		}
		else if (eSign == 0x2D)
		{
			while (eCount > 0)
			{
				U = (I32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
				capi_DivReturnRemainder(FracPart, 10, 3584 / CAPI_BIT_LENGTH);

				while (U > 0)
				{
					capi_AddReturnCarryEx(FracPart, ieee754_Double_Scale, 3584 / CAPI_BIT_LENGTH);
					U -= 1;
				}

				eCount -= 1;
			}
		}
		else return 1;
	}

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource++;
			CodePoint = *pSource;
		}

		if (CodePoint != 0) return 1;
	}

	if (capi_Compare(IntPart, (size_t*)MaxIntPart, 1024 / CAPI_BIT_LENGTH) == 1) return -1;

	if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
	{
		Exponent = 0;
		Shift = 52;
		goto skip_IntPart;
	}

	Shift = 1024 - (U32)capi_CountLZ(IntPart, 1024 / CAPI_BIT_LENGTH);
	Exponent = (U64)Shift + 0x3FE;
	Shift += 11;

	if (Shift <= 64)
	{
		Shift = (64 - Shift);
		Mantissa = *(U64*)&IntPart << Shift;

	skip_IntPart:

		if (I < 0)
		{
			TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);
			goto exit_func;
		}

		capi_memcopy(TempDown, FracPart, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));

		Shift--;

		if (Exponent == 0)
		{
			Exponent = 0x3FF;
		move_decimal:
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			if (Exponent != 0) Exponent -= 1;
			else Shift -= 1;

			if (capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH) == 1) goto move_decimal;
			else
			{
				if (Exponent == 0)
				{
					Mantissa |= (1LL << Shift);
					Shift -= 1;
				}
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		for (BitSet = 1LL << Shift; BitSet != 0; BitSet >>= 1)
		{
			capi_MulReturnOverflow(FracPart, 2, 3584 / CAPI_BIT_LENGTH);
			RetStatus = capi_Compare(ieee754_Double_OnesPlace, FracPart, 3584 / CAPI_BIT_LENGTH);
			if ((RetStatus == -1) || (RetStatus == 0))
			{
				Mantissa |= BitSet;
				capi_SubReturnBorrowEx(FracPart, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
			}
		}

		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memcopy(FracPart, TempDown, sizeof(size_t) * (3584 / CAPI_BIT_LENGTH));
			ieee754_GetFixedFracFromDouble(TempDown, TempResult);

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				if (capi_Compare(TempDown, FracPart, 3584 / CAPI_BIT_LENGTH) == -1) TempResult++;
			}
			else
			{
				ieee754_GetFixedFracFromDouble(TempUp, TempResult + 1);
				if (*(U64*)&IntPart != ieee754_GetIntPartFromDouble(TempResult + 1))
				{
					capi_AddReturnCarryEx(TempUp, ieee754_Double_OnesPlace, 3584 / CAPI_BIT_LENGTH);
				}

				capi_SubReturnBorrowEx(TempUp, FracPart, 3584 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(FracPart, TempDown, 3584 / CAPI_BIT_LENGTH);

				if (capi_Compare(FracPart, TempUp, 3584 / CAPI_BIT_LENGTH) == 1) TempResult++;
			}
		}
	}
	else
	{
		Shift -= 64;

		Mantissa = ieee754_GetValueFromSHR(IntPart, Shift, 1024 / CAPI_BIT_LENGTH);
		TempResult = ((Exponent & 0x7FF) << 52) | (Mantissa & 0xFFFFFFFFFFFFF);

		if (((Flags & SCPR_ROUND_MASK) != SCAN_DOWN) && ((Flags & SCPR_ROUND_MASK) != SCAN_TRUNCATE(0)))
		{
			capi_memset(TempDown, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

			if ((Flags & SCPR_ROUND_MASK) == SCAN_UP)
			{
				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				if (capi_Compare(TempDown, IntPart, 1024 / CAPI_BIT_LENGTH) == -1) TempResult += 1;
			}
			else
			{
				capi_memset(TempUp, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));

				ieee754_SetValueFromSHL((U32*)TempDown, Mantissa, Shift, 0, 32);
				ieee754_SetValueFromSHL((U32*)TempUp, Mantissa + 1, Shift, 0, 32);

				capi_SubReturnBorrowEx(TempUp, IntPart, 1024 / CAPI_BIT_LENGTH);
				capi_SubReturnBorrowEx(IntPart, TempDown, 1024 / CAPI_BIT_LENGTH);

				if (capi_Compare(IntPart, TempUp, 1024 / CAPI_BIT_LENGTH) == 1) TempResult += 1;
			}
		}
	}

exit_func:

	*(U64*)pResult = Sign | TempResult;

	if (ppNewPos != 0) *ppNewPos = (UTF32*)pSource;

	return 0;
}
