
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

CAPI_SUBFUNC(void) hexadecimal_ShiftLeftBy4(void* pResult, U32 nBytes)
{
	U32 I;
	U8 was32shifted;

	if ((pResult == 0) || (nBytes == 0)) return;

	pResult = (U8*)pResult + nBytes;

	I = nBytes >> 2;

	if (I == 0) { was32shifted = FALSE; goto shift_32_done; }

	pResult = (U32*)pResult - 1;

	was32shifted = TRUE;

shift_32_loop:

	((U32*)pResult)[0] <<= 4;

	I--;

	if (I == 0) goto shift_32_done;

	pResult = (U32*)pResult - 1;

	((U32*)pResult)[1] |= (((U32*)pResult)[0] >> 28);

	goto shift_32_loop;

shift_32_done:

	I = nBytes & 3;

	if (I == 0) return;

	pResult = (U8*)pResult - 1;

	if (was32shifted == TRUE) ((U8*)pResult)[1] |= (((U8*)pResult)[0] >> 4);

shift_8_loop:

	((U8*)pResult)[0] <<= 4;

	I--;

	if (I == 0) return;

	pResult = (U8*)pResult - 1;

	((U8*)pResult)[1] |= (((U8*)pResult)[0] >> 4);

	goto shift_8_loop;
}

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

CAPI_FUNC(I8) capi_ScanHexA(void* pResult, const ASCII* pSource, U32 Flags, ASCII** ppNewPos, U32 nBytes)
{
	U32 CodePoint, MaxDigits, nDigitsRead, LeadingZeros;

	if ((pResult == 0) || (pSource == 0)) return 2;

	nDigitsRead = 0;
	LeadingZeros = 0;
	MaxDigits = nBytes * 2;

	capi_memset(pResult, 0, nBytes);

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x30)
	{
		if ((pSource[1] == 0x78) || (pSource[1] == 0x58)) pSource += 2;
	}

	while (*pSource == 0x30)
	{
		pSource++;
		LeadingZeros++;
	}

	while (*pSource != 0)
	{
		if (*pSource == 0x20) break;

		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30; // 0 - 9
		else if ((CodePoint >= 0x41) && (CodePoint <= 0x46)) CodePoint -= 0x37; // A - F
		else if ((CodePoint >= 0x61) && (CodePoint <= 0x66)) CodePoint -= 0x57; // a - f
		else
		{
			if (Flags & SCAN_STRICT) return 1;
			if ((CodePoint == 0x0A) || (CodePoint == 0x0D)) break; // LF or CR
			if ((CodePoint >= 0x21) && (CodePoint <= 0x2F)) break; // ! " # $ % & ' ( ) * + , - . /
			if ((CodePoint >= 0x3A) && (CodePoint <= 0x40)) break; // : ; < = > ? @
			if ((CodePoint >= 0x5B) && (CodePoint <= 0x60)) break; // [ \ ] ^ _ `
			if ((CodePoint >= 0x7B) && (CodePoint <= 0x7E)) break; // { | } ~
		}

		hexadecimal_ShiftLeftBy4(pResult, nBytes);
		*(U8*)pResult |= CodePoint;

		if (nDigitsRead == MaxDigits) return -1;
		nDigitsRead++;

		pSource++;
	}

	nDigitsRead += LeadingZeros;
	if (nDigitsRead == 0) return 1;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) return 1;
	}

	if (ppNewPos != 0) *ppNewPos = (ASCII*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanHexU(void* pResult, const UTF8* pSource, U32 Flags, UTF8** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 CodePoint, NextCodePoint, MaxDigits, nDigitsRead, LeadingZeros;
	const UTF8* pNextChar;

	if ((pResult == 0) || (pSource == 0)) return 2;

	nDigitsRead = 0;
	LeadingZeros = 0;
	MaxDigits = nBytes * 2;

	capi_memset(pResult, 0, nBytes);

	CharUnits = capi_UTF8_GetCharUnits(*pSource);
	CodePoint = capi_UTF8_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
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

		if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
		{
			pSource += CharUnits + NextCharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*pSource);
			CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		}
	}

	while ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
		LeadingZeros++;
	}

	while (CodePoint != 0)
	{
		if (CodePoint == 0x20) break;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30; // 0 - 9
		else if ((CodePoint >= 0x41) && (CodePoint <= 0x46)) CodePoint -= 0x37; // A - F
		else if ((CodePoint >= 0x61) && (CodePoint <= 0x66)) CodePoint -= 0x57; // a - f
		else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10; // 0 - 9
		else if ((CodePoint >= 0xFF21) && (CodePoint <= 0xFF3A)) CodePoint -= 0xFF17; // A - F
		else if ((CodePoint >= 0xFF41) && (CodePoint <= 0xFF5A)) CodePoint -= 0xFF37; // a - f
		else
		{
			if (Flags & SCAN_STRICT) return 1;
			if ((CodePoint == 0x0A) || (CodePoint == 0x0D)) break; // LF or CR
			if ((CodePoint >= 0x21) && (CodePoint <= 0x2F)) break; // ! " # $ % & ' ( ) * + , - . /
			if ((CodePoint >= 0x3A) && (CodePoint <= 0x40)) break; // : ; < = > ? @
			if ((CodePoint >= 0x5B) && (CodePoint <= 0x60)) break; // [ \ ] ^ _ `
			if ((CodePoint >= 0x7B) && (CodePoint <= 0x7E)) break; // { | } ~
		}

		hexadecimal_ShiftLeftBy4(pResult, nBytes);
		*(U8*)pResult |= CodePoint;

		if (nDigitsRead == MaxDigits) return -1;
		nDigitsRead++;

		pSource += CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*pSource);
		CodePoint = capi_UTF8_Decode(CharUnits, pSource);
	}

	nDigitsRead += LeadingZeros;
	if (nDigitsRead == 0) return 1;

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

	if (ppNewPos != 0) *ppNewPos = (UTF8*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanHexW(void* pResult, const UTF16* pSource, U32 Flags, UTF16** ppNewPos, U32 nBytes)
{
	U8 CharUnits, NextCharUnits;
	U32 CodePoint, NextCodePoint, MaxDigits, nDigitsRead, LeadingZeros;
	const UTF16* pNextChar;

	if ((pResult == 0) || (pSource == 0)) return 2;

	nDigitsRead = 0;
	LeadingZeros = 0;
	MaxDigits = nBytes * 2;

	capi_memset(pResult, 0, nBytes);

	CharUnits = capi_UTF16_GetCharUnits(*pSource);
	CodePoint = capi_UTF16_Decode(CharUnits, pSource);

	while (CodePoint == 0x20)
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

		if ((NextCodePoint == 0x78) || (NextCodePoint == 0x58) || (NextCodePoint == 0xFF58) || (NextCodePoint == 0xFF38))
		{
			pSource += CharUnits + NextCharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*pSource);
			CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		}
	}

	while ((CodePoint == 0x30) || (CodePoint == 0xFF10))
	{
		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
		LeadingZeros++;
	}

	while (CodePoint != 0)
	{
		if (CodePoint == 0x20) break;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30; // 0 - 9
		else if ((CodePoint >= 0x41) && (CodePoint <= 0x46)) CodePoint -= 0x37; // A - F
		else if ((CodePoint >= 0x61) && (CodePoint <= 0x66)) CodePoint -= 0x57; // a - f
		else if ((CodePoint >= 0xFF10) && (CodePoint <= 0xFF19)) CodePoint -= 0xFF10; // 0 - 9
		else if ((CodePoint >= 0xFF21) && (CodePoint <= 0xFF3A)) CodePoint -= 0xFF17; // A - F
		else if ((CodePoint >= 0xFF41) && (CodePoint <= 0xFF5A)) CodePoint -= 0xFF37; // a - f
		else
		{
			if (Flags & SCAN_STRICT) return 1;
			if ((CodePoint == 0x0A) || (CodePoint == 0x0D)) break; // LF or CR
			if ((CodePoint >= 0x21) && (CodePoint <= 0x2F)) break; // ! " # $ % & ' ( ) * + , - . /
			if ((CodePoint >= 0x3A) && (CodePoint <= 0x40)) break; // : ; < = > ? @
			if ((CodePoint >= 0x5B) && (CodePoint <= 0x60)) break; // [ \ ] ^ _ `
			if ((CodePoint >= 0x7B) && (CodePoint <= 0x7E)) break; // { | } ~
		}

		hexadecimal_ShiftLeftBy4(pResult, nBytes);
		*(U8*)pResult |= CodePoint;

		if (nDigitsRead == MaxDigits) return -1;
		nDigitsRead++;

		pSource += CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*pSource);
		CodePoint = capi_UTF16_Decode(CharUnits, pSource);
	}

	nDigitsRead += LeadingZeros;
	if (nDigitsRead == 0) return 1;

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

	if (ppNewPos != 0) *ppNewPos = (UTF16*)pSource;

	return 0;
}

CAPI_FUNC(I8) capi_ScanHexL(void* pResult, const UTF32* pSource, U32 Flags, UTF32** ppNewPos, U32 nBytes)
{
	U32 CodePoint, MaxDigits, nDigitsRead, LeadingZeros;

	if ((pResult == 0) || (pSource == 0)) return 2;

	nDigitsRead = 0;
	LeadingZeros = 0;
	MaxDigits = nBytes * 2;

	capi_memset(pResult, 0, nBytes);

	while (*pSource == 0x20) pSource++;

	if (*pSource == 0x30)
	{
		if ((pSource[1] == 0x78) || (pSource[1] == 0x58)) pSource += 2;
	}

	while (*pSource == 0x30)
	{
		pSource++;
		LeadingZeros++;
	}

	while (*pSource != 0)
	{
		if (*pSource == 0x20) break;

		CodePoint = *pSource;

		if ((CodePoint >= 0x30) && (CodePoint <= 0x39)) CodePoint -= 0x30; // 0 - 9
		else if ((CodePoint >= 0x41) && (CodePoint <= 0x46)) CodePoint -= 0x37; // A - F
		else if ((CodePoint >= 0x61) && (CodePoint <= 0x66)) CodePoint -= 0x57; // a - f
		else
		{
			if (Flags & SCAN_STRICT) return 1;
			if ((CodePoint == 0x0A) || (CodePoint == 0x0D)) break; // LF or CR
			if ((CodePoint >= 0x21) && (CodePoint <= 0x2F)) break; // ! " # $ % & ' ( ) * + , - . /
			if ((CodePoint >= 0x3A) && (CodePoint <= 0x40)) break; // : ; < = > ? @
			if ((CodePoint >= 0x5B) && (CodePoint <= 0x60)) break; // [ \ ] ^ _ `
			if ((CodePoint >= 0x7B) && (CodePoint <= 0x7E)) break; // { | } ~
		}

		hexadecimal_ShiftLeftBy4(pResult, nBytes);
		*(U8*)pResult |= CodePoint;

		if (nDigitsRead == MaxDigits) return -1;
		nDigitsRead++;

		pSource++;
	}

	nDigitsRead += LeadingZeros;
	if (nDigitsRead == 0) return 1;

	if (Flags & SCAN_STRICT)
	{
		while (*pSource == 0x20) pSource++;
		if (*pSource != 0) return 1;
	}

	if (ppNewPos != 0) *ppNewPos = (UTF32*)pSource;

	return 0;
}
