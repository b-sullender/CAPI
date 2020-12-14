
/******************************************************************************************
*
* Name: image.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines common image functions.
*
*******************************************************************************************/

#include "image.h"

CAPI_SUBFUNC(PIXEL*) image_find_color_in_palette(PIXEL* pColor, PIXEL* pPalette, U32 Length)
{
	U32 I;

	for (I = 0; I < Length; I++)
	{
		if (*(U32*)pColor == ((U32*)pPalette)[I]) return &pPalette[I];
	}

	return 0;
}

CAPI_SUBFUNC(I32) image_generate_palette_from_image(PIXEL* pPalette, IMAGE* pImage)
{
	U32 Width, Height, ScanLine, I, PaletteLen;
	PIXEL* pSource;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	PaletteLen = 0;

	while (Height-- != 0)
	{
		for (I = 0; I < Width; I++)
		{
			if (!image_find_color_in_palette(&pSource[I], pPalette, PaletteLen))
			{
				if (PaletteLen == 256) return -1;
				pPalette[PaletteLen] = pSource[I];
				PaletteLen++;
			}
		}
		pSource += ScanLine;
	}

	return PaletteLen;
}

CAPI_SUBFUNC(U8) image_get_transparency_level(IMAGE* pImage)
{
	U8 Level;
	U32 Width, Height, ScanLine, I;
	PIXEL* pSource;

	if (pImage == 0) return 0;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	if (pSource == 0) return 0;

	Level = 0;

	while (Height-- != 0)
	{
		for (I = 0; I < Width; I++)
		{
			if ((pSource[I].Alpha > 0) && (pSource[I].Alpha < 255)) return 4;
			if (pSource[I].Alpha == 255) Level |= 1;
			if (pSource[I].Alpha == 0) Level |= 2;
		}
		pSource += ScanLine;
	}

	return Level;
}

CAPI_SUBFUNC(void) image_encode_1bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pSource;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] = (U8)(c << 7);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 6);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 5);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 4);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 3);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 2);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 1);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c & 1);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_encode_2bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pSource;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] = (U8)(c << 6);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 4);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c << 2);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c & 3);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_encode_4bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pSource;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] = (U8)(c << 4);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		c = (U32)(image_find_color_in_palette(&pSource[i], pPalette, 256) - pPalette);
		pDestination[b] |= (U8)(c & 15);

		i++;
		if (i == Width)
		{
			pSource += ScanLine;
			pDestination += Stride;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_decode_1bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pDestination;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pDestination = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = ((pSource[b] >> 7) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 6) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 5) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 4) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 3) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 2) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 1) & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = (pSource[b] & 0x1);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_decode_2bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pDestination;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pDestination = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = ((pSource[b] >> 6) & 0x3);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 4) & 0x3);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = ((pSource[b] >> 2) & 0x3);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = (pSource[b] & 0x3);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_decode_4bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette)
{
	U32 Width, Height, ScanLine, i, b, c;
	PIXEL* pDestination;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pDestination = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		c = ((pSource[b] >> 4) & 0xF);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		c = (pSource[b] & 0xF);
		pDestination[i].Red = pPalette[c].Red;
		pDestination[i].Green = pPalette[c].Green;
		pDestination[i].Blue = pPalette[c].Blue;
		pDestination[i].Alpha = pPalette[c].Alpha;

		i++;
		if (i == Width)
		{
			pSource += Stride;
			pDestination += ScanLine;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_apply_icAND(IMAGE* pImage, U8* p_icAND_mask, size_t mask_Stride)
{
	U32 Width, Height, ScanLine, i, b;
	PIXEL* pDestination;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pDestination = pImage->Pixels;

	while (Height-- != 0)
	{
		i = 0;
		b = 0;

	start_new_byte:

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 7) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 6) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 5) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 4) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 3) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 2) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = (((p_icAND_mask[b] >> 1) & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		pDestination[i].Alpha = ((p_icAND_mask[b] & 0x1) == 0) ? 255 : 0;

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		b++;
		goto start_new_byte;
	}
}

CAPI_SUBFUNC(void) image_test_adjust_alpha_pixels(IMAGE* pImage)
{
	U32 Width, Height, ScanLine, I;
	PIXEL* pSource;

	Width = pImage->Width;
	Height = pImage->Height;

	ScanLine = pImage->ScanLine;
	pSource = pImage->Pixels;

	while (Height-- != 0)
	{
		for (I = 0; I < Width; I++)
		{
			if (pSource[I].Alpha != 0) return;
		}
		pSource += ScanLine;
	}

	Height = pImage->Height;
	pSource = pImage->Pixels;

	while (Height-- != 0)
	{
		for (I = 0; I < Width; I++)
		{
			pSource[I].Alpha = 0xFF;
		}
		pSource += ScanLine;
	}
}

CAPI_FUNC(U32) capi_GetImageDimensionsFromMemory(void* pFile, size_t FileSize, U32* pHeight)
{
	if (pFile == 0) return 0;

	if (((BMP*)pFile)->Type == 0x4D42)
	{
		return capi_Get_BMP_DimensionsFromMemory((BMP*)pFile, FileSize, pHeight);
	}
	else if (((JPG*)pFile)->Identifier == 0xD8FF)
	{
		return capi_Get_JPG_DimensionsFromMemory((JPG*)pFile, FileSize, pHeight);
	}
	else if (((PNG*)pFile)->Signature == 0xA1A0A0D474E5089)
	{
		return capi_Get_PNG_DimensionsFromMemory((PNG*)pFile, FileSize, pHeight);
	}
	else if ((((ICO*)pFile)->Reserved == 0) && (((ICO*)pFile)->Type == 1))
	{
		return capi_Get_ICO_DimensionsFromMemory((ICO*)pFile, FileSize, pHeight, -1);
	}
	else return 0;
}

CAPI_FUNC(U32) capi_GetImageDimensionsFromFile(FILE* pFile, U32* pHeight)
{
	U8 ThisImage[16];

	if (pFile == 0) return 0;

	if (fseek(pFile, 0, SEEK_SET) != 0) return 0;
	if (fread(&ThisImage, 1, 8, pFile) != 8) return 0;

	if (((BMP*)&ThisImage)->Type == 0x4D42)
	{
		return capi_Get_BMP_DimensionsFromFile(pFile, pHeight);
	}
	else if (((JPG*)&ThisImage)->Identifier == 0xD8FF)
	{
		return capi_Get_JPG_DimensionsFromFile(pFile, pHeight);
	}
	else if (((PNG*)&ThisImage)->Signature == 0xA1A0A0D474E5089)
	{
		return capi_Get_PNG_DimensionsFromFile(pFile, pHeight);
	}
	else if ((((ICO*)&ThisImage)->Reserved == 0) && (((ICO*)&ThisImage)->Type == 1))
	{
		return capi_Get_ICO_DimensionsFromFile(pFile, pHeight, -1);
	}
	else return 0;
}

CAPI_FUNC(I32) capi_LoadImageFromMemory(IMAGE* pImage, U32 Alignment, void* pFile, U64 FileSize)
{
	if (pFile == 0) return CAPI_ERROR_INVALID_PARAMETER;

	if (((BMP*)pFile)->Type == 0x4D42)
	{
		return capi_Load_BMP_FromMemory(pImage, Alignment, (BMP*)pFile, (size_t)FileSize);
	}
	else if (((JPG*)pFile)->Identifier == 0xD8FF)
	{
		return capi_Load_JPG_FromMemory(pImage, Alignment, (JPG*)pFile, (size_t)FileSize);
	}
	else if (((PNG*)pFile)->Signature == 0xA1A0A0D474E5089)
	{
		return capi_Load_PNG_FromMemory(pImage, Alignment, (PNG*)pFile, (size_t)FileSize);
	}
	else if ((((ICO*)pFile)->Reserved == 0) && (((ICO*)pFile)->Type == 1))
	{
		return capi_Load_ICO_FromMemory(pImage, Alignment, (ICO*)pFile, (size_t)FileSize, -1);
	}
	else return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
}

CAPI_FUNC(PIXEL*) capi_CreateImage(IMAGE* pImage, U32 Width, U32 Height, U32 Color, U32 Alignment)
{
	PIXEL* Pixels;
	U32 ScanLine;

	if (pImage == 0) return 0;
	if (Alignment == 0) return 0;

	ScanLine = Width;
	AlignUp(ScanLine, Alignment);

	Pixels = (PIXEL*)capi_aligned_malloc((ScanLine * sizeof(PIXEL)) * Height, sizeof(PIXEL) * Alignment);

	if (Pixels != 0)
	{
		capi_memset32(Pixels, Color, ScanLine * Height);

		pImage->Pixels = Pixels;
		pImage->ScanLine = ScanLine;
		pImage->Width = Width;
		pImage->Height = Height;
	}

	return Pixels;
}

CAPI_FUNC(void) capi_FreeImage(IMAGE* pImage)
{
	if (pImage != 0)
	{
		capi_aligned_free(pImage->Pixels);

		pImage->Height = 0;
		pImage->Width = 0;
		pImage->ScanLine = 0;
		pImage->Pixels = 0;
	}
}

CAPI_FUNC(I32) capi_FillImage(IMAGE* pImage, U32 Color)
{
	void* Pixels;

	if (pImage == 0) return CAPI_ERROR_INVALID_PARAMETER;

	Pixels = pImage->Pixels;
	if (Pixels == 0) return CAPI_ERROR_INVALID_PARAMETER;

	capi_memset32(Pixels, Color, pImage->ScanLine * pImage->Height);

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(void) capi_RenderAlphaPixel(PIXEL* pDestination, U32 Color, U8 Alpha)
{
	I32 Alpha2, Remainder, Red, Green, Blue, Alpha3;

	Alpha2 = (Color >> (8 * offsetof(PIXEL, Alpha)));
	Alpha2 -= 255;
	Alpha2 += Alpha;
	if (Alpha2 < 0) Alpha2 = 0;
	Remainder = 255 - Alpha2;

	Red = (U8)(Color >> (8 * offsetof(PIXEL, Red)));
	Green = (U8)(Color >> (8 * offsetof(PIXEL, Green)));
	Blue = (U8)(Color >> (8 * offsetof(PIXEL, Blue)));
	Alpha3 = (U8)(Color >> (8 * offsetof(PIXEL, Alpha)));

	pDestination->Red = (U8)(((pDestination->Red * Remainder) + (Red * Alpha2)) / 255);
	pDestination->Green = (U8)(((pDestination->Green * Remainder) + (Green * Alpha2)) / 255);
	pDestination->Blue = (U8)(((pDestination->Blue * Remainder) + (Blue * Alpha2)) / 255);
	pDestination->Alpha = (U8)(((pDestination->Alpha * Remainder) + (Alpha3 * Alpha2)) / 255);
}

CAPI_FUNC(void) capi_RenderAlphaScanLine(PIXEL* pDestination, PIXEL* pSource, U32 Width, U8 Alpha)
{
	I32 Alpha2, Remainder;

	while (Width-- != 0)
	{
		Alpha2 = pSource->Alpha;
		Alpha2 -= 255;
		Alpha2 += Alpha;
		if (Alpha2 < 0) Alpha2 = 0;
		Remainder = 255 - Alpha2;

		pDestination->Red = (U8)(((pDestination->Red * Remainder) + (pSource->Red * Alpha2)) / 255);
		pDestination->Green = (U8)(((pDestination->Green * Remainder) + (pSource->Green * Alpha2)) / 255);
		pDestination->Blue = (U8)(((pDestination->Blue * Remainder) + (pSource->Blue * Alpha2)) / 255);
		pDestination->Alpha = (U8)(((pDestination->Alpha * Remainder) + (pSource->Alpha * Alpha2)) / 255);
		pDestination++;
		pSource++;
	}
}

CAPI_FUNC(void) capi_RenderAlphaScanLine32(PIXEL* pDestination, U32 Color, U32 Width, U8 Alpha)
{
	I32 Alpha2, Remainder, Red, Green, Blue, Alpha3;

	Alpha2 = (Color >> (8 * offsetof(PIXEL, Alpha)));
	Alpha2 -= 255;
	Alpha2 += Alpha;
	if (Alpha2 < 0) Alpha2 = 0;
	Remainder = 255 - Alpha2;

	Red = (U8)(Color >> (8 * offsetof(PIXEL, Red)));
	Green = (U8)(Color >> (8 * offsetof(PIXEL, Green)));
	Blue = (U8)(Color >> (8 * offsetof(PIXEL, Blue)));
	Alpha3 = (U8)(Color >> (8 * offsetof(PIXEL, Alpha)));

	while (Width-- != 0)
	{
		pDestination->Red = (U8)(((pDestination->Red * Remainder) + (Red * Alpha2)) / 255);
		pDestination->Green = (U8)(((pDestination->Green * Remainder) + (Green * Alpha2)) / 255);
		pDestination->Blue = (U8)(((pDestination->Blue * Remainder) + (Blue * Alpha2)) / 255);
		pDestination->Alpha = (U8)(((pDestination->Alpha * Remainder) + (Alpha3 * Alpha2)) / 255);
		pDestination++;
	}
}

CAPI_FUNC(I32) capi_DrawLine(IMAGE* pDestination, CRECT* pRect, U32 Color, U32 Thickness)
{
	CRECT AdjustedRect;
	I32 Width, Height, X, Y, DesScanLine, xSpan, ySpan, xStep, yStep, OverFlow, x2, y2;
	PIXEL* pDesPixels;

	if ((pDestination == 0) || (pRect == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	pDesPixels = pDestination->Pixels;
	if (pDesPixels == 0) return CAPI_ERROR_INVALID_PARAMETER;

	Width = pDestination->Width;
	Height = pDestination->Height;

	AdjustedRect.left = pRect->left;
	AdjustedRect.top = pRect->top;
	AdjustedRect.right = pRect->right;
	AdjustedRect.bottom = pRect->bottom;

	if (AdjustedRect.left > AdjustedRect.right)
	{
		xStep = -1;
		xSpan = AdjustedRect.left - AdjustedRect.right;
		x2 = AdjustedRect.right - 1;
	}
	else
	{
		xStep = 1;
		xSpan = AdjustedRect.right - AdjustedRect.left;
		x2 = AdjustedRect.right + 1;
	}

	if (AdjustedRect.top > AdjustedRect.bottom)
	{
		yStep = -1;
		ySpan = AdjustedRect.top - AdjustedRect.bottom;
		y2 = AdjustedRect.bottom - 1;
	}
	else
	{
		yStep = 1;
		ySpan = AdjustedRect.bottom - AdjustedRect.top;
		y2 = AdjustedRect.bottom + 1;
	}

	DesScanLine = pDestination->ScanLine;
	pDesPixels = &pDesPixels[AdjustedRect.left + (AdjustedRect.top * DesScanLine)];

	DesScanLine = (yStep * DesScanLine);

	if (ySpan < xSpan)
	{
		OverFlow = xSpan / 2;

		for (X = AdjustedRect.left, Y = AdjustedRect.top; X != x2; X += xStep)
		{
			if ((X >= 0) && (Y >= 0) && (X < Width) && (Y < Height))
			{
				*(U32*)pDesPixels = Color;
			}

			if ((OverFlow += ySpan) >= xSpan)
			{
				OverFlow -= xSpan;
				Y += yStep;
				pDesPixels += DesScanLine;
			}

			pDesPixels += xStep;
		}
	}
	else
	{
		OverFlow = ySpan / 2;

		for (X = AdjustedRect.left, Y = AdjustedRect.top; Y != y2; Y += yStep)
		{
			if ((X >= 0) && (Y >= 0) && (X < Width) && (Y < Height))
			{
				*(U32*)pDesPixels = Color;
			}

			if ((OverFlow += xSpan) >= ySpan)
			{
				OverFlow -= ySpan;
				X += xStep;
				pDesPixels += xStep;
			}

			pDesPixels += DesScanLine;
		}
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_DrawRect(IMAGE* pDestination, CRECT* pRect, U32 LineColor, U32 Thickness, U32 FillColor)
{
	CRECT AdjustedRect;
	I32 Width, Height, xLimit, yLimit, DesScanLine, X, Y, Z, HorTop, VerLeft, VerRight, FullThickness;
	PIXEL* pDesPixels;

	if ((pDestination == 0) || (pRect == 0) || (Thickness == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	pDesPixels = (PIXEL*)pDestination->Pixels;
	if (pDesPixels == 0) return CAPI_ERROR_INVALID_PARAMETER;

	// Starting points?

	HorTop = VerLeft = VerRight = Thickness;

	if (pRect->left > pRect->right)
	{
		AdjustedRect.left = pRect->right;
		AdjustedRect.right = pRect->left;
	}
	else
	{
		AdjustedRect.left = pRect->left;
		AdjustedRect.right = pRect->right;
	}

	if (pRect->top > pRect->bottom)
	{
		AdjustedRect.top = pRect->bottom;
		AdjustedRect.bottom = pRect->top;
	}
	else
	{
		AdjustedRect.top = pRect->top;
		AdjustedRect.bottom = pRect->bottom;
	}

	X = -(I32)Thickness;

	if (AdjustedRect.left >= (I32)pDestination->Width) return CAPI_ERROR_NONE;
	else if (AdjustedRect.left < 0)
	{
		VerLeft -= -AdjustedRect.left;
		AdjustedRect.left = 0;
	}

	if (AdjustedRect.top >= (I32)pDestination->Height) return CAPI_ERROR_NONE;
	else if (AdjustedRect.top < 0)
	{
		HorTop -= -AdjustedRect.top;
		AdjustedRect.top = 0;
	}

	// Beyond image limits?

	if (AdjustedRect.right < 0) return CAPI_ERROR_NONE;
	if (AdjustedRect.bottom < 0) return CAPI_ERROR_NONE;

	xLimit = pDestination->Width - AdjustedRect.left;
	yLimit = pDestination->Height - AdjustedRect.top;

	Width = (AdjustedRect.right - AdjustedRect.left) + 1;
	Height = (AdjustedRect.bottom - AdjustedRect.top) + 1;

	if (Height < HorTop) HorTop = Height;

	if (xLimit < Width)
	{
		VerRight -= (Width - xLimit);
		Width = xLimit;
	}

	if (yLimit < Height)
	{
		if ((I32)Thickness <= (Height - yLimit)) X += Thickness;
		else X += (Height - yLimit);
		Height = yLimit;
	}

	// Setup variables and pointer!

	Y = 0;
	X += Height;

	FullThickness = Thickness = 0;
	if (VerLeft > 0)
	{
		Thickness = VerLeft;
		FullThickness = VerLeft;
	}
	if (VerRight > 0) FullThickness += VerRight;

	VerRight++;

	DesScanLine = pDestination->ScanLine;
	pDesPixels = &pDesPixels[AdjustedRect.left + (AdjustedRect.top * DesScanLine)];

	// Fast draw!

	if (((LineColor >> (8 * offsetof(PIXEL, Alpha))) == 0xFF) && ((FillColor >> (8 * offsetof(PIXEL, Alpha))) == 0xFF))
	{
		for (; Y < HorTop; Y++)
		{
			capi_memset32(pDesPixels, LineColor, Width);
			pDesPixels += DesScanLine;
		}

		if (Width > FullThickness)
		{
			for (; Y < X; Y++)
			{
				for (Z = 0; Z < VerLeft; Z++) ((U32*)pDesPixels)[Z] = LineColor;
				capi_memset32(pDesPixels + Thickness, FillColor, Width - FullThickness);
				for (Z = 1; Z < VerRight; Z++) ((U32*)pDesPixels)[Width - Z] = LineColor;
				pDesPixels += DesScanLine;
			}
		}

		for (; Y < Height; Y++)
		{
			capi_memset32(pDesPixels, LineColor, Width);
			pDesPixels += DesScanLine;
		}
	}
	else
	{
		for (; Y < HorTop; Y++)
		{
			capi_RenderAlphaScanLine32(pDesPixels, LineColor, Width, 255);
			pDesPixels += DesScanLine;
		}

		if (Width > FullThickness)
		{
			for (; Y < X; Y++)
			{
				for (Z = 0; Z < VerLeft; Z++) capi_RenderAlphaPixel(&pDesPixels[Z], LineColor, 255);
				capi_RenderAlphaScanLine32(pDesPixels + Thickness, FillColor, Width - FullThickness, 255);
				for (Z = 1; Z < VerRight; Z++) capi_RenderAlphaPixel(&pDesPixels[Width - Z], LineColor, 255);
				pDesPixels += DesScanLine;
			}
		}

		for (; Y < Height; Y++)
		{
			capi_RenderAlphaScanLine32(pDesPixels, LineColor, Width, 255);
			pDesPixels += DesScanLine;
		}
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_DrawImage(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y)
{
	I32 Width, Height, SrcX, SrcY, xLimit, yLimit, CopyLength, DesScanLine, SrcScanLine;
	PIXEL* pDesPixels, * pSrcPixels;

	if ((pDestination == 0) || (pSource == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((pDestination->Pixels == 0) || (pSource->Pixels == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	Width = pSource->Width;
	Height = pSource->Height;

	// Starting points?

	SrcX = SrcY = 0;

	if (X >= (I32)pDestination->Width) return CAPI_ERROR_NONE;
	else if (X < 0)
	{
		SrcX = -X;
		X = 0;
		Width -= SrcX;
	}

	if (Y >= (I32)pDestination->Height) return CAPI_ERROR_NONE;
	else if (Y < 0)
	{
		SrcY = -Y;
		Y = 0;
		Height -= SrcY;
	}

	// Beyond image limits?

	if (SrcX >= Width) return CAPI_ERROR_NONE;
	if (SrcY >= Height) return CAPI_ERROR_NONE;

	xLimit = pDestination->Width - X;
	yLimit = pDestination->Height - Y;

	if (xLimit < Width) Width = xLimit;
	if (yLimit < Height) Height = yLimit;

	// Setup variables and pointers!

	CopyLength = Width * sizeof(PIXEL_RGBA);

	DesScanLine = pDestination->ScanLine;
	SrcScanLine = pSource->ScanLine;

	pDesPixels = &pDestination->Pixels[X + (Y * DesScanLine)];
	pSrcPixels = &pSource->Pixels[SrcX + (SrcY * SrcScanLine)];

	// Fast draw!

	for (Y = 0; Y < Height; Y++)
	{
		capi_memcopy(pDesPixels, pSrcPixels, CopyLength);
		pDesPixels += DesScanLine;
		pSrcPixels += SrcScanLine;
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_DrawImageA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Alpha)
{
	I32 Width, Height, SrcX, SrcY, xLimit, yLimit, DesScanLine, SrcScanLine;
	PIXEL* pDesPixels, * pSrcPixels;

	if ((pDestination == 0) || (pSource == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((pDestination->Pixels == 0) || (pSource->Pixels == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	Width = pSource->Width;
	Height = pSource->Height;

	// Starting points?

	SrcX = SrcY = 0;

	if (X >= (I32)pDestination->Width) return CAPI_ERROR_NONE;
	else if (X < 0)
	{
		SrcX = -X;
		X = 0;
		Width -= SrcX;
	}

	if (Y >= (I32)pDestination->Height) return CAPI_ERROR_NONE;
	else if (Y < 0)
	{
		SrcY = -Y;
		Y = 0;
		Height -= SrcY;
	}

	// Beyond image limits?

	if (SrcX >= Width) return CAPI_ERROR_NONE;
	if (SrcY >= Height) return CAPI_ERROR_NONE;

	xLimit = pDestination->Width - X;
	yLimit = pDestination->Height - Y;

	if (xLimit < Width) Width = xLimit;
	if (yLimit < Height) Height = yLimit;

	// Setup variables and pointers!

	DesScanLine = pDestination->ScanLine;
	SrcScanLine = pSource->ScanLine;

	pDesPixels = &pDestination->Pixels[X + (Y * DesScanLine)];
	pSrcPixels = &pSource->Pixels[SrcX + (SrcY * SrcScanLine)];

	// Fast draw!

	for (Y = 0; Y < Height; Y++)
	{
		capi_RenderAlphaScanLine(pDesPixels, pSrcPixels, Width, Alpha);
		pDesPixels += DesScanLine;
		pSrcPixels += SrcScanLine;
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_DrawImageEx(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height)
{
	U32 bUpsampleX, bUpsampleY, fh, fw, y1a, y1b, y1c, y1d, x1a, x1b, x1c, x1d, y2, Weight_Y, x2, W, A, Red, Green, Blue, Alpha;
	I32 SrcWidth, SrcHeight, SrcX, SrcY, xLimit, yLimit, DesScanLine, SrcScanLine, yFractPart, xFractPart, yE, xES, xE, yIntPart, xIntPart;
	PIXEL* pDesPixels, * pSrcPixels, * pDesThisPixel, * pSrcThisPixel;

	if ((pDestination == 0) || (pSource == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((pDestination->Pixels == 0) || (pSource->Pixels == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	if ((Width == 0) || (Height == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	SrcWidth = Width;
	SrcHeight = Height;

	// Starting points?

	SrcX = SrcY = 0;

	if (X >= (I32)pDestination->Width) return CAPI_ERROR_NONE;
	else if (X < 0)
	{
		SrcX = -X;
		X = 0;
		SrcWidth -= SrcX;
	}

	if (Y >= (I32)pDestination->Height) return CAPI_ERROR_NONE;
	else if (Y < 0)
	{
		SrcY = -Y;
		Y = 0;
		SrcHeight -= SrcY;
	}

	// Beyond image limits?

	if (SrcX >= (I32)pSource->Width) return CAPI_ERROR_NONE;
	if (SrcY >= (I32)pSource->Height) return CAPI_ERROR_NONE;

	xLimit = pDestination->Width - X;
	yLimit = pDestination->Height - Y;

	if (xLimit < SrcWidth) SrcWidth = xLimit;
	if (yLimit < SrcHeight) SrcHeight = yLimit;

	// Setup variables and pointers!

	DesScanLine = pDestination->ScanLine;
	SrcScanLine = pSource->ScanLine;

	pDesPixels = &pDestination->Pixels[X + (Y * DesScanLine)];

	// Fast draw!

	switch (Algorithm)
	{
	case DRAW_RESIZE_LINEAR:
	{
		yIntPart = (pSource->Height / Height) * pSource->Width;
		xIntPart = pSource->Width / Width;
		yFractPart = pSource->Height % Height;
		xFractPart = pSource->Width % Width;

		pSrcPixels = pSource->Pixels;
		pSrcPixels += (SrcY * yIntPart) + (SrcX * xIntPart);

		if (yFractPart == 0) yE = 0;
		else
		{
			for (yE = 0; SrcY != 0; SrcY--)
			{
				yE += yFractPart;
				if (yE >= (I32)Height)
				{
					yE -= Height;
					pSrcPixels += SrcScanLine;
				}
			}
		}

		if (xFractPart == 0) xES = 0;
		else
		{
			for (xES = 0; SrcX != 0; SrcX--)
			{
				xES += xFractPart;
				if (xES >= (I32)Width)
				{
					xES -= Width;
					pSrcPixels++;
				}
			}
		}

		for (Y = 0; Y < SrcHeight; Y++)
		{
			pSrcThisPixel = pSrcPixels;

			for (X = 0, xE = xES; X < SrcWidth; X++)
			{
				pDesPixels[X] = *pSrcThisPixel;

				pSrcThisPixel += xIntPart;
				xE += xFractPart;
				if (xE >= (I32)Width)
				{
					xE -= Width;
					pSrcThisPixel++;
				}
			}

			pSrcPixels += yIntPart;
			yE += yFractPart;
			if (yE >= (I32)Height)
			{
				yE -= Height;
				pSrcPixels += SrcScanLine;
			}

			pDesPixels += DesScanLine;
		}

		break;
	}
	case DRAW_RESIZE_BOXSAMP:
	{
		bUpsampleX = (pSource->Width < Width);
		bUpsampleY = (pSource->Height < Height);

		fh = 256 * pSource->Height / Height;
		fw = 256 * pSource->Width / Width;

		SrcHeight += SrcY;
		SrcWidth += SrcX;

		// For every scan line:
		for (Y = SrcY; Y < SrcHeight; Y++)
		{
			// Find the y-range of input pixels that will contribute:
			y1a = (Y * fh);
			y1b = ((Y + 1) * fh);

			// Map to same pixel -> we want to interpolate between two pixels!
			if (bUpsampleY) y1b = y1a + 256;
			y1b = capi_min(y1b, 256 * pSource->Height - 1);
			y1c = y1a >> 8;
			y1d = y1b >> 8;
			y1a &= 0xFF;
			y1b &= 0xFF;

			pDesThisPixel = pDesPixels;

			// For every output pixel:
			for (X = SrcX; X < SrcWidth; X++)
			{
				// Find the x-range of input pixels that will contribute:
				x1a = (X * fw);
				x1b = ((X + 1) * fw);

				// Map to same pixel -> we want to interpolate between two pixels!
				if (bUpsampleX) x1b = x1a + 256;
				x1b = capi_min(x1b, 256 * pSource->Width - 1);
				x1c = x1a >> 8;
				x1d = x1b >> 8;
				x1a &= 0xFF;
				x1b &= 0xFF;

				Red = Green = Blue = Alpha = A = 0;

				if (y1c != y1d) Weight_Y = 256 - y1a; // if (y2 == y1c)
				else Weight_Y = 256;

				pSrcPixels = &pSource->Pixels[(y1c * SrcScanLine) + x1c];

				// Add up all input pixels contributing to this output pixel:
				for (y2 = y1c; y2 <= y1d; y2++)
				{
					if (y2 == y1d)
					{
						if (y1c != y1d) Weight_Y = y1b;
					}

					pSrcThisPixel = pSrcPixels;
					x2 = x1c;

					if (x2 < x1d)
					{
						W = (256 - x1a) * Weight_Y;

						A += W;
						Red += pSrcThisPixel->Red * W;
						Green += pSrcThisPixel->Green * W;
						Blue += pSrcThisPixel->Blue * W;
						Alpha += pSrcThisPixel->Alpha * W;
						pSrcThisPixel++;
						x2++;
					}

					if (x2 < x1d)
					{
						W = 256 * Weight_Y;

						do
						{
							A += W;
							Red += pSrcThisPixel->Red * W;
							Green += pSrcThisPixel->Green * W;
							Blue += pSrcThisPixel->Blue * W;
							Alpha += pSrcThisPixel->Alpha * W;
							pSrcThisPixel++;
							x2++;
						} while (x2 < x1d);
					}

					if (x2 == x1d)
					{
						if (x1c != x1d) W = x1b * Weight_Y;
						else W = 256 * Weight_Y;

						A += W;
						Red += pSrcThisPixel->Red * W;
						Green += pSrcThisPixel->Green * W;
						Blue += pSrcThisPixel->Blue * W;
						Alpha += pSrcThisPixel->Alpha * W;
						pSrcThisPixel++;
					}

					Weight_Y = 256;
					pSrcPixels += SrcScanLine;
				}

				*(U32*)pDesThisPixel =
					((Red / A) << (8 * offsetof(PIXEL, Red))) |
					((Green / A) << (8 * offsetof(PIXEL, Green))) |
					((Blue / A) << (8 * offsetof(PIXEL, Blue))) |
					((Alpha / A) << (8 * offsetof(PIXEL, Alpha)));

				pDesThisPixel++;
			}

			pDesPixels += DesScanLine;
		}
		break;
	}
	default:
		return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_DrawImageExA(IMAGE* pDestination, IMAGE* pSource, I32 X, I32 Y, U8 Algorithm, U32 Width, U32 Height, U8 Alpha)
{
	U32 bUpsampleX, bUpsampleY, fh, fw, y1a, y1b, y1c, y1d, x1a, x1b, x1c, x1d, y2, Weight_Y, x2, W, A, Red, Green, Blue, Alpha2;
	I32 SrcWidth, SrcHeight, SrcX, SrcY, xLimit, yLimit, DesScanLine, SrcScanLine, yFractPart, xFractPart, yE, xES, xE, yIntPart, xIntPart;
	PIXEL* pDesPixels, * pSrcPixels, * pDesThisPixel, * pSrcThisPixel;

	if ((pDestination == 0) || (pSource == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((pDestination->Pixels == 0) || (pSource->Pixels == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	if ((Width == 0) || (Height == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	SrcWidth = Width;
	SrcHeight = Height;

	// Starting points?

	SrcX = SrcY = 0;

	if (X >= (I32)pDestination->Width) return CAPI_ERROR_NONE;
	else if (X < 0)
	{
		SrcX = -X;
		X = 0;
		SrcWidth -= SrcX;
	}

	if (Y >= (I32)pDestination->Height) return CAPI_ERROR_NONE;
	else if (Y < 0)
	{
		SrcY = -Y;
		Y = 0;
		SrcHeight -= SrcY;
	}

	// Beyond image limits?

	if (SrcX >= (I32)pSource->Width) return CAPI_ERROR_NONE;
	if (SrcY >= (I32)pSource->Height) return CAPI_ERROR_NONE;

	xLimit = pDestination->Width - X;
	yLimit = pDestination->Height - Y;

	if (xLimit < SrcWidth) SrcWidth = xLimit;
	if (yLimit < SrcHeight) SrcHeight = yLimit;

	// Setup variables and pointers!

	DesScanLine = pDestination->ScanLine;
	SrcScanLine = pSource->ScanLine;

	pDesPixels = &pDestination->Pixels[X + (Y * DesScanLine)];

	// Fast draw!

	switch (Algorithm)
	{
	case DRAW_RESIZE_LINEAR:
	{
		yIntPart = (pSource->Height / Height) * pSource->Width;
		xIntPart = pSource->Width / Width;
		yFractPart = pSource->Height % Height;
		xFractPart = pSource->Width % Width;

		pSrcPixels = pSource->Pixels;
		pSrcPixels += (SrcY * yIntPart) + (SrcX * xIntPart);

		if (yFractPart == 0) yE = 0;
		else
		{
			for (yE = 0; SrcY != 0; SrcY--)
			{
				yE += yFractPart;
				if (yE >= (I32)Height)
				{
					yE -= Height;
					pSrcPixels += SrcScanLine;
				}
			}
		}

		if (xFractPart == 0) xES = 0;
		else
		{
			for (xES = 0; SrcX != 0; SrcX--)
			{
				xES += xFractPart;
				if (xES >= (I32)Width)
				{
					xES -= Width;
					pSrcPixels++;
				}
			}
		}

		for (Y = 0; Y < SrcHeight; Y++)
		{
			pSrcThisPixel = pSrcPixels;

			for (X = 0, xE = xES; X < SrcWidth; X++)
			{
				capi_RenderAlphaPixel(&pDesPixels[X], *(U32*)pSrcThisPixel, Alpha);

				pSrcThisPixel += xIntPart;
				xE += xFractPart;
				if (xE >= (I32)Width)
				{
					xE -= Width;
					pSrcThisPixel++;
				}
			}

			pSrcPixels += yIntPart;
			yE += yFractPart;
			if (yE >= (I32)Height)
			{
				yE -= Height;
				pSrcPixels += SrcScanLine;
			}

			pDesPixels += DesScanLine;
		}

		break;
	}
	case DRAW_RESIZE_BOXSAMP:
	{
		bUpsampleX = (pSource->Width < Width);
		bUpsampleY = (pSource->Height < Height);

		fh = 256 * pSource->Height / Height;
		fw = 256 * pSource->Width / Width;

		SrcHeight += SrcY;
		SrcWidth += SrcX;

		// For every scan line:
		for (Y = SrcY; Y < SrcHeight; Y++)
		{
			// Find the y-range of input pixels that will contribute:
			y1a = (Y * fh);
			y1b = ((Y + 1) * fh);

			// Map to same pixel -> we want to interpolate between two pixels!
			if (bUpsampleY) y1b = y1a + 256;
			y1b = capi_min(y1b, 256 * pSource->Height - 1);
			y1c = y1a >> 8;
			y1d = y1b >> 8;
			y1a &= 0xFF;
			y1b &= 0xFF;

			pDesThisPixel = pDesPixels;

			// For every output pixel:
			for (X = SrcX; X < SrcWidth; X++)
			{
				// Find the x-range of input pixels that will contribute:
				x1a = (X * fw);
				x1b = ((X + 1) * fw);

				// Map to same pixel -> we want to interpolate between two pixels!
				if (bUpsampleX) x1b = x1a + 256;
				x1b = capi_min(x1b, 256 * pSource->Width - 1);
				x1c = x1a >> 8;
				x1d = x1b >> 8;
				x1a &= 0xFF;
				x1b &= 0xFF;

				Red = Green = Blue = Alpha2 = A = 0;

				if (y1c != y1d) Weight_Y = 256 - y1a; // if (y2 == y1c)
				else Weight_Y = 256;

				pSrcPixels = &pSource->Pixels[(y1c * SrcScanLine) + x1c];

				// Add up all input pixels contributing to this output pixel:
				for (y2 = y1c; y2 <= y1d; y2++)
				{
					if (y2 == y1d)
					{
						if (y1c != y1d) Weight_Y = y1b;
					}

					pSrcThisPixel = pSrcPixels;
					x2 = x1c;

					if (x2 < x1d)
					{
						W = (256 - x1a) * Weight_Y;

						A += W;
						Red += pSrcThisPixel->Red * W;
						Green += pSrcThisPixel->Green * W;
						Blue += pSrcThisPixel->Blue * W;
						Alpha2 += pSrcThisPixel->Alpha * W;
						pSrcThisPixel++;
						x2++;
					}

					if (x2 < x1d)
					{
						W = 256 * Weight_Y;

						do
						{
							A += W;
							Red += pSrcThisPixel->Red * W;
							Green += pSrcThisPixel->Green * W;
							Blue += pSrcThisPixel->Blue * W;
							Alpha2 += pSrcThisPixel->Alpha * W;
							pSrcThisPixel++;
							x2++;
						} while (x2 < x1d);
					}

					if (x2 == x1d)
					{
						if (x1c != x1d) W = x1b * Weight_Y;
						else W = 256 * Weight_Y;

						A += W;
						Red += pSrcThisPixel->Red * W;
						Green += pSrcThisPixel->Green * W;
						Blue += pSrcThisPixel->Blue * W;
						Alpha2 += pSrcThisPixel->Alpha * W;
						pSrcThisPixel++;
					}

					Weight_Y = 256;
					pSrcPixels += SrcScanLine;
				}

				capi_RenderAlphaPixel(pDesThisPixel,
					((Red / A) << (8 * offsetof(PIXEL, Red))) |
					((Green / A) << (8 * offsetof(PIXEL, Green))) |
					((Blue / A) << (8 * offsetof(PIXEL, Blue))) |
					((Alpha2 / A) << (8 * offsetof(PIXEL, Alpha))),
					Alpha);

				pDesThisPixel++;
			}

			pDesPixels += DesScanLine;
		}
		break;
	}
	default:
		return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	}

	return CAPI_ERROR_NONE;
}
