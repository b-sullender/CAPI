
/******************************************************************************************
*
* Name: bmp.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines common BMP (bitmap) functions.
*
*******************************************************************************************/

#include "bmp.h"

CAPI_SUBFUNC(U32) bmp_ctz(U32 Value)
{
	U32 I;

	for (I = 0; I < 32; I++)
	{
		if ((Value & 0x1) == 1) break;
		Value >>= 1;
	}

	return I;
}

CAPI_SUBFUNC(U8) bmp_color_from_mask(U32 Color, U32 Mask)
{
	U32 tz;

	if (Mask == 0) return 0;

	tz = bmp_ctz(Mask);

	Mask >>= tz;
	Color >>= tz;
	Color &= Mask;

	if (Mask < 255) Color *= (255 / Mask);
	else if (Mask > 255) Color = (255 * Color) / Mask;

	return (U8)Color;
}

CAPI_SUBFUNC(void) bmp_encode_palette(PIXEL_BGR* pDesBGR, PIXEL_BGRA* pDesBGRA, PIXEL* pSrcPalette, U32 PaletteLen)
{
	U32 I;

	if (pDesBGR != 0)
	{
		for (I = 0; I < PaletteLen; I++)
		{
			pDesBGR[I].Red = pSrcPalette[I].Red;
			pDesBGR[I].Green = pSrcPalette[I].Green;
			pDesBGR[I].Blue = pSrcPalette[I].Blue;
		}
	}
	else if (pDesBGRA != 0)
	{
		for (I = 0; I < PaletteLen; I++)
		{
			pDesBGRA[I].Red = pSrcPalette[I].Red;
			pDesBGRA[I].Green = pSrcPalette[I].Green;
			pDesBGRA[I].Blue = pSrcPalette[I].Blue;
			pDesBGRA[I].Alpha = pSrcPalette[I].Alpha;
		}
	}
}

CAPI_SUBFUNC(void) bmp_decode_palette(PIXEL* pDesPalette, PIXEL_BGR* pSrcBGR, PIXEL_BGRA* pSrcBGRA, U32 PaletteLen)
{
	U32 I;

	if (pSrcBGR != 0)
	{
		for (I = 0; I < PaletteLen; I++)
		{
			pDesPalette[I].Red = pSrcBGR[I].Red;
			pDesPalette[I].Green = pSrcBGR[I].Green;
			pDesPalette[I].Blue = pSrcBGR[I].Blue;
			pDesPalette[I].Alpha = 0xFF;
		}
	}
	else if (pSrcBGRA != 0)
	{
		for (I = 0; I < PaletteLen; I++)
		{
			pDesPalette[I].Red = pSrcBGRA[I].Red;
			pDesPalette[I].Green = pSrcBGRA[I].Green;
			pDesPalette[I].Blue = pSrcBGRA[I].Blue;
			pDesPalette[I].Alpha = 0xFF;
		}
	}
}

CAPI_SUBFUNC(I32) bmp_decode_rle4(U8* In, U32 InLen, PIXEL* pPalette, PIXEL_BGRA* Out, U32 OutLen, U32 Width)
{
	U32 InPos, X, Y, I, C, C2, xMove, yMove;
	U8 Code, Code2;

	InPos = X = Y = 0;

	while (InPos < InLen)
	{
		Code = In[InPos++];

		if (InPos >= InLen) return 0;
		Code2 = In[InPos++];

		if (Code == 0) // Absolute mode, or Encoded mode with escape character
		{
			if (Code2 == 0) // End of line
			{
				X = 0;
				Y += Width;
			}
			else if (Code2 == 1) // End of bitmap
			{
				return 1;
			}
			else if (Code2 == 2) // Delta
			{
				if (InPos >= InLen) return 0;
				xMove = In[InPos++];
				if (InPos >= InLen) return 0;
				yMove = In[InPos++];

				for (I = 0; I < yMove; I++)
				{
					for (C = X; C < Width + X; C++)
					{
						if ((C + Y) >= OutLen) return 0;
						Out[C + Y].Alpha = 0;
					}
					Y += Width;
				}

				for (I = 0; I < xMove; I++)
				{
					if ((X + Y) >= OutLen) return 0;
					Out[X + Y].Alpha = 0;
					X++;
				}
			}
			else
			{
				I = 0;
				while (I < Code2)
				{
					if (InPos >= InLen) return 0;
					C = In[InPos++];
					C2 = C & 0xF;
					C >>= 4;

					if ((X + Y) >= OutLen) return 0;
					Out[X + Y].Red = pPalette[C].Red;
					Out[X + Y].Green = pPalette[C].Green;
					Out[X + Y].Blue = pPalette[C].Blue;
					Out[X + Y].Alpha = pPalette[C].Alpha;
					X++;

					I++;
					if (I == Code2) break;

					if ((X + Y) >= OutLen) return 0;
					Out[X + Y].Red = pPalette[C2].Red;
					Out[X + Y].Green = pPalette[C2].Green;
					Out[X + Y].Blue = pPalette[C2].Blue;
					Out[X + Y].Alpha = pPalette[C2].Alpha;
					X++;

					I++;
				}

				InPos += (InPos % 2);
			}
		}
		else // Encoded mode
		{
			C = Code2 >> 4;
			C2 = Code2 & 0xF;

			I = 0;
			while (I < Code)
			{
				if ((X + Y) >= OutLen) return 0;
				Out[X + Y].Red = pPalette[C].Red;
				Out[X + Y].Green = pPalette[C].Green;
				Out[X + Y].Blue = pPalette[C].Blue;
				Out[X + Y].Alpha = pPalette[C].Alpha;
				X++;

				I++;
				if (I == Code) break;

				if ((X + Y) >= OutLen) return 0;
				Out[X + Y].Red = pPalette[C2].Red;
				Out[X + Y].Green = pPalette[C2].Green;
				Out[X + Y].Blue = pPalette[C2].Blue;
				Out[X + Y].Alpha = pPalette[C2].Alpha;
				X++;

				I++;
			}
		}
	}

	return 1;
}

CAPI_SUBFUNC(I32) bmp_decode_rle8(U8* In, U32 InLen, PIXEL* pPalette, PIXEL_BGRA* Out, U32 OutLen, U32 Width)
{
	U32 InPos, X, Y, I, C, xMove, yMove;
	U8 Code, Code2;

	InPos = X = Y = 0;

	while (InPos < InLen)
	{
		Code = In[InPos++];

		if (InPos >= InLen) return 0;
		Code2 = In[InPos++];

		if (Code == 0) // Absolute mode, or Encoded mode with escape character
		{
			if (Code2 == 0) // End of line
			{
				X = 0;
				Y += Width;
			}
			else if (Code2 == 1) // End of bitmap
			{
				return 1;
			}
			else if (Code2 == 2) // Delta
			{
				if (InPos >= InLen) return 0;
				xMove = In[InPos++];
				if (InPos >= InLen) return 0;
				yMove = In[InPos++];

				for (I = 0; I < yMove; I++)
				{
					for (C = X; C < Width + X; C++)
					{
						if ((C + Y) >= OutLen) return 0;
						Out[C + Y].Alpha = 0;
					}
					Y += Width;
				}

				for (I = 0; I < xMove; I++)
				{
					if ((X + Y) >= OutLen) return 0;
					Out[X + Y].Alpha = 0;
					X++;
				}
			}
			else
			{
				for (I = 0; I < Code2; I++)
				{
					if (InPos >= InLen) return 0;
					C = In[InPos++];

					if ((X + Y) >= OutLen) return 0;
					Out[X + Y].Red = pPalette[C].Red;
					Out[X + Y].Green = pPalette[C].Green;
					Out[X + Y].Blue = pPalette[C].Blue;
					Out[X + Y].Alpha = pPalette[C].Alpha;
					X++;
				}

				InPos += (InPos % 2);
			}
		}
		else // Encoded mode
		{
			C = Code2;

			for (I = 0; I < Code; I++)
			{
				if ((X + Y) >= OutLen) return 0;
				Out[X + Y].Red = pPalette[C].Red;
				Out[X + Y].Green = pPalette[C].Green;
				Out[X + Y].Blue = pPalette[C].Blue;
				Out[X + Y].Alpha = pPalette[C].Alpha;
				X++;
			}
		}
	}

	return 1;
}

CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromMemory(BMP* pBmpFile, size_t FileSize, U32* pHeight)
{
	BMPV2* BmpV2Info;
	BMPINFOHEADER* BmpInfo;
	I32 BITMAP_Height;
	U32 Width;

	if ((pBmpFile == 0) || (FileSize == 0)) return 0;
	if (pBmpFile->Type != 0x4D42) return 0;

	BmpInfo = 0;
	BmpV2Info = (BMPV2*)((size_t)pBmpFile + sizeof(BMP));

	if ((BmpV2Info->HeaderSize < sizeof(BMPV2)) || (BmpV2Info->HeaderSize > sizeof(BMPINFOHEADER))) return 0;

	if (BmpV2Info->HeaderSize == 12)
	{
		Width = BmpV2Info->Width;
		BITMAP_Height = BmpV2Info->Height;
	}
	else
	{
		BmpInfo = (BMPINFOHEADER*)BmpV2Info;
		Width = BmpInfo->Width;
		BITMAP_Height = BmpInfo->Height;
	}

	if (pHeight != 0)
	{
		if (BITMAP_Height >= 0) *pHeight = BITMAP_Height;
		else *pHeight = -BITMAP_Height;
	}

	return Width;
}

CAPI_FUNC(U32) capi_Get_BMP_DimensionsFromFile(FILE* pBmpFile, U32* pHeight)
{
	BMP ThisBmp;
	BMPV2 BmpV2Header;
	BMPV3 BmpV3Header;
	I32 BITMAP_Height;
	U32 Width;

	if (pBmpFile == 0) return 0;

	if (fseek(pBmpFile, 0, SEEK_SET) != 0) return 0;
	if (fread(&ThisBmp, 1, sizeof(BMP), pBmpFile) != sizeof(BMP)) return 0;

	if (ThisBmp.Type != 0x4D42) return 0;

	if (fread(&BmpV2Header, 1, sizeof(BMPV2), pBmpFile) != sizeof(BMPV2)) return 0;

	if (BmpV2Header.HeaderSize == sizeof(BMPV2))
	{
		BITMAP_Height = BmpV2Header.Height;

		if (pHeight != 0)
		{
			if (BITMAP_Height >= 0) *pHeight = BITMAP_Height;
			else *pHeight = -BITMAP_Height;
		}
		Width = BmpV2Header.Width;
	}
	else
	{
		if (BmpV2Header.HeaderSize < sizeof(BMPV2)) return 0;
		capi_memcopy(&BmpV3Header, &BmpV2Header, sizeof(BMPV2));

		if (pHeight != 0)
		{
			if (BmpV3Header.Height >= 0) *pHeight = BmpV3Header.Height;
			else *pHeight = -BmpV3Header.Height;
		}
		Width = BmpV3Header.Width;
	}

	return Width;
}

CAPI_FUNC(I32) capi_Load_BMP_FromMemory(IMAGE* pImage, U32 Alignment, BMP* pBmpFile, size_t FileSize)
{
	BMPV2* BmpV2Info;
	BMPINFOHEADER* BmpInfo;
	I32 BITMAP_Height;
	size_t Stride;
	U32 Width, Height, BPP, Compression, PaletteLen, ImageSize, ScanLine, X, I, RedMask, GreenMask, BlueMask, AlphaMask;
	void* pSource;
	PIXEL_BGR* pPaletteBGR;
	PIXEL_BGRA* pPaletteBGRA, * pRLE_Image;
	PIXEL* pDestination, pPalette[256];

	if ((pImage == 0) || (pBmpFile == 0) || (FileSize == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if (pBmpFile->Type != 0x4D42) return CAPI_ERROR_INVALID_FILE_FORMAT;

	pPaletteBGR = 0;
	pPaletteBGRA = 0;

	BmpInfo = 0;
	BmpV2Info = (BMPV2*)((size_t)pBmpFile + sizeof(BMP));

	if (BmpV2Info->HeaderSize == 12)
	{
		Width = BmpV2Info->Width;
		BITMAP_Height = BmpV2Info->Height;
		BPP = BmpV2Info->BitsPerPixel;
		Compression = 0;
		PaletteLen = 0;
		pPaletteBGR = (PIXEL_BGR*)((size_t)BmpV2Info + BmpV2Info->HeaderSize);
		ImageSize = 0;
	}
	else
	{
		BmpInfo = (BMPINFOHEADER*)BmpV2Info;
		Width = BmpInfo->Width;
		BITMAP_Height = BmpInfo->Height;
		BPP = BmpInfo->BitsPerPixel;
		Compression = BmpInfo->Compression;
		PaletteLen = BmpInfo->ColorPalette;
		pPaletteBGRA = (PIXEL_BGRA*)((size_t)BmpInfo + BmpInfo->HeaderSize);
		ImageSize = BmpInfo->ImageSize;
	}

	if ((BmpV2Info->HeaderSize < sizeof(BMPV2)) || (BmpV2Info->HeaderSize > sizeof(BMPINFOHEADER))) return CAPI_ERROR_INVALID_FILE_FORMAT;

	if (BPP > 64) return CAPI_ERROR_INVALID_FILE_FORMAT;
	if (Width > 0x9000) return CAPI_ERROR_INVALID_FILE_FORMAT;

	Stride = ((((Width * BPP) + 31) & ~31) >> 3);
	pSource = (void*)((size_t)pBmpFile + pBmpFile->PixelStart);

	if (BITMAP_Height > 0)
	{
		Height = BITMAP_Height;
		pSource = (void*)((size_t)pSource + (Height - 1) * Stride);
		Stride = -(ptrdiff_t)Stride;
	}
	else Height = -BITMAP_Height;

	if (Height > 0x9000) return CAPI_ERROR_INVALID_FILE_FORMAT;

	pDestination = capi_CreateImage(pImage, Width, Height, 0, Alignment);
	if (pDestination == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	ScanLine = pImage->ScanLine;

	switch (Compression)
	{
	case 0:
	{
		switch (BPP)
		{
		case 1:
		{
			if ((PaletteLen == 0) || (PaletteLen > 2)) PaletteLen = 2;
			bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);
			image_decode_1bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 2:
		{
			if ((PaletteLen == 0) || (PaletteLen > 4)) PaletteLen = 4;
			bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);
			image_decode_2bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 4:
		{
			if ((PaletteLen == 0) || (PaletteLen > 16)) PaletteLen = 16;
			bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);
			image_decode_4bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 8:
		{
			if ((PaletteLen == 0) || (PaletteLen > 256)) PaletteLen = 256;
			bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);

			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					I = ((U8*)pSource)[X];

					pDestination[X].Red = pPalette[I].Red;
					pDestination[X].Green = pPalette[I].Green;
					pDestination[X].Blue = pPalette[I].Blue;
					pDestination[X].Alpha = pPalette[I].Alpha;
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		case 16:
		{
			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					pDestination[X].Red = ((((U16*)pSource)[X] >> 10) & 0x1F) * 8;
					pDestination[X].Green = ((((U16*)pSource)[X] >> 5) & 0x1F) * 8;
					pDestination[X].Blue = (((U16*)pSource)[X] & 0x1F) * 8;
					pDestination[X].Alpha = 0xFF;
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		case 24:
		{
			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					pDestination[X].Red = ((PIXEL_BGR*)pSource)[X].Red;
					pDestination[X].Green = ((PIXEL_BGR*)pSource)[X].Green;
					pDestination[X].Blue = ((PIXEL_BGR*)pSource)[X].Blue;
					pDestination[X].Alpha = 0xFF;
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		case 32:
		{
			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					pDestination[X].Red = ((PIXEL_BGRA*)pSource)[X].Red;
					pDestination[X].Green = ((PIXEL_BGRA*)pSource)[X].Green;
					pDestination[X].Blue = ((PIXEL_BGRA*)pSource)[X].Blue;
					pDestination[X].Alpha = ((PIXEL_BGRA*)pSource)[X].Alpha;
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		default:
			capi_FreeImage(pImage);
			return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
			break;
		}
		break;
	}
	case 1:
	{
		if (BPP != 8)
		{
			capi_FreeImage(pImage);
			return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
		}

		Stride = ((((Width * 32) + 31) & ~31) >> 3);
		pSource = (void*)((size_t)pBmpFile + pBmpFile->PixelStart);

		pRLE_Image = (PIXEL_BGRA*)capi_malloc(Stride * Height);
		if (pRLE_Image == 0)
		{
			capi_FreeImage(pImage);
			return CAPI_ERROR_OUT_OF_MEMORY;
		}

		capi_memset32((U32*)pRLE_Image, 0xFFFFFFFF, Width * Height);

		if ((PaletteLen == 0) || (PaletteLen > 256)) PaletteLen = 256;
		bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);
		bmp_decode_rle8((U8*)pSource, ImageSize, pPalette, pRLE_Image, Width * Height, Width);

		pSource = pRLE_Image;

		if (BITMAP_Height > 0)
		{
			pRLE_Image = (PIXEL_BGRA*)((size_t)pRLE_Image + (Height - 1) * Stride);
			Stride = -(ptrdiff_t)Stride;
		}

		while (Height-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				pDestination[X].Red = pRLE_Image[X].Red;
				pDestination[X].Green = pRLE_Image[X].Green;
				pDestination[X].Blue = pRLE_Image[X].Blue;
				pDestination[X].Alpha = pRLE_Image[X].Alpha;
			}

			pDestination += ScanLine;
			pRLE_Image = (PIXEL_BGRA*)((size_t)pRLE_Image + Stride);
		}
		capi_free(pSource);

		break;
	}
	case 2:
	{
		if (BPP != 4)
		{
			capi_FreeImage(pImage);
			return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
		}

		Stride = ((((Width * 32) + 31) & ~31) >> 3);
		pSource = (void*)((size_t)pBmpFile + pBmpFile->PixelStart);

		pRLE_Image = (PIXEL_BGRA*)capi_malloc(Stride * Height);
		if (pRLE_Image == 0)
		{
			capi_FreeImage(pImage);
			return CAPI_ERROR_OUT_OF_MEMORY;
		}

		capi_memset32((U32*)pRLE_Image, 0xFFFFFFFF, Width * Height);

		if ((PaletteLen == 0) || (PaletteLen > 16)) PaletteLen = 16;
		bmp_decode_palette(pPalette, pPaletteBGR, pPaletteBGRA, PaletteLen);
		bmp_decode_rle4((U8*)pSource, ImageSize, pPalette, pRLE_Image, Width * Height, Width);

		pSource = pRLE_Image;

		if (BITMAP_Height > 0)
		{
			pRLE_Image = (PIXEL_BGRA*)((size_t)pRLE_Image + (Height - 1) * Stride);
			Stride = -(ptrdiff_t)Stride;
		}

		while (Height-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				pDestination[X].Red = pRLE_Image[X].Red;
				pDestination[X].Green = pRLE_Image[X].Green;
				pDestination[X].Blue = pRLE_Image[X].Blue;
				pDestination[X].Alpha = pRLE_Image[X].Alpha;
			}

			pDestination += ScanLine;
			pRLE_Image = (PIXEL_BGRA*)((size_t)pRLE_Image + Stride);
		}
		capi_free(pSource);

		break;
	}
	case 3:
	{
		if (BmpInfo == 0)
		{
			RedMask = 0;
			GreenMask = 0;
			BlueMask = 0;
			AlphaMask = 0;
		}
		else
		{
			RedMask = BmpInfo->RedMask;
			GreenMask = BmpInfo->GreenMask;
			BlueMask = BmpInfo->BlueMask;
			AlphaMask = BmpInfo->AlphaMask;
		}

		switch (BPP)
		{
		case 16:
		{
			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					pDestination[X].Red = bmp_color_from_mask(((U16*)pSource)[X], RedMask);
					pDestination[X].Green = bmp_color_from_mask(((U16*)pSource)[X], GreenMask);
					pDestination[X].Blue = bmp_color_from_mask(((U16*)pSource)[X], BlueMask);
					if (AlphaMask == 0) pDestination[X].Alpha = 0xFF;
					else pDestination[X].Alpha = bmp_color_from_mask(((U16*)pSource)[X], AlphaMask);
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		case 32:
		{
			while (Height-- != 0)
			{
				for (X = 0; X < Width; X++)
				{
					pDestination[X].Red = bmp_color_from_mask(((U32*)pSource)[X], RedMask);
					pDestination[X].Green = bmp_color_from_mask(((U32*)pSource)[X], GreenMask);
					pDestination[X].Blue = bmp_color_from_mask(((U32*)pSource)[X], BlueMask);
					if (AlphaMask == 0) pDestination[X].Alpha = 0xFF;
					else pDestination[X].Alpha = bmp_color_from_mask(((U32*)pSource)[X], AlphaMask);
				}

				pDestination += ScanLine;
				pSource = (void*)((size_t)pSource + Stride);
			}
			break;
		}
		default:
			capi_FreeImage(pImage);
			return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
			break;
		}
		break;
	}
	default:
		capi_FreeImage(pImage);
		return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
		break;
	}

	image_test_adjust_alpha_pixels(pImage);

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_Create_BMP_ImageToMemory(BMP** ppFilePointer, U64* pFileSize, IMAGE* pImage)
{
	PIXEL* pSource, pPalette[256];
	size_t Stride;
	U32 Width, Height, PaletteLen, Length, BPP, ScanLine, X, I;
	BMP* pBMP; BMPV3* pBMPV3;
	PIXEL_BGRA* pPaletteBGRA;
	void* pDestination;

	if ((ppFilePointer == 0) || (pFileSize == 0) || (pImage == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	pSource = pImage->Pixels;
	if (pSource == 0) return CAPI_ERROR_INVALID_PARAMETER;

	ScanLine = pImage->ScanLine;

	Width = pImage->Width;
	Height = pImage->Height;

	Length = sizeof(BMP);

	if (image_get_transparency_level(pImage) > 1)
	{
		BPP = 32;
		PaletteLen = 0;
	}
	else
	{
		PaletteLen = image_generate_palette_from_image(pPalette, pImage);
		if ((I32)PaletteLen == -1)
		{
			BPP = 24;
			PaletteLen = 0;
		}
		else
		{
			Length += sizeof(PIXEL_BGRA) * PaletteLen;

			if (PaletteLen <= 2) BPP = 1;
			else if (PaletteLen <= 4) BPP = 2;
			else if (PaletteLen <= 16) BPP = 4;
			else BPP = 8;
		}
	}

	Stride = ((((Width * BPP) + 31) & ~31) >> 3);
	Length += (U32)Stride * Height;
	pDestination = (void*)((Height - 1) * Stride);

	Stride = -(ptrdiff_t)Stride;

	Length += sizeof(BMPV3);

	pBMP = (BMP*)capi_malloc(Length);
	if (pBMP == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	pBMPV3 = (BMPV3*)((size_t)pBMP + sizeof(BMP));

	pBMP->Type = 0x4D42;
	pBMP->BmpSize = Length;
	pBMP->Reserved1 = 0;
	pBMP->Reserved2 = 0;
	pBMP->PixelStart = sizeof(BMP) + sizeof(BMPV3) + (sizeof(PIXEL_BGRA) * PaletteLen);

	pBMPV3->HeaderSize = sizeof(BMPV3);
	pBMPV3->Width = Width;
	pBMPV3->Height = Height;
	pBMPV3->Planes = 1;
	pBMPV3->BitsPerPixel = (U16)BPP;
	pBMPV3->Compression = 0;
	pBMPV3->ImageSize = 0;
	pBMPV3->HorResolution = 0;
	pBMPV3->VerResolution = 0;
	pBMPV3->ColorPalette = PaletteLen;
	pBMPV3->ImportantColors = 0;

	pPaletteBGRA = (PIXEL_BGRA*)((size_t)pBMPV3 + sizeof(BMPV3));
	pDestination = (void*)((size_t)pDestination + (size_t)pBMP + pBMP->PixelStart);

	switch (BPP)
	{
	case 1:
	{
		bmp_encode_palette(0, pPaletteBGRA, pPalette, PaletteLen);
		image_encode_1bit_Stream((U8*)pDestination, pImage, Stride, pPalette);
		break;
	}
	case 2:
	{
		bmp_encode_palette(0, pPaletteBGRA, pPalette, PaletteLen);
		image_encode_2bit_Stream((U8*)pDestination, pImage, Stride, pPalette);
		break;
	}
	case 4:
	{
		bmp_encode_palette(0, pPaletteBGRA, pPalette, PaletteLen);
		image_encode_4bit_Stream((U8*)pDestination, pImage, Stride, pPalette);
		break;
	}
	case 8:
	{
		bmp_encode_palette(0, pPaletteBGRA, pPalette, PaletteLen);

		while (Height-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				I = (U32)(image_find_color_in_palette(&pSource[X], pPalette, PaletteLen) - pPalette);
				((U8*)pDestination)[X] = (U8)I;
			}

			pSource += ScanLine;
			pDestination = (void*)((size_t)pDestination + Stride);
		}
		break;
	}
	case 24:
	{
		while (Height-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				((PIXEL_BGR*)pDestination)[X].Red = pSource[X].Red;
				((PIXEL_BGR*)pDestination)[X].Green = pSource[X].Green;
				((PIXEL_BGR*)pDestination)[X].Blue = pSource[X].Blue;
			}

			pSource += ScanLine;
			pDestination = (void*)((size_t)pDestination + Stride);
		}
		break;
	}
	case 32:
	{
		while (Height-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				((PIXEL_BGRA*)pDestination)[X].Red = pSource[X].Red;
				((PIXEL_BGRA*)pDestination)[X].Green = pSource[X].Green;
				((PIXEL_BGRA*)pDestination)[X].Blue = pSource[X].Blue;
				((PIXEL_BGRA*)pDestination)[X].Alpha = pSource[X].Alpha;
			}

			pSource += ScanLine;
			pDestination = (void*)((size_t)pDestination + Stride);
		}
		break;
	}
	default:
		break;
	}

	*ppFilePointer = pBMP;
	*pFileSize = Length;

	return CAPI_ERROR_NONE;
}
