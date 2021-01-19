
/******************************************************************************************
*
* Name: scan.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines functions to convert a readable string into a data variable.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(I8) capi_ScanSignedA(void* pResult, ASCII* pSource, U32 Flags, ASCII** ppNewPos, U32 nBytes)
{
	U32 Count, TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	I8 IsNegative, RetStatus;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;
	IsNegative = FALSE;
	RetStatus = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x2D)
	{
		IsNegative = TRUE;
		pSource++;
	}

	if (*pSource == 0x2B)
	{
		if (IsNegative != FALSE) goto exit_invalid_error;
		pSource++;
	}

	if (*pSource == 0x30)
	{
		if (Flags & SCAN_HEX)
		{
			if ((pSource[1] == 0x78) || (pSource[1] == 0x58))
			{
				RetStatus = capi_ScanHexA(TempValue, pSource, Flags, &pSource, nBytes);
				if ((Flags & SCAN_MAX_HEX) && (IsNegative == FALSE)) goto exit_ScanSigned;

				goto test_sign;
			}
		}
	}

	while ((*pSource >= 0x30) && (*pSource <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;
		pSource++;
	}

	if ((Flags & SCAN_SPACE) && (*pSource == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

			while (*pSource == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;

				for (I = 0; I < 3; I++)
				{
					if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

					nDigits++;
					pSource++;
				}

				if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (*pSource == 0x2C)) || ((Flags & SCAN_PERIOD) && (*pSource == 0x2E)))
	{
		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		TpMarker = *pSource;

		while (*pSource == TpMarker)
		{
			pSource++;

			for (I = 0; I < 3; I++)
			{
				if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

				nDigits++;
				pSource++;
			}

			if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
		}
	}

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) goto exit_invalid_error;
	}

test_sign:

	if (IsNegative == FALSE)
	{
		if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}
	else
	{
		capi_Negate(TempValue, Count);
		if (capi_TestForZero(TempValue, Count) == 0) goto exit_invalid_error;
		if (capi_CountL1(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}

exit_ScanSigned:

	capi_memcopy(pResult, TempValue, nBytes);

	if (RetStatus == 0)
	{
		if (ppNewPos != 0) *ppNewPos = pSource;
	}

	if (TempValue != LocalValue) capi_free(TempValue);

	return RetStatus;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanSignedU(void* pResult, UTF8* pSource, U32 Flags, UTF8** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 Count, CodePoint, NextCodePoint, TpMarker, Fullwidth_TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	I8 IsNegative, RetStatus;
	UTF8* pNextChar;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;
	IsNegative = FALSE;
	RetStatus = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

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
		IsNegative = TRUE;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (IsNegative != FALSE) goto exit_invalid_error;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

		if (Flags & SCAN_HEX)
		{
			if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
			{
				RetStatus = capi_ScanHexU(TempValue, pSource, Flags, &pSource, nBytes);
				if ((Flags & SCAN_MAX_HEX) && (IsNegative == FALSE)) goto exit_ScanSigned;

				goto test_sign;
			}
		}
	}

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0xFF10, Count) != 0) goto exit_range_error;

		nDigits++;

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
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

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
					else goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

					nDigits++;

					pSource += CharUnits;
					CharUnits = capi_UTF8_GetCharUnits(*pSource);
					CodePoint = capi_UTF8_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
			}
		}
	}
	else
	{
		switch (CodePoint)
		{
		case 0x2C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0C;
			break;
		case 0x2E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0E;
			break;
		case 0xFF0C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2C;
			break;
		case 0xFF0E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2E;
			break;
		default:
			goto result_done;
		}

		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

				nDigits++;

				pSource += CharUnits;
				CharUnits = capi_UTF8_GetCharUnits(*pSource);
				CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
		}
	}

result_done:

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}
		if (CodePoint != 0) goto exit_invalid_error;
	}

test_sign:

	if (IsNegative == FALSE)
	{
		if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}
	else
	{
		capi_Negate(TempValue, Count);
		if (capi_TestForZero(TempValue, Count) == 0) goto exit_invalid_error;
		if (capi_CountL1(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}

exit_ScanSigned:

	capi_memcopy(pResult, TempValue, nBytes);

	if (RetStatus == 0)
	{
		if (ppNewPos != 0) *ppNewPos = pSource;
	}

	if (TempValue != LocalValue) capi_free(TempValue);

	return RetStatus;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanSignedW(void* pResult, UTF16* pSource, U32 Flags, UTF16** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 Count, CodePoint, NextCodePoint, TpMarker, Fullwidth_TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	I8 IsNegative, RetStatus;
	UTF16* pNextChar;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;
	IsNegative = FALSE;
	RetStatus = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

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
		IsNegative = TRUE;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		if (IsNegative != FALSE) goto exit_invalid_error;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

		if (Flags & SCAN_HEX)
		{
			if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
			{
				RetStatus = capi_ScanHexW(TempValue, pSource, Flags, &pSource, nBytes);
				if ((Flags & SCAN_MAX_HEX) && (IsNegative == FALSE)) goto exit_ScanSigned;

				goto test_sign;
			}
		}
	}

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0xFF10, Count) != 0) goto exit_range_error;

		nDigits++;

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
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

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
					else goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

					nDigits++;

					pSource += CharUnits;
					CharUnits = capi_UTF16_GetCharUnits(*pSource);
					CodePoint = capi_UTF16_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
			}
		}
	}
	else
	{
		switch (CodePoint)
		{
		case 0x2C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0C;
			break;
		case 0x2E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0E;
			break;
		case 0xFF0C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2C;
			break;
		case 0xFF0E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2E;
			break;
		default:
			goto result_done;
		}

		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

				nDigits++;

				pSource += CharUnits;
				CharUnits = capi_UTF16_GetCharUnits(*pSource);
				CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
		}
	}

result_done:

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}
		if (CodePoint != 0) goto exit_invalid_error;
	}

test_sign:

	if (IsNegative == FALSE)
	{
		if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}
	else
	{
		capi_Negate(TempValue, Count);
		if (capi_TestForZero(TempValue, Count) == 0) goto exit_invalid_error;
		if (capi_CountL1(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}

exit_ScanSigned:

	capi_memcopy(pResult, TempValue, nBytes);

	if (RetStatus == 0)
	{
		if (ppNewPos != 0) *ppNewPos = pSource;
	}

	if (TempValue != LocalValue) capi_free(TempValue);

	return RetStatus;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanSignedL(void* pResult, UTF32* pSource, U32 Flags, UTF32** ppNewPos, U32 nBytes)
{
	U32 Count, TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	I8 IsNegative, RetStatus;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;
	IsNegative = FALSE;
	RetStatus = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x2D)
	{
		IsNegative = TRUE;
		pSource++;
	}

	if (*pSource == 0x2B)
	{
		if (IsNegative != FALSE) goto exit_invalid_error;
		pSource++;
	}

	if (*pSource == 0x30)
	{
		if (Flags & SCAN_HEX)
		{
			if ((pSource[1] == 0x78) || (pSource[1] == 0x58))
			{
				RetStatus = capi_ScanHexL(TempValue, pSource, Flags, &pSource, nBytes);
				if ((Flags & SCAN_MAX_HEX) && (IsNegative == FALSE)) goto exit_ScanSigned;

				goto test_sign;
			}
		}
	}

	while ((*pSource >= 0x30) && (*pSource <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;
		pSource++;
	}

	if ((Flags & SCAN_SPACE) && (*pSource == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

			while (*pSource == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;

				for (I = 0; I < 3; I++)
				{
					if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

					nDigits++;
					pSource++;
				}

				if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (*pSource == 0x2C)) || ((Flags & SCAN_PERIOD) && (*pSource == 0x2E)))
	{
		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		TpMarker = *pSource;

		while (*pSource == TpMarker)
		{
			pSource++;

			for (I = 0; I < 3; I++)
			{
				if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

				nDigits++;
				pSource++;
			}

			if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
		}
	}

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) goto exit_invalid_error;
	}

test_sign:

	if (IsNegative == FALSE)
	{
		if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}
	else
	{
		capi_Negate(TempValue, Count);
		if (capi_TestForZero(TempValue, Count) == 0) goto exit_invalid_error;
		if (capi_CountL1(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - ((8 * nBytes) - 1))) goto exit_range_error;
	}

exit_ScanSigned:

	capi_memcopy(pResult, TempValue, nBytes);

	if (RetStatus == 0)
	{
		if (ppNewPos != 0) *ppNewPos = pSource;
	}

	if (TempValue != LocalValue) capi_free(TempValue);

	return RetStatus;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanUnsignedA(void* pResult, ASCII* pSource, U32 Flags, ASCII** ppNewPos, U32 nBytes)
{
	U32 Count, TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x2B) pSource++;

	if (*pSource == 0x30)
	{
		if (Flags & SCAN_HEX)
		{
			if ((pSource[1] == 0x78) || (pSource[1] == 0x58)) return capi_ScanHexA(pResult, pSource, Flags, ppNewPos, nBytes);
		}
	}

	while ((*pSource >= 0x30) && (*pSource <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;
		pSource++;
	}

	if ((Flags & SCAN_SPACE) && (*pSource == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

			while (*pSource == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;

				for (I = 0; I < 3; I++)
				{
					if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

					nDigits++;
					pSource++;
				}

				if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (*pSource == 0x2C)) || ((Flags & SCAN_PERIOD) && (*pSource == 0x2E)))
	{
		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		TpMarker = *pSource;

		while (*pSource == TpMarker)
		{
			pSource++;

			for (I = 0; I < 3; I++)
			{
				if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

				nDigits++;
				pSource++;
			}

			if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
		}
	}

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) goto exit_invalid_error;
	}

	if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - (8 * nBytes))) goto exit_range_error;
	capi_memcopy(pResult, TempValue, nBytes);

	if (ppNewPos != 0) *ppNewPos = pSource;

	if (TempValue != LocalValue) capi_free(TempValue);

	return 0;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanUnsignedU(void* pResult, UTF8* pSource, U32 Flags, UTF8** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 Count, CodePoint, NextCodePoint, TpMarker, Fullwidth_TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	UTF8* pNextChar;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	CharUnits = capi_UTF8_GetCharUnits(*pSource);
	CodePoint = capi_UTF8_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF8_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF8_Decode(NextCharUnits, pNextChar);

		if (Flags & SCAN_HEX)
		{
			if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
			{
				return capi_ScanHexU(pResult, pSource, Flags, ppNewPos, nBytes);
			}
		}
	}

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0xFF10, Count) != 0) goto exit_range_error;

		nDigits++;

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
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

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
					else goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

					nDigits++;

					pSource += CharUnits;
					CharUnits = capi_UTF8_GetCharUnits(*pSource);
					CodePoint = capi_UTF8_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
			}
		}
	}
	else
	{
		switch (CodePoint)
		{
		case 0x2C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0C;
			break;
		case 0x2E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0E;
			break;
		case 0xFF0C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2C;
			break;
		case 0xFF0E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2E;
			break;
		default:
			goto result_done;
		}

		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

				nDigits++;

				pSource += CharUnits;
				CharUnits = capi_UTF8_GetCharUnits(*pSource);
				CodePoint = capi_UTF8_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
		}
	}

result_done:

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}
		if (CodePoint != 0) goto exit_invalid_error;
	}

	if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - (8 * nBytes))) goto exit_range_error;
	capi_memcopy(pResult, TempValue, nBytes);

	if (ppNewPos != 0) *ppNewPos = pSource;

	if (TempValue != LocalValue) capi_free(TempValue);

	return 0;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanUnsignedW(void* pResult, UTF16* pSource, U32 Flags, UTF16** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 Count, CodePoint, NextCodePoint, TpMarker, Fullwidth_TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;
	UTF16* pNextChar;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	CharUnits = capi_UTF16_GetCharUnits(*pSource);
	CodePoint = capi_UTF16_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x2B) || (CodePoint == 0xFE62) || (CodePoint == 0xFF0B))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	if ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pNextChar = pSource + CharUnits;
		NextCharUnits = capi_UTF16_GetCharUnits(*pNextChar);
		NextCodePoint = capi_UTF16_Decode(NextCharUnits, pNextChar);

		if (Flags & SCAN_HEX)
		{
			if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
			{
				return capi_ScanHexW(pResult, pSource, Flags, ppNewPos, nBytes);
			}
		}
	}

get_integer:

	if ((CodePoint >= 0x30) && (CodePoint <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);

		goto get_integer;
	}

	if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, CodePoint - 0xFF10, Count) != 0) goto exit_range_error;

		nDigits++;

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
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

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
					else goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

					nDigits++;

					pSource += CharUnits;
					CharUnits = capi_UTF16_GetCharUnits(*pSource);
					CodePoint = capi_UTF16_Decode(CharUnits, pSource);
				}

				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
				if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
			}
		}
	}
	else
	{
		switch (CodePoint)
		{
		case 0x2C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0C;
			break;
		case 0x2E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			TpMarker = CodePoint;
			Fullwidth_TpMarker = 0xFF0E;
			break;
		case 0xFF0C:
			if (!(Flags & SCAN_COMMA)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2C;
			break;
		case 0xFF0E:
			if (!(Flags & SCAN_PERIOD)) goto result_done;
			Fullwidth_TpMarker = CodePoint;
			TpMarker = 0x2E;
			break;
		default:
			goto result_done;
		}

		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		while ((CodePoint == TpMarker) || (CodePoint == Fullwidth_TpMarker))
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);

			for (I = 0; I < 3; I++)
			{
				if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30;
				else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10;
				else goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, CodePoint, Count) != 0) goto exit_range_error;

				nDigits++;

				pSource += CharUnits;
				CharUnits = capi_UTF16_GetCharUnits(*pSource);
				CodePoint = capi_UTF16_Decode(CharUnits, pSource);
			}

			if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) goto exit_invalid_error;
			if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) goto exit_invalid_error;
		}
	}

result_done:

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (CodePoint == 0x20)
		{
			pSource += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}
		if (CodePoint != 0) goto exit_invalid_error;
	}

	if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - (8 * nBytes))) goto exit_range_error;
	capi_memcopy(pResult, TempValue, nBytes);

	if (ppNewPos != 0) *ppNewPos = pSource;

	if (TempValue != LocalValue) capi_free(TempValue);

	return 0;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}

CAPI_FUNC(I8) capi_ScanUnsignedL(void* pResult, UTF32* pSource, U32 Flags, UTF32** ppNewPos, U32 nBytes)
{
	U32 Count, TpMarker, I;
	size_t LocalValue[8], * TempValue, nDigits;

	if ((pResult == 0) || (pSource == 0) || (nBytes == 0)) return 2;

	nDigits = 0;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return 2;
	}

	capi_memset(TempValue, 0, Count * sizeof(size_t));

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x2B) pSource++;

	if (*pSource == 0x30)
	{
		if (Flags & SCAN_HEX)
		{
			if ((pSource[1] == 0x78) || (pSource[1] == 0x58)) return capi_ScanHexL(pResult, pSource, Flags, ppNewPos, nBytes);
		}
	}

	while ((*pSource >= 0x30) && (*pSource <= 0x39))
	{
		if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
		if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

		nDigits++;
		pSource++;
	}

	if ((Flags & SCAN_SPACE) && (*pSource == 0x20))
	{
		if ((pSource[1] >= 0x30) && (pSource[1] <= 0x39))
		{
			if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

			while (*pSource == 0x20)
			{
				if ((pSource[1] < 0x30) || (pSource[1] > 0x39)) break;

				pSource++;

				for (I = 0; I < 3; I++)
				{
					if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

					if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
					if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

					nDigits++;
					pSource++;
				}

				if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
			}
		}
	}
	else if (((Flags & SCAN_COMMA) && (*pSource == 0x2C)) || ((Flags & SCAN_PERIOD) && (*pSource == 0x2E)))
	{
		if ((nDigits == 0) || (nDigits > 3)) goto exit_invalid_error;

		TpMarker = *pSource;

		while (*pSource == TpMarker)
		{
			pSource++;

			for (I = 0; I < 3; I++)
			{
				if ((*pSource < 0x30) || (*pSource > 0x39)) goto exit_invalid_error;

				if (capi_MulReturnOverflow(TempValue, 10, Count) != 0) goto exit_range_error;
				if (capi_AddReturnCarry(TempValue, *pSource - 0x30, Count) != 0) goto exit_range_error;

				nDigits++;
				pSource++;
			}

			if ((*pSource >= 0x30) && (*pSource <= 0x39)) goto exit_invalid_error;
		}
	}

	if (nDigits == 0) goto exit_invalid_error;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) goto exit_invalid_error;
	}

	if (capi_CountLZ(TempValue, Count) < ((Count * CAPI_BIT_LENGTH) - (8 * nBytes))) goto exit_range_error;
	capi_memcopy(pResult, TempValue, nBytes);

	if (ppNewPos != 0) *ppNewPos = pSource;

	if (TempValue != LocalValue) capi_free(TempValue);

	return 0;

exit_invalid_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return 1;

exit_range_error:
	if (TempValue != LocalValue) capi_free(TempValue);

	return -1;
}
