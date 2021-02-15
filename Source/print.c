
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
#include "ieee754.h"

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

CAPI_FUNC(size_t) capi_PrintFloatA(ASCII* pBuffer, size_t Length, float Value, U32 Format)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[192 / CAPI_BIT_LENGTH];

	U32 Exponent, Mantissa, Shift, eCnt, eSign;
	ASCII TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U32*)&Value >> 23) & 0xFF;
	Mantissa = *(U32*)&Value & 0x7FFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0xFF)
	{
		if (*(U32*)&Value >> 31)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x400000) Mantissa &= 0x3FFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseA(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseA(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (192 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFF82;
	}
	else
	{
		Exponent -= 0x7F;
		Mantissa += 0x800000;
	}

	Shift = Exponent + 9;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 4);

	Shift = (23 - Exponent) - 32;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0xC0 - Shift, -1, 6);

	if (*(U32*)&Value >> 31)
	{
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
			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (ASCII)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (ASCII)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (ASCII)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (ASCII)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseA(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintDoubleA(ASCII* pBuffer, size_t Length, double Value, U32 Format)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[1088 / CAPI_BIT_LENGTH];

	U64 Mantissa;

	U32 Exponent, Shift, eCnt, eSign;
	ASCII TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U64*)&Value >> 52) & 0x7FF;
	Mantissa = *(U64*)&Value & 0xFFFFFFFFFFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0x7FF)
	{
		if (*(U64*)&Value >> 63)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x8000000000000) Mantissa &= 0x7FFFFFFFFFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseA(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseA(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (1088 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFC02;
	}
	else
	{
		Exponent -= 0x3FF;
		Mantissa += 0x10000000000000;
	}

	Shift = Exponent - 20;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 32);

	Shift = 52 - Exponent;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0x460 - Shift, -1, 34);

	if (*(U64*)&Value >> 63)
	{
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
			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (ASCII)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (ASCII)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (ASCII)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseA(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (ASCII)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (ASCII)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseA(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintFloatW(UTF16* pBuffer, size_t Length, float Value, U32 Format)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[192 / CAPI_BIT_LENGTH];

	U32 Exponent, Mantissa, Shift, eCnt, eSign;
	UTF16 TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U32*)&Value >> 23) & 0xFF;
	Mantissa = *(U32*)&Value & 0x7FFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0xFF)
	{
		if (*(U32*)&Value >> 31)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x400000) Mantissa &= 0x3FFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseW(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseW(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (192 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFF82;
	}
	else
	{
		Exponent -= 0x7F;
		Mantissa += 0x800000;
	}

	Shift = Exponent + 9;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 4);

	Shift = (23 - Exponent) - 32;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0xC0 - Shift, -1, 6);

	if (*(U32*)&Value >> 31)
	{
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
			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (UTF16)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (UTF16)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (UTF16)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (UTF16)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseW(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintDoubleW(UTF16* pBuffer, size_t Length, double Value, U32 Format)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[1088 / CAPI_BIT_LENGTH];

	U64 Mantissa;

	U32 Exponent, Shift, eCnt, eSign;
	UTF16 TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U64*)&Value >> 52) & 0x7FF;
	Mantissa = *(U64*)&Value & 0xFFFFFFFFFFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0x7FF)
	{
		if (*(U64*)&Value >> 63)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x8000000000000) Mantissa &= 0x7FFFFFFFFFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseW(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseW(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (1088 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFC02;
	}
	else
	{
		Exponent -= 0x3FF;
		Mantissa += 0x10000000000000;
	}

	Shift = Exponent - 20;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 32);

	Shift = 52 - Exponent;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0x460 - Shift, -1, 34);

	if (*(U64*)&Value >> 63)
	{
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
			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (UTF16)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (UTF16)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF16)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseW(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (UTF16)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (UTF16)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseW(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintFloatL(UTF32* pBuffer, size_t Length, float Value, U32 Format)
{
	size_t IntPart[128 / CAPI_BIT_LENGTH];
	size_t FracPart[192 / CAPI_BIT_LENGTH];

	U32 Exponent, Mantissa, Shift, eCnt, eSign;
	UTF32 TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U32*)&Value >> 23) & 0xFF;
	Mantissa = *(U32*)&Value & 0x7FFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0xFF)
	{
		if (*(U32*)&Value >> 31)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x400000) Mantissa &= 0x3FFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseL(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseL(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (128 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (192 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFF82;
	}
	else
	{
		Exponent -= 0x7F;
		Mantissa += 0x800000;
	}

	Shift = Exponent + 9;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 4);

	Shift = (23 - Exponent) - 32;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0xC0 - Shift, -1, 6);

	if (*(U32*)&Value >> 31)
	{
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
			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (UTF32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (UTF32)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 128 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 128 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (UTF32)capi_MulReturnOverflow(FracPart, 10, 192 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 192 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (UTF32)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseL(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintDoubleL(UTF32* pBuffer, size_t Length, double Value, U32 Format)
{
	size_t IntPart[1024 / CAPI_BIT_LENGTH];
	size_t FracPart[1088 / CAPI_BIT_LENGTH];

	U64 Mantissa;

	U32 Exponent, Shift, eCnt, eSign;
	UTF32 TpMarker, Decimal, HexCase;

	size_t I, nChars, Len;

	if ((pBuffer == 0) || (Length == 0)) return -1;
	Len = Length;

	Exponent = (*(U64*)&Value >> 52) & 0x7FF;
	Mantissa = *(U64*)&Value & 0xFFFFFFFFFFFFF;

	I = 0;
	nChars = 0;

	Decimal = 0x2E;
	if (Format & PRINT_DP_COMMA) Decimal = 0x2C;

	HexCase = 0x57;
	if (Format & PRINT_UPPERCASE) HexCase = 0x37;

	eSign = 0;

	if (Exponent == 0x7FF)
	{
		if (*(U64*)&Value >> 63)
		{
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
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}

		if (Mantissa == 0)
		{
			if (Len < 4) goto length_error;
			Len -= 3;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'I';
				pBuffer[1] = 'N';
				pBuffer[2] = 'F';
			}
			else
			{
				pBuffer[0] = 'i';
				pBuffer[1] = 'n';
				pBuffer[2] = 'f';
			}

			nChars += 3;
			pBuffer += 3;

			goto exit_func;
		}
		else
		{
			if (Len < 5) goto length_error;
			Len -= 4;

			if (Format & PRINT_FCAP)
			{
				pBuffer[0] = 'Q';
				pBuffer[1] = 'N';
				pBuffer[2] = 'A';
				pBuffer[3] = 'N';
			}
			else
			{
				pBuffer[0] = 'q';
				pBuffer[1] = 'n';
				pBuffer[2] = 'a';
				pBuffer[3] = 'n';
			}

			if (Mantissa & 0x8000000000000) Mantissa &= 0x7FFFFFFFFFFFF;
			else pBuffer[0] += 2;

			nChars += 4;
			pBuffer += 4;

			if (!(Format & PRINT_PAYLOAD)) goto exit_func;

			if (Len < 2) goto length_error;
			Len--;

			pBuffer[0] = Decimal;
			nChars++;
			pBuffer++;

			if (Format & PRINT_HEX)
			{
				if (Format & PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x30;
					pBuffer++;

					if ((Format & PRINT_H_PREFIX) == PRINT_x_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x78;
						pBuffer++;
					}
					else if ((Format & PRINT_H_PREFIX) == PRINT_X_PREFIX)
					{
						if (Len < 2) goto length_error;
						Len--;

						nChars += 1;
						*pBuffer = 0x58;
						pBuffer++;
					}
				}

				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = Mantissa & 0xF;
					if (pBuffer[I] < 10) pBuffer[I] += 0x30;
					else pBuffer[I] += HexCase;
					Mantissa >>= 4;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseL(pBuffer);
				pBuffer += I;

				if ((Format & PRINT_H_PREFIX) == PRINT_h_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x68;
					pBuffer++;
				}
				else if ((Format & PRINT_H_PREFIX) == PRINT_H_PREFIX)
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					*pBuffer = 0x48;
					pBuffer++;
				}
			}
			else
			{
				do
				{
					if (Len < 2) goto length_error;
					Len--;

					nChars += 1;
					pBuffer[I] = (Mantissa % 10) + 0x30;
					Mantissa /= 10;
					I++;
				} while (Mantissa != 0);

				pBuffer[I] = 0;
				capi_StrReverseL(pBuffer);
				pBuffer += I;
			}

			goto exit_func;
		}
	}

	capi_memset(IntPart, 0, sizeof(size_t) * (1024 / CAPI_BIT_LENGTH));
	capi_memset(FracPart, 0, sizeof(size_t) * (1088 / CAPI_BIT_LENGTH));

	if (Exponent == 0)
	{
		Exponent = 0xFFFFFC02;
	}
	else
	{
		Exponent -= 0x3FF;
		Mantissa += 0x10000000000000;
	}

	Shift = Exponent - 20;
	ieee754_SetValueFromSHL((U32*)IntPart, Mantissa, Shift, -1, 32);

	Shift = 52 - Exponent;
	ieee754_SetValueFromSHL((U32*)FracPart, Mantissa, 0x460 - Shift, -1, 34);

	if (*(U64*)&Value >> 63)
	{
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
			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) | capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH))
			{
				if (Len < 2) goto length_error;
				Len--;

				nChars += 1;
				*pBuffer = 0x2B;
				pBuffer++;
			}
		}
	}

	if (Format & PRINT_e_ENABLE)
	{
		eCnt = 0;

		if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH)) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0))
		{
		test_eInt:
			*pBuffer = (UTF32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0)
			{
				eCnt++;
				goto test_eInt;
			}

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[0] += 0x30;
			I++;

			if (eCnt != 0) eSign = 0x2B;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0)
			{
				pBuffer++;
				goto intPart_done;
			}
		}
		else if ((capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) && (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH)))
		{
		test_eFrac:
			eCnt++;
			*pBuffer = (UTF32)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH);
			if (*pBuffer == 0) goto test_eFrac;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer += 0x30;
			pBuffer++;
			eSign = 0x2D;

			goto intPart_done;
		}
	}

	if ((Format & PRINT_SPACE) == 0)
	{
		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;
		} while (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH));
	}
	else
	{
		TpMarker = 32;
		if ((Format & PRINT_SPACE) == PRINT_COMMA)
		{
			Decimal = 0x2E;
			TpMarker = 0x2C;
		}
		else if ((Format & PRINT_SPACE) == PRINT_PERIOD)
		{
			Decimal = 0x2C;
			TpMarker = 0x2E;
		}

		if (I != 0) goto skip_ones_place;

		while (TRUE)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

		skip_ones_place:

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (UTF32)capi_DivReturnRemainder(IntPart, 10, 1024 / CAPI_BIT_LENGTH) + 0x30;
			I++;

			if (capi_TestForZero(IntPart, 1024 / CAPI_BIT_LENGTH) == 0) break;

			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = TpMarker;
			I++;
		}
	}

	pBuffer[I] = 0;
	capi_StrReverseL(pBuffer);
	pBuffer += I;

intPart_done:

	I = Format >> 24;

	if (Format & PRINT_EXACT(0))
	{
		if (I == 0) goto exit_func;
	}
	else
	{
		if (Format & PRINT_ZEROF) goto skip_zero_test_l3;
		if (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH) == 0) goto exit_func;

	skip_zero_test_l3:;
	}

	if (Len < 2) goto length_error;
	Len--;

	nChars += 1;
	*pBuffer = Decimal;
	pBuffer++;

	do
	{
		if (Len < 2) goto length_error;
		Len--;

		nChars += 1;
		*pBuffer = (UTF32)capi_MulReturnOverflow(FracPart, 10, 1088 / CAPI_BIT_LENGTH) + 0x30;
		pBuffer++;

		if (I != 0)
		{
			I -= 1;
			if (I == 0) break;
		}
	} while (capi_TestForZero(FracPart, 1088 / CAPI_BIT_LENGTH));

	if (Format & PRINT_EXACT(0))
	{
		while (I != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			*pBuffer = 0x30;
			pBuffer++;
			I -= 1;
		}
	}
	else
	{
	clear_char:

		pBuffer--;

		if (*pBuffer == 0x30)
		{
			nChars -= 1;
			goto clear_char;
		}

		if (*pBuffer != Decimal) pBuffer++;
		else
		{
			if (!(Format & PRINT_ZEROF)) nChars -= 1;
			else
			{
				pBuffer++;
				*pBuffer = 0x30;
				pBuffer++;
				nChars += 1;
			}
		}
	}

exit_func:

	if (eSign != 0)
	{
		if (Len < 3) goto length_error;
		Len -= 2;

		nChars += 2;

		if ((Format & PRINT_E_ENABLE) == PRINT_e_ENABLE) pBuffer[0] = 0x65;
		else pBuffer[0] = 0x45;

		pBuffer[1] = (UTF32)eSign;
		pBuffer += 2;

		I = 0;

		do
		{
			if (Len < 2) goto length_error;
			Len--;

			nChars += 1;
			pBuffer[I] = (eCnt % 10) + 0x30;
			eCnt /= 10;
			I++;
		} while (eCnt != 0);

		pBuffer[I] = 0;
		capi_StrReverseL(pBuffer);
		pBuffer += I;
	}

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}