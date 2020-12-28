
/******************************************************************************************
*
* Name: strings.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines the basic and common string functions.
*
*******************************************************************************************/

#include "CAPI.h"

// *                      * //
// **                    ** //
// ***  ASCII versions  *** //
// **                    ** //
// *                      * //

CAPI_FUNC(size_t) capi_StrLenA(const ASCII* String)
{
	size_t Len;

	if (String == 0) return -1;
	Len = 0;

	while (*String != 0)
	{
		Len++;
		String++;
	}

	return Len;
}

CAPI_FUNC(size_t) capi_StrCopyA(ASCII* Destination, size_t Length, const ASCII* Source)
{
	size_t Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	Length--;
	while (*Source != 0)
	{
		if (Len == Length)
		{
			Destination[Len] = 0;
			return Length + 1;
		}
		Destination[Len] = *Source;
		Len++;
		Source++;
	}
	Destination[Len] = 0;

	return Len;
}

CAPI_FUNC(size_t) capi_StrAppendA(ASCII* Destination, size_t Length, const ASCII* Source)
{
	size_t Len, I, cnt;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	Length--;
	while (Destination[Len] != 0)
	{
		if (Len == Length) return Length + 1;
		Len++;
	}
	cnt = Len;

	I = 0;
	while (Source[I] != 0)
	{
		if (Len == Length)
		{
			Destination[Len] = 0;
			return Length + 1;
		}
		Destination[Len] = Source[I];
		Len++;
		I++;
	}
	Destination[Len] = 0;

	return Len - cnt;
}

CAPI_FUNC(I32) capi_StrCompareA(const ASCII* String1, const ASCII* String2)
{
	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		if (*String1 != *String2)
		{
			if (*String1 > *String2) return 1;
			else return -1;
		}
		String1++;
		String2++;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(I32) capi_StrCompareInsensitiveA(const ASCII* String1, const ASCII* String2)
{
	ASCII Code1, Code2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		Code1 = *String1;
		Code2 = *String2;

		if ((Code1 > 0x40) && (Code1 < 0x5B)) Code1 += 0x20;
		if ((Code2 > 0x40) && (Code2 < 0x5B)) Code2 += 0x20;

		if (Code1 != Code2)
		{
			if (Code1 > Code2) return 1;
			else return -1;
		}
		String1++;
		String2++;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(ASCII*) capi_StrFindA(const ASCII* String, ASCII Delimit)
{
	if (String == 0) return 0;

	while (*String != 0)
	{
		if (*String == Delimit) return (ASCII*)String;
		String++;
	}

	return 0;
}

CAPI_FUNC(ASCII*) capi_StrFindStrA(const ASCII* String, const ASCII* StrDelimit)
{
	size_t I, Length;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenA(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		while ((String[I] != 0) && (StrDelimit[I] != 0))
		{
			if (String[I] != StrDelimit[I]) break;
			I++;
		}

		if (I == Length) return (ASCII*)String;

		String++;
	}

	return 0;
}

CAPI_FUNC(ASCII*) capi_StrFindStrInsensitiveA(const ASCII* String, const ASCII* StrDelimit)
{
	size_t I, Length;
	ASCII Code1, Code2;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenA(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		while ((String[I] != 0) && (StrDelimit[I] != 0))
		{
			Code1 = String[I];
			Code2 = StrDelimit[I];

			if ((Code1 > 0x40) && (Code1 < 0x5B)) Code1 += 0x20;
			if ((Code2 > 0x40) && (Code2 < 0x5B)) Code2 += 0x20;

			if (Code1 != Code2) break;
			I++;
		}

		if (I == Length) return (ASCII*)String;

		String++;
	}

	return 0;
}

CAPI_FUNC(ASCII*) capi_StrSplitA(ASCII* String, ASCII Delimit)
{
	if (String == 0) return 0;

	while (*String != 0)
	{
		if (*String == Delimit)
		{
			*String = 0;
			return String + 1;
		}
		String = String + 1;
	}

	return 0;
}

CAPI_FUNC(void) capi_StrReverseA(ASCII* String)
{
	ASCII Code;
	ASCII* First, * Last;

	if (String == 0) return;

	Last = First = String;

	while (*Last != 0) Last++;
	while ((First != Last) && (First != --Last))
	{
		Code = *First;
		*First = *Last;
		*Last = Code;
		First = First + 1;
	}
}

// *                         * //
// **                       ** //
// ***  UTF-8LE  versions  *** //
// **                       ** //
// *                         * //

CAPI_FUNC(U8) capi_UTF8_GetCharUnits(UTF8 Code)
{
	if ((Code & 0x80) == 0) return 1;
	else if ((Code & 0xE0) == 0xC0) return 2;
	else if ((Code & 0xF0) == 0xE0) return 3;
	else if ((Code & 0xF8) == 0xF0) return 4;
	else return 0;
}

CAPI_FUNC(U8) capi_UTF8_Encode_Unsafe(UTF8* String, U32 CodePoint)
{
	if (CodePoint == 0) return 0;

	if (CodePoint < 0x80)
	{
		String[0] = (UTF8)CodePoint & 0x7F;
		return 1;
	}
	else if (CodePoint < 0x800)
	{
		String[0] = (UTF8)(CodePoint >> 6) | 0xC0;
		String[1] = (UTF8)(CodePoint & 0x3F) | 0x80;
		return 2;
	}
	else if (CodePoint < 0x10000)
	{
		String[0] = (UTF8)(CodePoint >> 12) | 0xE0;
		String[1] = (UTF8)((CodePoint >> 6) & 0x3F) | 0x80;
		String[2] = (UTF8)(CodePoint & 0x3F) | 0x80;
		return 3;
	}
	else if (CodePoint < 0x110000)
	{
		String[0] = (UTF8)(CodePoint >> 18) | 0xF0;
		String[1] = (UTF8)((CodePoint >> 12) & 0x3F) | 0x80;
		String[2] = (UTF8)((CodePoint >> 6) & 0x3F) | 0x80;
		String[3] = (UTF8)(CodePoint & 0x3F) | 0x80;
		return 4;
	}

	return 0;
}

CAPI_FUNC(U8) capi_UTF8_Encode(UTF8* String, size_t Length, U32 CodePoint)
{
	if (CodePoint == 0) return 0;

	if (CodePoint < 0x80)
	{
		if (Length < 2) return 0;
		String[0] = (UTF8)CodePoint & 0x7F;
		String[1] = 0;
		return 1;
	}
	else if (CodePoint < 0x800)
	{
		if (Length < 3) return 0;
		String[0] = (UTF8)(CodePoint >> 6) | 0xC0;
		String[1] = (UTF8)(CodePoint & 0x3F) | 0x80;
		String[2] = 0;
		return 2;
	}
	else if (CodePoint < 0x10000)
	{
		if (Length < 4) return 0;
		String[0] = (UTF8)(CodePoint >> 12) | 0xE0;
		String[1] = (UTF8)((CodePoint >> 6) & 0x3F) | 0x80;
		String[2] = (UTF8)(CodePoint & 0x3F) | 0x80;
		String[3] = 0;
		return 3;
	}
	else if (CodePoint < 0x110000)
	{
		if (Length < 5) return 0;
		String[0] = (UTF8)(CodePoint >> 18) | 0xF0;
		String[1] = (UTF8)((CodePoint >> 12) & 0x3F) | 0x80;
		String[2] = (UTF8)((CodePoint >> 6) & 0x3F) | 0x80;
		String[3] = (UTF8)(CodePoint & 0x3F) | 0x80;
		String[4] = 0;
		return 4;
	}

	return 0;
}

CAPI_FUNC(U32) capi_UTF8_Decode(U8 Units, const UTF8* String)
{
	U32 CodePoint;

	switch (Units)
	{
	case 1:
		CodePoint = String[0];
		break;
	case 2:
		CodePoint = (String[0] << 6 | (String[1] & 0x3F)) & 0x7FF;
		break;
	case 3:
		CodePoint = (String[0] << 12 | ((String[1] & 0x3F) << 6) | (String[2] & 0x3F)) & 0xFFFF;
		break;
	case 4:
		CodePoint = (String[0] << 18 | ((String[1] & 0x3F) << 12) | ((String[2] & 0x3F) << 6) | (String[3] & 0x3F)) & 0x1FFFFF;
		break;
	default:
		return 0;
	}

	return CodePoint;
}

CAPI_FUNC(size_t) capi_StrLenU(const UTF8* String)
{
	size_t Len, CharUnits;

	if (String == 0) return -1;
	Len = 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) return Len;
		String += CharUnits;
		Len++;
	}

	return Len;
}

CAPI_FUNC(size_t) capi_StrUnitsU(const UTF8* String)
{
	size_t TotalUnits, CharUnits;

	if (String == 0) return -1;
	TotalUnits = 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) return TotalUnits;
		String += CharUnits;
		TotalUnits += CharUnits;
	}

	return TotalUnits;
}

CAPI_FUNC(size_t) capi_StrCopyU(UTF8* Destination, size_t Length, const UTF8* Source)
{
	U8 CharUnits;
	size_t Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	while (*Source != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*Source);
		if (CharUnits == 0) break;
		Len += CharUnits;
		if (Len >= Length)
		{
			*Destination = 0;
			return Length;
		}
		switch (CharUnits)
		{
		case 1:
			Destination[0] = Source[0];
			break;
		case 2:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			break;
		case 3:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			Destination[2] = Source[2];
			break;
		case 4:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			Destination[2] = Source[2];
			Destination[3] = Source[3];
			break;
		default:
			break;
		}
		Destination += CharUnits;
		Source += CharUnits;
	}
	*Destination = 0;

	return Len;
}

CAPI_FUNC(size_t) capi_StrAppendU(UTF8* Destination, size_t Length, const UTF8* Source)
{
	size_t Len, cnt;
	U32 CharUnits;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	while (*Destination != 0)
	{
		if (Len >= Length) return Length;
		CharUnits = capi_UTF8_GetCharUnits(*Destination);
		if (CharUnits == 0) break;
		Destination += CharUnits;
		Len += CharUnits;
	}
	cnt = Len;

	while (*Source != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*Source);
		if (CharUnits == 0) break;
		Len += CharUnits;
		if (Len >= Length)
		{
			*Destination = 0;
			return Length;
		}
		switch (CharUnits)
		{
		case 1:
			Destination[0] = Source[0];
			break;
		case 2:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			break;
		case 3:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			Destination[2] = Source[2];
			break;
		case 4:
			Destination[0] = Source[0];
			Destination[1] = Source[1];
			Destination[2] = Source[2];
			Destination[3] = Source[3];
			break;
		default:
			break;
		}
		Destination += CharUnits;
		Source += CharUnits;
	}
	*Destination = 0;

	return Len - cnt;
}

CAPI_FUNC(I32) capi_StrCompareU(const UTF8* String1, const UTF8* String2)
{
	U8 CharUnits1, CharUnits2;
	U32 CodePoint1, CodePoint2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		CharUnits1 = capi_UTF8_GetCharUnits(*String1);
		CharUnits2 = capi_UTF8_GetCharUnits(*String2);

		if (CharUnits1 == 0) return 0x7FFFFFFF;
		if (CharUnits2 == 0) return 0x7FFFFFFF;

		CodePoint1 = capi_UTF8_Decode(CharUnits1, String1);
		CodePoint2 = capi_UTF8_Decode(CharUnits2, String2);

		if (CodePoint1 != CodePoint2)
		{
			if (CodePoint1 > CodePoint2) return 1;
			else return -1;
		}
		String1 += CharUnits1;
		String2 += CharUnits2;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(I32) capi_StrCompareInsensitiveU(const UTF8* String1, const UTF8* String2)
{
	U8 CharUnits1, CharUnits2;
	U32 CodePoint1, CodePoint2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		CharUnits1 = capi_UTF8_GetCharUnits(*String1);
		CharUnits2 = capi_UTF8_GetCharUnits(*String2);

		if (CharUnits1 == 0) return 0x7FFFFFFF;
		if (CharUnits2 == 0) return 0x7FFFFFFF;

		CodePoint1 = capi_UTF8_Decode(CharUnits1, String1);
		CodePoint2 = capi_UTF8_Decode(CharUnits2, String2);

		if ((CodePoint1 > 0x40) && (CodePoint1 < 0x5B)) CodePoint1 += 0x20;
		else if ((CodePoint1 > 0xFF20) && (CodePoint1 < 0xFF3B)) CodePoint1 -= 0xFEE0;
		else if ((CodePoint1 > 0xFF40) && (CodePoint1 < 0xFF5B)) CodePoint1 -= 0xFF00;

		if ((CodePoint2 > 0x40) && (CodePoint2 < 0x5B)) CodePoint2 += 0x20;
		else if ((CodePoint2 > 0xFF20) && (CodePoint2 < 0xFF3B)) CodePoint2 -= 0xFEE0;
		else if ((CodePoint2 > 0xFF40) && (CodePoint2 < 0xFF5B)) CodePoint2 -= 0xFF00;

		if (CodePoint1 != CodePoint2)
		{
			if (CodePoint1 > CodePoint2) return 1;
			else return -1;
		}
		String1 += CharUnits1;
		String2 += CharUnits2;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(UTF8*) capi_StrFindU(const UTF8* String, U32 Delimit)
{
	U8 CharUnits;
	U32 CodePoint;

	if (String == 0) return 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) break;
		CodePoint = capi_UTF8_Decode(CharUnits, String);
		if (CodePoint == Delimit) return (ASCII*)String;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF8*) capi_StrFindStrU(const UTF8* String, const UTF8* StrDelimit)
{
	size_t I, Length;
	const UTF8* String2, * StrDelimit2;
	U32 CodePoint1, CodePoint2;
	U8 CharUnits;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenU(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		String2 = String;
		StrDelimit2 = StrDelimit;

		while ((*String2 != 0) && (*StrDelimit2 != 0))
		{
			CharUnits = capi_UTF8_GetCharUnits(*String2);
			CodePoint1 = capi_UTF8_Decode(CharUnits, String2);
			String2 += CharUnits;

			CharUnits = capi_UTF8_GetCharUnits(*StrDelimit2);
			CodePoint2 = capi_UTF8_Decode(CharUnits, StrDelimit2);
			StrDelimit2 += CharUnits;

			if (CodePoint1 != CodePoint2) break;
			I++;
		}

		if (I == Length) return (UTF8*)String;

		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) break;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF8*) capi_StrFindStrInsensitiveU(const UTF8* String, const UTF8* StrDelimit)
{
	size_t I, Length;
	const UTF8* String2, * StrDelimit2;
	U32 CodePoint1, CodePoint2;
	U8 CharUnits;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenU(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		String2 = String;
		StrDelimit2 = StrDelimit;

		while ((*String2 != 0) && (*StrDelimit2 != 0))
		{
			CharUnits = capi_UTF8_GetCharUnits(*String2);
			CodePoint1 = capi_UTF8_Decode(CharUnits, String2);
			String2 += CharUnits;

			CharUnits = capi_UTF8_GetCharUnits(*StrDelimit2);
			CodePoint2 = capi_UTF8_Decode(CharUnits, StrDelimit2);
			StrDelimit2 += CharUnits;

			if ((CodePoint1 > 0x40) && (CodePoint1 < 0x5B)) CodePoint1 += 0x20;
			else if ((CodePoint1 > 0xFF20) && (CodePoint1 < 0xFF3B)) CodePoint1 -= 0xFEE0;
			else if ((CodePoint1 > 0xFF40) && (CodePoint1 < 0xFF5B)) CodePoint1 -= 0xFF00;

			if ((CodePoint2 > 0x40) && (CodePoint2 < 0x5B)) CodePoint2 += 0x20;
			else if ((CodePoint2 > 0xFF20) && (CodePoint2 < 0xFF3B)) CodePoint2 -= 0xFEE0;
			else if ((CodePoint2 > 0xFF40) && (CodePoint2 < 0xFF5B)) CodePoint2 -= 0xFF00;

			if (CodePoint1 != CodePoint2) break;
			I++;
		}

		if (I == Length) return (UTF8*)String;

		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) break;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF8*) capi_StrSplitU(UTF8* String, U32 Delimit)
{
	U8 CharUnits;
	U32 CodePoint;

	if (String == 0) return 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF8_GetCharUnits(*String);
		if (CharUnits == 0) break;
		CodePoint = capi_UTF8_Decode(CharUnits, String);

		if (CodePoint == Delimit)
		{
			*String = 0;
			return String + CharUnits;
		}
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(void) capi_StrReverseU(UTF8* String)
{
	U8 CharUnits;
	U32 CodePoint1, CodePoint2;
	size_t I, U;
	UTF8* ThisChar, * NextChar;

	if (String == 0) return;

	U = capi_StrLenU(String);
	if (U == 0) return;

	for (; U != 1; U--)
	{
		ThisChar = String;

		CharUnits = capi_UTF8_GetCharUnits(*ThisChar);
		if (CharUnits == 0) return;
		CodePoint1 = capi_UTF8_Decode(CharUnits, ThisChar);

		NextChar = ThisChar + CharUnits;
		CharUnits = capi_UTF8_GetCharUnits(*NextChar);

		for (I = U; I != 1; I--)
		{
			CodePoint2 = capi_UTF8_Decode(CharUnits, NextChar);
			ThisChar += capi_UTF8_Encode_Unsafe(ThisChar, CodePoint2);

			NextChar += CharUnits;
			CharUnits = capi_UTF8_GetCharUnits(*NextChar);
			if (CharUnits == 0) return;
		}

		capi_UTF8_Encode_Unsafe(ThisChar, CodePoint1);
	}
}

// *                         * //
// **                       ** //
// ***  UTF-16LE versions  *** //
// **                       ** //
// *                         * //

CAPI_FUNC(U8) capi_UTF16_GetCharUnits(UTF16 Code)
{
	if ((Code > 0xD7FF) && (Code < 0xE000)) return 2;

	return 1;
}

CAPI_FUNC(U8) capi_UTF16_Encode_Unsafe(UTF16* String, U32 CodePoint)
{
	if (CodePoint < 0x10000)
	{
		String[0] = (UTF16)CodePoint;
		return 1;
	}

	CodePoint -= 0x10000;
	String[0] = (UTF16)(CodePoint >> 10) + 0xD800;
	String[1] = (UTF16)(CodePoint & 0x3FF) + 0xDC00;

	return 2;
}

CAPI_FUNC(U8) capi_UTF16_Encode(UTF16* String, size_t Length, U32 CodePoint)
{
	if (CodePoint < 0x10000)
	{
		if (Length < 2) return 0;
		String[0] = (UTF16)CodePoint;
		String[1] = 0;
		return 1;
	}

	if (Length < 3) return 0;
	CodePoint -= 0x10000;
	String[0] = (UTF16)(CodePoint >> 10) + 0xD800;
	String[1] = (UTF16)(CodePoint & 0x3FF) + 0xDC00;
	String[2] = 0;

	return 2;
}

CAPI_FUNC(U32) capi_UTF16_Decode(U8 Units, const UTF16* String)
{
	if (Units == 1) return String[0];
	else if (Units == 2) return ((String[0] - 0xD800) << 10) + (String[1] - 0xDC00) + 0x10000;
	else return 0;
}

CAPI_FUNC(size_t) capi_StrLenW(const UTF16* String)
{
	size_t Len, CharUnits;

	if (String == 0) return 0;
	Len = 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) return Len;
		String += CharUnits;
		Len++;
	}

	return Len;
}

CAPI_FUNC(size_t) capi_StrUnitsW(const UTF16* String)
{
	size_t TotalUnits, CharUnits;

	if (String == 0) return 0;
	TotalUnits = 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) return TotalUnits;
		String += CharUnits;
		TotalUnits += CharUnits;
	}

	return TotalUnits;
}

CAPI_FUNC(size_t) capi_StrCopyW(UTF16* Destination, size_t Length, const UTF16* Source)
{
	U32 CharUnits;
	size_t Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	while (*Source != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*Source);
		if (CharUnits == 0) break;
		Len += CharUnits;
		if (Len >= Length)
		{
			*Destination = 0;
			return Length;
		}
		Destination[0] = Source[0];
		if (CharUnits == 2)
		{
			Destination[1] = Source[1];
		}
		Destination += CharUnits;
		Source += CharUnits;
	}
	*Destination = 0;

	return Len;
}

CAPI_FUNC(size_t) capi_StrAppendW(UTF16* Destination, size_t Length, const UTF16* Source)
{
	size_t Len, cnt;
	U32 CharUnits;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	while (*Destination != 0)
	{
		if (Len >= Length) return Length;
		CharUnits = capi_UTF16_GetCharUnits(*Destination);
		if (CharUnits == 0) break;
		Destination += CharUnits;
		Len += CharUnits;
	}
	cnt = Len;

	while (*Source != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*Source);
		if (CharUnits == 0) break;
		Len += CharUnits;
		if (Len >= Length)
		{
			*Destination = 0;
			return Length;
		}
		Destination[0] = Source[0];
		if (CharUnits == 2)
		{
			Destination[1] = Source[1];
		}
		Destination += CharUnits;
		Source += CharUnits;
	}
	*Destination = 0;

	return Len - cnt;
}

CAPI_FUNC(I32) capi_StrCompareW(const UTF16* String1, const UTF16* String2)
{
	U8 CharUnits1, CharUnits2;
	U32 CodePoint1, CodePoint2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		CharUnits1 = capi_UTF16_GetCharUnits(*String1);
		CharUnits2 = capi_UTF16_GetCharUnits(*String2);

		if (CharUnits1 == 0) return 0x7FFFFFFF;
		if (CharUnits2 == 0) return 0x7FFFFFFF;

		CodePoint1 = capi_UTF16_Decode(CharUnits1, String1);
		CodePoint2 = capi_UTF16_Decode(CharUnits2, String2);

		if (CodePoint1 != CodePoint2)
		{
			if (CodePoint1 > CodePoint2) return 1;
			else return -1;
		}
		String1 += CharUnits1;
		String2 += CharUnits2;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(I32) capi_StrCompareInsensitiveW(const UTF16* String1, const UTF16* String2)
{
	U8 CharUnits1, CharUnits2;
	U32 CodePoint1, CodePoint2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		CharUnits1 = capi_UTF16_GetCharUnits(*String1);
		CharUnits2 = capi_UTF16_GetCharUnits(*String2);

		if (CharUnits1 == 0) return 0x7FFFFFFF;
		if (CharUnits2 == 0) return 0x7FFFFFFF;

		CodePoint1 = capi_UTF16_Decode(CharUnits1, String1);
		CodePoint2 = capi_UTF16_Decode(CharUnits2, String2);

		if ((CodePoint1 > 0x40) && (CodePoint1 < 0x5B)) CodePoint1 += 0x20;
		else if ((CodePoint1 > 0xFF20) && (CodePoint1 < 0xFF3B)) CodePoint1 -= 0xFEE0;
		else if ((CodePoint1 > 0xFF40) && (CodePoint1 < 0xFF5B)) CodePoint1 -= 0xFF00;

		if ((CodePoint2 > 0x40) && (CodePoint2 < 0x5B)) CodePoint2 += 0x20;
		else if ((CodePoint2 > 0xFF20) && (CodePoint2 < 0xFF3B)) CodePoint2 -= 0xFEE0;
		else if ((CodePoint2 > 0xFF40) && (CodePoint2 < 0xFF5B)) CodePoint2 -= 0xFF00;

		if (CodePoint1 != CodePoint2)
		{
			if (CodePoint1 > CodePoint2) return 1;
			else return -1;
		}
		String1 += CharUnits1;
		String2 += CharUnits2;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(UTF16*) capi_StrFindW(const UTF16* String, U32 Delimit)
{
	U8 CharUnits;
	U32 CodePoint;

	if (String == 0) return 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) break;
		CodePoint = capi_UTF16_Decode(CharUnits, String);
		if (CodePoint == Delimit) return (UTF16*)String;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF16*) capi_StrFindStrW(const UTF16* String, const UTF16* StrDelimit)
{
	size_t I, Length;
	const UTF16* String2, * StrDelimit2;
	U32 CodePoint1, CodePoint2;
	U8 CharUnits;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenW(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		String2 = String;
		StrDelimit2 = StrDelimit;

		while ((*String2 != 0) && (*StrDelimit2 != 0))
		{
			CharUnits = capi_UTF16_GetCharUnits(*String2);
			CodePoint1 = capi_UTF16_Decode(CharUnits, String2);
			String2 += CharUnits;

			CharUnits = capi_UTF16_GetCharUnits(*StrDelimit2);
			CodePoint2 = capi_UTF16_Decode(CharUnits, StrDelimit2);
			StrDelimit2 += CharUnits;

			if (CodePoint1 != CodePoint2) break;
			I++;
		}

		if (I == Length) return (UTF16*)String;

		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) break;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF16*) capi_StrFindStrInsensitiveW(const UTF16* String, const UTF16* StrDelimit)
{
	size_t I, Length;
	const UTF16* String2, * StrDelimit2;
	U32 CodePoint1, CodePoint2;
	U8 CharUnits;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenW(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		String2 = String;
		StrDelimit2 = StrDelimit;

		while ((*String2 != 0) && (*StrDelimit2 != 0))
		{
			CharUnits = capi_UTF16_GetCharUnits(*String2);
			CodePoint1 = capi_UTF16_Decode(CharUnits, String2);
			String2 += CharUnits;

			CharUnits = capi_UTF16_GetCharUnits(*StrDelimit2);
			CodePoint2 = capi_UTF16_Decode(CharUnits, StrDelimit2);
			StrDelimit2 += CharUnits;

			if ((CodePoint1 > 0x40) && (CodePoint1 < 0x5B)) CodePoint1 += 0x20;
			else if ((CodePoint1 > 0xFF20) && (CodePoint1 < 0xFF3B)) CodePoint1 -= 0xFEE0;
			else if ((CodePoint1 > 0xFF40) && (CodePoint1 < 0xFF5B)) CodePoint1 -= 0xFF00;

			if ((CodePoint2 > 0x40) && (CodePoint2 < 0x5B)) CodePoint2 += 0x20;
			else if ((CodePoint2 > 0xFF20) && (CodePoint2 < 0xFF3B)) CodePoint2 -= 0xFEE0;
			else if ((CodePoint2 > 0xFF40) && (CodePoint2 < 0xFF5B)) CodePoint2 -= 0xFF00;

			if (CodePoint1 != CodePoint2) break;
			I++;
		}

		if (I == Length) return (UTF16*)String;

		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) break;
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(UTF16*) capi_StrSplitW(UTF16* String, U32 Delimit)
{
	U8 CharUnits;
	U32 CodePoint;

	if (String == 0) return 0;

	while (*String != 0)
	{
		CharUnits = capi_UTF16_GetCharUnits(*String);
		if (CharUnits == 0) break;
		CodePoint = capi_UTF16_Decode(CharUnits, String);

		if (CodePoint == Delimit)
		{
			*String = 0;
			return String + CharUnits;
		}
		String += CharUnits;
	}

	return 0;
}

CAPI_FUNC(void) capi_StrReverseW(UTF16* String)
{
	U8 CharUnits;
	U32 CodePoint1, CodePoint2;
	size_t I, U;
	UTF16* ThisChar, * NextChar;

	if (String == 0) return;

	U = capi_StrLenW(String);
	if (U == 0) return;

	for (; U != 1; U--)
	{
		ThisChar = String;

		CharUnits = capi_UTF16_GetCharUnits(*ThisChar);
		if (CharUnits == 0) return;
		CodePoint1 = capi_UTF16_Decode(CharUnits, ThisChar);

		NextChar = ThisChar + CharUnits;
		CharUnits = capi_UTF16_GetCharUnits(*NextChar);

		for (I = U; I != 1; I--)
		{
			CodePoint2 = capi_UTF16_Decode(CharUnits, NextChar);
			ThisChar += capi_UTF16_Encode_Unsafe(ThisChar, CodePoint2);

			NextChar += CharUnits;
			CharUnits = capi_UTF16_GetCharUnits(*NextChar);
			if (CharUnits == 0) return;
		}
		capi_UTF16_Encode_Unsafe(ThisChar, CodePoint1);
	}
}

// *                         * //
// **                       ** //
// ***  UTF-32LE versions  *** //
// **                       ** //
// *                         * //

CAPI_FUNC(size_t) capi_StrLenL(const UTF32* String)
{
	size_t Len;

	if (String == 0) return 0;
	Len = 0;

	while (*String != 0)
	{
		Len++;
		String++;
	}

	return Len;
}

CAPI_FUNC(size_t) capi_StrCopyL(UTF32* Destination, size_t Length, const UTF32* Source)
{
	size_t Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	Length--;
	while (*Source != 0)
	{
		if (Len == Length)
		{
			Destination[Len] = 0;
			return Length + 1;
		}
		Destination[Len] = *Source;
		Len++;
		Source++;
	}
	Destination[Len] = 0;

	return Len;
}

CAPI_FUNC(size_t) capi_StrAppendL(UTF32* Destination, size_t Length, const UTF32* Source)
{
	size_t Len, I, cnt;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = 0;

	Length--;
	while (Destination[Len] != 0)
	{
		if (Len == Length) return Length + 1;
		Len++;
	}
	cnt = Len;

	I = 0;
	while (Source[I] != 0)
	{
		if (Len == Length)
		{
			Destination[Len] = 0;
			return Length + 1;
		}
		Destination[Len] = Source[I];
		Len++;
		I++;
	}
	Destination[Len] = 0;

	return Len - cnt;
}

CAPI_FUNC(I32) capi_StrCompareL(const UTF32* String1, const UTF32* String2)
{
	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		if (*String1 != *String2)
		{
			if (*String1 > *String2) return 1;
			else return -1;
		}
		String1++;
		String2++;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(I32) capi_StrCompareInsensitiveL(const UTF32* String1, const UTF32* String2)
{
	UTF32 Code1, Code2;

	if ((String1 == 0) || (String2 == 0)) return 0x7FFFFFFF;

	while ((*String1 != 0) && (*String2 != 0))
	{
		Code1 = *String1;
		Code2 = *String2;

		if ((Code1 > 0x40) && (Code1 < 0x5B)) Code1 += 0x20;
		else if ((Code1 > 0xFF20) && (Code1 < 0xFF3B)) Code1 -= 0xFEE0;
		else if ((Code1 > 0xFF40) && (Code1 < 0xFF5B)) Code1 -= 0xFF00;

		if ((Code2 > 0x40) && (Code2 < 0x5B)) Code2 += 0x20;
		else if ((Code2 > 0xFF20) && (Code2 < 0xFF3B)) Code2 -= 0xFEE0;
		else if ((Code2 > 0xFF40) && (Code2 < 0xFF5B)) Code2 -= 0xFF00;

		if (Code1 != Code2)
		{
			if (Code1 > Code2) return 1;
			else return -1;
		}
		String1++;
		String2++;
	}
	if ((*String1 == 0) && (*String2 == 0)) return 0;
	else
	{
		if (*String1 != 0) return 1;
		else return -1;
	}
}

CAPI_FUNC(UTF32*) capi_StrFindL(const UTF32* String, U32 Delimit)
{
	if (String == 0) return 0;

	while (*String != 0)
	{
		if (*String == Delimit) return (UTF32*)String;
		String++;
	}

	return 0;
}

CAPI_FUNC(UTF32*) capi_StrFindStrL(const UTF32* String, const UTF32* StrDelimit)
{
	size_t I, Length;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenL(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		while ((String[I] != 0) && (StrDelimit[I] != 0))
		{
			if (String[I] != StrDelimit[I]) break;
			I++;
		}

		if (I == Length) return (UTF32*)String;

		String++;
	}

	return 0;
}

CAPI_FUNC(UTF32*) capi_StrFindStrInsensitiveL(const UTF32* String, const UTF32* StrDelimit)
{
	size_t I, Length;
	UTF32 Code1, Code2;

	if ((String == 0) || (StrDelimit == 0)) return 0;

	Length = capi_StrLenL(StrDelimit);

	while (*String != 0)
	{
		I = 0;
		while ((String[I] != 0) && (StrDelimit[I] != 0))
		{
			Code1 = String[I];
			Code2 = StrDelimit[I];

			if ((Code1 > 0x40) && (Code1 < 0x5B)) Code1 += 0x20;
			else if ((Code1 > 0xFF20) && (Code1 < 0xFF3B)) Code1 -= 0xFEE0;
			else if ((Code1 > 0xFF40) && (Code1 < 0xFF5B)) Code1 -= 0xFF00;

			if ((Code2 > 0x40) && (Code2 < 0x5B)) Code2 += 0x20;
			else if ((Code2 > 0xFF20) && (Code2 < 0xFF3B)) Code2 -= 0xFEE0;
			else if ((Code2 > 0xFF40) && (Code2 < 0xFF5B)) Code2 -= 0xFF00;

			if (Code1 != Code2) break;
			I++;
		}

		if (I == Length) return (UTF32*)String;

		String++;
	}

	return 0;
}

CAPI_FUNC(UTF32*) capi_StrSplitL(UTF32* String, U32 Delimit)
{
	if (String == 0) return 0;

	while (*String != 0)
	{
		if (*String == Delimit)
		{
			*String = 0;
			return String + 1;
		}
		String = String + 1;
	}

	return 0;
}

CAPI_FUNC(void) capi_StrReverseL(UTF32* String)
{
	UTF32 Code;
	UTF32* First, * Last;

	if (String == 0) return;

	Last = First = String;

	while (*Last != 0) Last++;
	while ((First != Last) && (First != --Last))
	{
		Code = *First;
		*First = *Last;
		*Last = Code;
		First = First + 1;
	}
}

// *                  * //
// **                ** //
// ***  Conversion  *** //
// **                ** //
// *                  * //

CAPI_FUNC(size_t) capi_UTF8_To_UTF16(UTF16* Destination, size_t Length, const UTF8* Source)
{
	size_t CharCnt, Len;
	U8 CharUnits1, CharUnits2;
	U32 CodePoint;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		CharUnits1 = capi_UTF8_GetCharUnits(*Source);
		CodePoint = capi_UTF8_Decode(CharUnits1, Source);
		CharUnits2 = capi_UTF16_Encode(Destination, Len, CodePoint);
		if (CharUnits2 == 0) return Length;
		Len -= CharUnits2;
		Destination += CharUnits2;
		Source += CharUnits1;
		CharCnt++;
	}
	*Destination = 0;

	return CharCnt;
}

CAPI_FUNC(size_t) capi_UTF8_To_UTF32(UTF32* Destination, size_t Length, const UTF8* Source)
{
	U8 CharUnits;
	size_t CharCnt, Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		if (Len == 1)
		{
			*Destination = 0;
			return Length;
		}
		CharUnits = capi_UTF8_GetCharUnits(*Source);
		*Destination = capi_UTF8_Decode(CharUnits, Source);
		Len--;
		Source += CharUnits;
		Destination++;
		CharCnt++;
	}
	*Destination = 0;

	return CharCnt;
}

CAPI_FUNC(size_t) capi_UTF16_To_UTF8(UTF8* Destination, size_t Length, const UTF16* Source)
{
	size_t CharCnt, Len;
	U8 CharUnits1, CharUnits2;
	U32 CodePoint;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		CharUnits1 = capi_UTF16_GetCharUnits(*Source);
		CodePoint = capi_UTF16_Decode(CharUnits1, Source);
		CharUnits2 = capi_UTF8_Encode(Destination, Len, CodePoint);
		if (CharUnits2 == 0) return Length;
		Len -= CharUnits2;
		Destination += CharUnits2;
		Source += CharUnits1;
		CharCnt++;
	}

	return CharCnt;
}

CAPI_FUNC(size_t) capi_UTF16_To_UTF32(UTF32* Destination, size_t Length, const UTF16* Source)
{
	U8 CharUnits;
	size_t CharCnt, Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		if (Len == 1)
		{
			*Destination = 0;
			return Length;
		}
		CharUnits = capi_UTF16_GetCharUnits(*Source);
		*Destination = capi_UTF16_Decode(CharUnits, Source);
		Len--;
		Source += CharUnits;
		Destination++;
		CharCnt++;
	}
	*Destination = 0;

	return CharCnt;
}

CAPI_FUNC(size_t) capi_UTF32_To_UTF8(UTF8* Destination, size_t Length, const UTF32* Source)
{
	U8 CharUnits;
	size_t CharCnt, Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		CharUnits = capi_UTF8_Encode(Destination, Len, *Source);
		if (CharUnits == 0) return Length;
		Len -= CharUnits;
		Destination += CharUnits;
		Source++;
		CharCnt++;
	}

	return CharCnt;
}

CAPI_FUNC(size_t) capi_UTF32_To_UTF16(UTF16* Destination, size_t Length, const UTF32* Source)
{
	U8 CharUnits;
	size_t CharCnt, Len;

	if ((Destination == 0) || (Source == 0) || (Length == 0)) return -1;
	Len = Length;

	CharCnt = 0;
	while (*Source != 0)
	{
		CharUnits = capi_UTF16_Encode(Destination, Len, *Source);
		if (CharUnits == 0) return Length;
		Len -= CharUnits;
		Destination += CharUnits;
		Source++;
		CharCnt++;
	}

	return CharCnt;
}
