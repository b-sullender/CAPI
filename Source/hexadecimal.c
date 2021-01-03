
/******************************************************************************************
*
* Name: hexadecimal.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines hexadecimal string functions.
*
*******************************************************************************************/

#include "CAPI.h"

CAPI_FUNC(size_t) capi_PrintHexA(ASCII* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	U8 ABCDEF_Case;
	U8* ThisByte;
	size_t I, U, nChars, Len;
	U8 ThisChar;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	ABCDEF_Case = 0x57;
	if (Format & PRINT_UPPERCASE) ABCDEF_Case = 0x37;

	U = I = nBytes * 2;
	ThisByte = (U8*)pValue + nBytes - 1;

	while (I != 0)
	{
		if (*ThisByte >> 4) break;
		I--;
		if (*ThisByte & 15) break;
		I--;
		ThisByte--;
	}

	if (I == 0) I = 1;

	U -= I;
	nChars = I;

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

	if (Format & PRINT_LZEROS)
	{
		nChars += U;

		while (U != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			U--;
			*pBuffer = 0x30;
			pBuffer++;
		}
	}

	if (I & 1)
	{
		ThisChar = *ThisByte;
		goto get_low_digit;
	}

	while (I != 0)
	{
		ThisChar = *ThisByte;

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar >> 4) < 10) *pBuffer = (ThisChar >> 4) + 0x30;
		else *pBuffer = (ThisChar >> 4) + ABCDEF_Case;
		pBuffer++;

		I--;
		if (I == 0) break;

	get_low_digit:

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar & 15) < 10) *pBuffer = (ThisChar & 15) + 0x30;
		else *pBuffer = (ThisChar & 15) + ABCDEF_Case;
		pBuffer++;

		I--;
		ThisByte--;
	}

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

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintHexU(UTF8* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	return capi_PrintHexA(pBuffer, Length, pValue, Format, nBytes);
}

CAPI_FUNC(size_t) capi_PrintHexW(UTF16* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	U8 ABCDEF_Case;
	U8* ThisByte;
	size_t I, U, nChars, Len;
	U8 ThisChar;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	ABCDEF_Case = 0x57;
	if (Format & PRINT_UPPERCASE) ABCDEF_Case = 0x37;

	U = I = nBytes * 2;
	ThisByte = (U8*)pValue + nBytes - 1;

	while (I != 0)
	{
		if (*ThisByte >> 4) break;
		I--;
		if (*ThisByte & 15) break;
		I--;
		ThisByte--;
	}

	if (I == 0) I = 1;

	U -= I;
	nChars = I;

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

	if (Format & PRINT_LZEROS)
	{
		nChars += U;

		while (U != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			U--;
			*pBuffer = 0x30;
			pBuffer++;
		}
	}

	if (I & 1)
	{
		ThisChar = *ThisByte;
		goto get_low_digit;
	}

	while (I != 0)
	{
		ThisChar = *ThisByte;

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar >> 4) < 10) *pBuffer = (ThisChar >> 4) + 0x30;
		else *pBuffer = (ThisChar >> 4) + ABCDEF_Case;
		pBuffer++;

		I--;
		if (I == 0) break;

	get_low_digit:

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar & 15) < 10) *pBuffer = (ThisChar & 15) + 0x30;
		else *pBuffer = (ThisChar & 15) + ABCDEF_Case;
		pBuffer++;

		I--;
		ThisByte--;
	}

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

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}

CAPI_FUNC(size_t) capi_PrintHexL(UTF32* pBuffer, size_t Length, void* pValue, U32 Format, size_t nBytes)
{
	U8 ABCDEF_Case;
	U8* ThisByte;
	size_t I, U, nChars, Len;
	U8 ThisChar;

	if ((pBuffer == 0) || (Length == 0) || (pValue == 0) || (nBytes == 0)) return -1;
	Len = Length;

	ABCDEF_Case = 0x57;
	if (Format & PRINT_UPPERCASE) ABCDEF_Case = 0x37;

	U = I = nBytes * 2;
	ThisByte = (U8*)pValue + nBytes - 1;

	while (I != 0)
	{
		if (*ThisByte >> 4) break;
		I--;
		if (*ThisByte & 15) break;
		I--;
		ThisByte--;
	}

	if (I == 0) I = 1;

	U -= I;
	nChars = I;

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

	if (Format & PRINT_LZEROS)
	{
		nChars += U;

		while (U != 0)
		{
			if (Len < 2) goto length_error;
			Len--;

			U--;
			*pBuffer = 0x30;
			pBuffer++;
		}
	}

	if (I & 1)
	{
		ThisChar = *ThisByte;
		goto get_low_digit;
	}

	while (I != 0)
	{
		ThisChar = *ThisByte;

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar >> 4) < 10) *pBuffer = (ThisChar >> 4) + 0x30;
		else *pBuffer = (ThisChar >> 4) + ABCDEF_Case;
		pBuffer++;

		I--;
		if (I == 0) break;

	get_low_digit:

		if (Len < 2) goto length_error;
		Len--;

		if ((ThisChar & 15) < 10) *pBuffer = (ThisChar & 15) + 0x30;
		else *pBuffer = (ThisChar & 15) + ABCDEF_Case;
		pBuffer++;

		I--;
		ThisByte--;
	}

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

	*pBuffer = 0;

	return nChars;

length_error:
	*pBuffer = 0;

	return Length;
}
