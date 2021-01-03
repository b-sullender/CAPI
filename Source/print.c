
/******************************************************************************************
*
* Name: print.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines string functions to convert data variables into a readable string.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(size_t) capi_PrintUnsignedA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	ASCII TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_ZeroExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (Format & PRINT_POSITIVE)
	{
		if (capi_TestForZero(TempValue, Count) != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x2B;
			pBuffer++;
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexA(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}

CAPI_FUNC(size_t) capi_PrintSignedA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	ASCII TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_SignExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (TempValue[Count - 1] >> (CAPI_BIT_LENGTH - 1))
	{
		capi_Negate(TempValue, Count);

		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = 0x2D;
		pBuffer++;
	}
	else
	{
		if (Format & PRINT_POSITIVE)
		{
			if (capi_TestForZero(TempValue, Count) != 0)
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexA(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (ASCII)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}

CAPI_FUNC(size_t) capi_PrintUnsignedU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	return capi_PrintUnsignedA(pBuffer, Length, pValue, Format, nBytes);
}

CAPI_FUNC(size_t) capi_PrintSignedU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	return capi_PrintSignedA(pBuffer, Length, pValue, Format, nBytes);
}

CAPI_FUNC(size_t) capi_PrintUnsignedW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	UTF16 TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_ZeroExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (Format & PRINT_POSITIVE)
	{
		if (capi_TestForZero(TempValue, Count) != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x2B;
			pBuffer++;
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexW(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}

CAPI_FUNC(size_t) capi_PrintSignedW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	UTF16 TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_SignExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (TempValue[Count - 1] >> (CAPI_BIT_LENGTH - 1))
	{
		capi_Negate(TempValue, Count);

		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = 0x2D;
		pBuffer++;
	}
	else
	{
		if (Format & PRINT_POSITIVE)
		{
			if (capi_TestForZero(TempValue, Count) != 0)
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexW(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF16)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}

CAPI_FUNC(size_t) capi_PrintUnsignedL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	UTF32 TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_ZeroExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (Format & PRINT_POSITIVE)
	{
		if (capi_TestForZero(TempValue, Count) != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x2B;
			pBuffer++;
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexL(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}

CAPI_FUNC(size_t) capi_PrintSignedL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	size_t LocalValue[8], * TempValue, I, Count, nChars, Len, Result;
	UTF32 TpMarker;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	if ((nBytes == 1) || (nBytes == 2) || (nBytes == 4)) Count = 1;
	else Count = nBytes / sizeof(size_t);

	TempValue = LocalValue;

	if (Count > 8)
	{
		TempValue = capi_malloc(Count * sizeof(size_t));
		if (TempValue == 0) return -1;
	}

	capi_SignExtend(TempValue, Count, pValue, nBytes);

	I = 0;
	nChars = 0;

	if (TempValue[Count - 1] >> (CAPI_BIT_LENGTH - 1))
	{
		capi_Negate(TempValue, Count);

		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = 0x2D;
		pBuffer++;
	}
	else
	{
		if (Format & PRINT_POSITIVE)
		{
			if (capi_TestForZero(TempValue, Count) != 0)
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_HEX)
	{
		nChars += capi_PrintHexL(pBuffer, Len, TempValue, Format, nBytes);
		if (TempValue != LocalValue) capi_free(TempValue);

		return nChars;
	}

	if ((Format & PRINT_SPACE) == PRINT_COMMA) TpMarker = 0x2C;
	else if ((Format & PRINT_SPACE) == PRINT_PERIOD) TpMarker = 0x2E;
	else if ((Format & PRINT_SPACE) == PRINT_SPACE) TpMarker = 0x20;
	else TpMarker = 0;

	if (TpMarker == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;
		} while (capi_TestForZero(TempValue, Count) != 0);
	}
	else
	{
		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			Result = capi_DivReturnRemainder(TempValue, 10, Count);
			pBuffer[I] = (UTF32)Result + 0x30;
			I++;

			if (capi_TestForZero(TempValue, Count) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return nChars + I;

length_error:
	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);

	if (TempValue != LocalValue) capi_free(TempValue);

	return Length;
}
