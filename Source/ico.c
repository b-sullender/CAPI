
/******************************************************************************************
*
* Name: ico.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines common ICO (icon) functions.
*
*******************************************************************************************/

#include "ico.h"

CAPI_SUBFUNC(U32) ico_get_embedded_image_dimensions_mem(void* pEmbeddedFile, U32* pHeight)
{
	if (((PNG*)pEmbeddedFile)->Signature == 0xA1A0A0D474E5089)
	{
		return png_get_dimensions((PNG*)pEmbeddedFile, pHeight);
	}
	else
	{
		if (((BMPV3*)pEmbeddedFile)->HeaderSize != sizeof(BMPV3)) return 0;

		if (pHeight != 0)
		{
			if (((BMPV3*)pEmbeddedFile)->Height >= 0) *pHeight = ((BMPV3*)pEmbeddedFile)->Height / 2;
			else *pHeight = (-((BMPV3*)pEmbeddedFile)->Height) / 2;
		}

		return ((BMPV3*)pEmbeddedFile)->Width;
	}
}

CAPI_SUBFUNC(U32) ico_get_embedded_image_dimensions_file(FILE* pIcoFile, U32 ImageOffset, U32* pHeight)
{
	PNG ThisPng;
	PNG_IHDR PngIhdr;
	U32 ChunkSize, Width;
	U32 crc32chk;
	BMPV3 ThisBmp;

	if (pIcoFile == 0) return 0;

	if (fseek(pIcoFile, ImageOffset, SEEK_SET) != 0) return 0;
	if (fread(&ThisPng, 1, sizeof(PNG), pIcoFile) != sizeof(PNG)) return 0;

	if (ThisPng.Signature == 0xA1A0A0D474E5089)
	{
		if (fread(&PngIhdr, 1, sizeof(PNG_IHDR), pIcoFile) != sizeof(PNG_IHDR)) return 0;
		if (fread(&crc32chk, 1, 4, pIcoFile) != 4) return 0;

		ChunkSize = ToLittleEndian_32(&ThisPng.FirstChunk.Length);
		if (ChunkSize != sizeof(PNG_IHDR)) return 0;

		crc32chk = ToLittleEndian_32(&crc32chk);
		if (crc32(crc32(0, ThisPng.FirstChunk.ChuckType, 4), (const Bytef*)&PngIhdr, sizeof(PNG_IHDR)) != crc32chk) return 0;

		Width = ToLittleEndian_32(&PngIhdr.Width);
		if (pHeight != 0) *pHeight = ToLittleEndian_32(&PngIhdr.Height);
	}
	else
	{
		capi_memcopy(&ThisBmp, &ThisPng, sizeof(PNG));
		if (fread(&((U8*)&ThisBmp)[sizeof(PNG)], 1, sizeof(BMPV3) - sizeof(PNG), pIcoFile) != (sizeof(BMPV3) - sizeof(PNG))) return 0;

		if (ThisBmp.HeaderSize != sizeof(BMPV3)) return 0;

		if (pHeight != 0)
		{
			if (ThisBmp.Height >= 0) *pHeight = ThisBmp.Height / 2;
			else *pHeight = (-ThisBmp.Height) / 2;
		}

		Width = ThisBmp.Width;
	}

	return Width;
}

CAPI_SUBFUNC(void) ico_apply_icAND(U8* p_icAND_mask, IMAGE* pImage, size_t mask_Stride)
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

		p_icAND_mask[b] = (((pDestination[i].Alpha == 0) ? 1 : 0) << 7);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 6);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 5);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 4);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 3);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 2);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (((pDestination[i].Alpha == 0) ? 1 : 0) << 1);

		i++;
		if (i == Width)
		{
			p_icAND_mask += mask_Stride;
			pDestination += ScanLine;
			continue;
		}

		p_icAND_mask[b] |= (pDestination[i].Alpha == 0) ? 1 : 0;

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

CAPI_SUBFUNC(I32) ico_Create_BMP_Image(BMPV3** ppFilePointer, U64* pFileSize, IMAGE* pImage)
{
	PIXEL* pSource, pPalette[256];
	size_t Stride, mask_Stride;
	U32 Width, Height, PaletteLen, Length, BPP, ScanLine, X, I;
	BMPV3* pBMPV3;
	PIXEL_BGRA* pPaletteBGRA;
	void* pDestination;
	U8* p_icAND_mask;

	if ((ppFilePointer == 0) || (pFileSize == 0) || (pImage == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	pSource = pImage->Pixels;
	if (pSource == 0) return CAPI_ERROR_INVALID_PARAMETER;

	ScanLine = pImage->ScanLine;

	Width = pImage->Width;
	Height = pImage->Height;

	Length = sizeof(BMPV3);

	if (image_get_transparency_level(pImage) == 4)
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
	mask_Stride = ((((Width * 1) + 31) & ~31) >> 3);
	Length += ((U32)Stride * Height) + ((U32)mask_Stride * Height);

	pBMPV3 = (BMPV3*)capi_malloc(Length);
	if (pBMPV3 == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	pBMPV3->HeaderSize = sizeof(BMPV3);
	pBMPV3->Width = Width;
	pBMPV3->Height = Height * 2;
	pBMPV3->Planes = 1;
	pBMPV3->BitsPerPixel = (U16)BPP;
	pBMPV3->Compression = 0;
	pBMPV3->ImageSize = 0;
	pBMPV3->HorResolution = 0;
	pBMPV3->VerResolution = 0;
	pBMPV3->ColorPalette = PaletteLen;
	pBMPV3->ImportantColors = 0;

	pPaletteBGRA = (PIXEL_BGRA*)((size_t)pBMPV3 + sizeof(BMPV3));
	pDestination = (void*)((size_t)pBMPV3 + sizeof(BMPV3) + (sizeof(PIXEL_BGRA) * PaletteLen));
	p_icAND_mask = (U8*)((size_t)pDestination + (Height * Stride) + ((Height - 1) * mask_Stride));
	pDestination = (void*)((size_t)pDestination + ((Height - 1) * Stride));

	Stride = -(ptrdiff_t)Stride;
	mask_Stride = -(ptrdiff_t)mask_Stride;

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

	ico_apply_icAND(p_icAND_mask, pImage, mask_Stride);

	*ppFilePointer = pBMPV3;
	*pFileSize = Length;

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromMemory(ICO* pIcoFile, size_t FileSize, U32* pHeight, I8 IndexNumber)
{
	ICO_ENTRY* pIcoEntry, * pDesiredIcoEntry;
	void* EmbeddedFile;
	U32 I, X, Width;

	if ((pIcoFile == 0) || (FileSize == 0)) return 0;
	if ((pIcoFile->Reserved != 0) || (pIcoFile->Type != 1)) return 0;

	if (IndexNumber != -1)
	{
		if (IndexNumber >= pIcoFile->nImages) return 0;

		pDesiredIcoEntry = (ICO_ENTRY*)((size_t)pIcoFile + sizeof(ICO) + (sizeof(ICO_ENTRY) * IndexNumber));
	}
	else
	{
		pDesiredIcoEntry = (ICO_ENTRY*)((size_t)pIcoFile + sizeof(ICO));
		pIcoEntry = pDesiredIcoEntry;

		EmbeddedFile = (void*)((size_t)pIcoFile + pDesiredIcoEntry->ImageOffset);
		Width = ico_get_embedded_image_dimensions_mem(EmbeddedFile, 0);

		for (I = 1; I < pIcoFile->nImages; I++)
		{
			pIcoEntry = (ICO_ENTRY*)((size_t)pIcoEntry + sizeof(ICO_ENTRY));
			EmbeddedFile = (void*)((size_t)pIcoFile + pIcoEntry->ImageOffset);

			if (X = ico_get_embedded_image_dimensions_mem(EmbeddedFile, 0) > Width)
			{
				Width = X;
				pDesiredIcoEntry = pIcoEntry;
			}
		}
	}

	EmbeddedFile = (void*)((size_t)pIcoFile + pDesiredIcoEntry->ImageOffset);

	return ico_get_embedded_image_dimensions_mem(EmbeddedFile, pHeight);
}

CAPI_FUNC(U32) capi_Get_ICO_DimensionsFromFile(FILE* pIcoFile, U32* pHeight, I8 IndexNumber)
{
	ICO ThisIco;
	ICO_ENTRY IcoEntry, DesiredIcoEntry;
	U32 I, X, Width;

	if (pIcoFile == 0) return 0;

	if (fseek(pIcoFile, 0, SEEK_SET) != 0) return 0;
	if (fread(&ThisIco, 1, sizeof(ICO), pIcoFile) != sizeof(ICO)) return 0;

	if ((ThisIco.Reserved != 0) || (ThisIco.Type != 1)) return 0;

	if (IndexNumber != -1)
	{
		if (IndexNumber >= ThisIco.nImages) return 0;

		if (fseek(pIcoFile, sizeof(ICO) + (sizeof(ICO_ENTRY) * IndexNumber), SEEK_SET) != 0) return 0;
		if (fread(&DesiredIcoEntry, 1, sizeof(ICO_ENTRY), pIcoFile) != sizeof(ICO_ENTRY)) return 0;
	}
	else
	{
		if (fread(&DesiredIcoEntry, 1, sizeof(ICO_ENTRY), pIcoFile) != sizeof(ICO_ENTRY)) return 0;
		capi_memcopy(&IcoEntry, &DesiredIcoEntry, sizeof(ICO_ENTRY));

		Width = ico_get_embedded_image_dimensions_file(pIcoFile, DesiredIcoEntry.ImageOffset, 0);

		for (I = 1; I < ThisIco.nImages; I++)
		{
			if (fseek(pIcoFile, sizeof(ICO) + (sizeof(ICO_ENTRY) * I), SEEK_SET) != 0) return 0;
			if (fread(&IcoEntry, 1, sizeof(ICO_ENTRY), pIcoFile) != sizeof(ICO_ENTRY)) return 0;

			if (X = ico_get_embedded_image_dimensions_file(pIcoFile, IcoEntry.ImageOffset, 0) > Width)
			{
				Width = X;
				capi_memcopy(&DesiredIcoEntry, &IcoEntry, sizeof(ICO_ENTRY));
			}
		}
	}

	return ico_get_embedded_image_dimensions_file(pIcoFile, DesiredIcoEntry.ImageOffset, pHeight);
}

CAPI_FUNC(I32) capi_Load_ICO_FromMemory(IMAGE* pImage, U32 Alignment, ICO* pIcoFile, size_t FileSize, I8 IndexNumber)
{
	ICO_ENTRY* pIcoEntry, * pDesiredIcoEntry;
	void* EmbeddedFile, * pSource;
	I32 BITMAP_Height;
	size_t Stride, mask_Stride;
	U32 I, X, Width, Height, BPP, ScanLine, PaletteLen;
	PIXEL_BGRA* pPaletteBGRA;
	PIXEL* pDestination, pPalette[256];
	U8* p_icAND_mask;

	if ((pImage == 0) || (pIcoFile == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((pIcoFile->Reserved != 0) || (pIcoFile->Type != 1)) return CAPI_ERROR_INVALID_FILE_FORMAT;

	if (IndexNumber != -1)
	{
		if (IndexNumber >= pIcoFile->nImages) return CAPI_ERROR_INVALID_PARAMETER;

		pDesiredIcoEntry = (ICO_ENTRY*)((size_t)pIcoFile + sizeof(ICO) + (sizeof(ICO_ENTRY) * IndexNumber));
	}
	else
	{
		pDesiredIcoEntry = (ICO_ENTRY*)((size_t)pIcoFile + sizeof(ICO));
		pIcoEntry = pDesiredIcoEntry;

		EmbeddedFile = (void*)((size_t)pIcoFile + pDesiredIcoEntry->ImageOffset);
		Width = ico_get_embedded_image_dimensions_mem(EmbeddedFile, 0);

		for (I = 1; I < pIcoFile->nImages; I++)
		{
			pIcoEntry = (ICO_ENTRY*)((size_t)pIcoEntry + sizeof(ICO_ENTRY));
			EmbeddedFile = (void*)((size_t)pIcoFile + pIcoEntry->ImageOffset);

			if (X = ico_get_embedded_image_dimensions_mem(EmbeddedFile, 0) > Width)
			{
				Width = X;
				pDesiredIcoEntry = pIcoEntry;
			}
		}
	}

	EmbeddedFile = (void*)((size_t)pIcoFile + pDesiredIcoEntry->ImageOffset);
	Width = ico_get_embedded_image_dimensions_mem(EmbeddedFile, &Height);

	if (((PNG*)EmbeddedFile)->Signature == 0xA1A0A0D474E5089)
	{
		return capi_Load_PNG_FromMemory(pImage, Alignment, (PNG*)EmbeddedFile, FileSize);
	}
	else
	{
		if (((BMPV3*)EmbeddedFile)->HeaderSize != sizeof(BMPV3)) return CAPI_ERROR_INVALID_FILE_FORMAT;

		BPP = pDesiredIcoEntry->BitsPerPixel;
		if (BPP == 0) BPP = ((BMPV3*)EmbeddedFile)->BitsPerPixel;

		Stride = ((((Width * BPP) + 31) & ~31) >> 3);
		pSource = (void*)((size_t)EmbeddedFile + sizeof(BMPV3));

		pPaletteBGRA = 0;

		if (BPP < 16)
		{
			PaletteLen = ((BMPV3*)EmbeddedFile)->ColorPalette;
			if (PaletteLen == 0)
			{
				if (pDesiredIcoEntry->nPaletteColors != 0) PaletteLen = pDesiredIcoEntry->nPaletteColors;
			}
			pPaletteBGRA = (PIXEL_BGRA*)pSource;

			if ((PaletteLen == 0) || (PaletteLen > (1u << BPP))) PaletteLen = 1u << BPP;

			pSource = (void*)((size_t)pSource + (sizeof(PIXEL_BGRA) * PaletteLen));
		}

		mask_Stride = ((((Width * 1) + 31) & ~31) >> 3);
		BITMAP_Height = ((BMPV3*)EmbeddedFile)->Height;

		if (BITMAP_Height >= 0)
		{
			BITMAP_Height /= 2;
			p_icAND_mask = (U8*)((size_t)pSource + (Height * Stride) + (((size_t)BITMAP_Height - 1) * mask_Stride));
			pSource = (void*)((size_t)pSource + (Height - 1) * Stride);
			Stride = -(ptrdiff_t)Stride;
			mask_Stride = -(ptrdiff_t)mask_Stride;
		}
		else
		{
			BITMAP_Height = -BITMAP_Height;
			BITMAP_Height /= 2;
			p_icAND_mask = (U8*)((size_t)pSource + (Height * Stride));
		}

		pDestination = capi_CreateImage(pImage, Width, Height, 0, Alignment);
		if (pDestination == 0) return CAPI_ERROR_OUT_OF_MEMORY;

		ScanLine = pImage->ScanLine;

		switch (BPP)
		{
		case 1:
		{
			bmp_decode_palette(pPalette, 0, pPaletteBGRA, PaletteLen);
			image_decode_1bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 2:
		{
			bmp_decode_palette(pPalette, 0, pPaletteBGRA, PaletteLen);
			image_decode_2bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 4:
		{
			bmp_decode_palette(pPalette, 0, pPaletteBGRA, PaletteLen);
			image_decode_4bit_Stream(pImage, (U8*)pSource, Stride, pPalette);
			break;
		}
		case 8:
		{
			bmp_decode_palette(pPalette, 0, pPaletteBGRA, PaletteLen);

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
		case 24:
		{
			while (Height-- != 0)
			{
				for (I = 0; I < Width; I++)
				{
					pDestination[I].Red = ((PIXEL_BGR*)pSource)[I].Red;
					pDestination[I].Green = ((PIXEL_BGR*)pSource)[I].Green;
					pDestination[I].Blue = ((PIXEL_BGR*)pSource)[I].Blue;
					pDestination[I].Alpha = 0xFF;
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
				for (I = 0; I < Width; I++)
				{
					pDestination[I].Red = ((PIXEL_BGRA*)pSource)[I].Red;
					pDestination[I].Green = ((PIXEL_BGRA*)pSource)[I].Green;
					pDestination[I].Blue = ((PIXEL_BGRA*)pSource)[I].Blue;
					pDestination[I].Alpha = ((PIXEL_BGRA*)pSource)[I].Alpha;
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
	}

	if (BPP < 32) image_apply_icAND(pImage, p_icAND_mask, mask_Stride);
	else
	{
		if (image_get_transparency_level(pImage) < 3) image_apply_icAND(pImage, p_icAND_mask, mask_Stride);
	}

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_Create_ICO_ImageToMemory(ICO** ppFilePointer, U64* pFileSize, IMAGE* pImageList, U16 nImages, U8 Format, void* pParameters)
{
	I32 I, ErrorCode, Length;
	void* pFileList[64], * pThisFile;
	U64 FileSizeList[64];
	ICO* pIcoFile;
	ICO_ENTRY* pIcoEntry;

	if ((ppFilePointer == 0) || (pFileSize == 0) || (pImageList == 0) || (nImages == 0) || (nImages > 64)) return CAPI_ERROR_INVALID_PARAMETER;

	pIcoFile = 0;
	Length = 0;
	ErrorCode = CAPI_ERROR_NONE;

	capi_memset(pFileList, 0, sizeof(void*) * 64);
	capi_memset(FileSizeList, 0, sizeof(U64) * 64);

	if (Format > 1)
	{
		return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	}
	else if (Format == ICO_FORMAT_PNG)
	{
		if (pParameters == 0)
		{
			return CAPI_ERROR_INVALID_PARAMETER;
		}

		for (I = 0; I < nImages; I++)
		{
			ErrorCode = capi_Create_PNG_ImageToMemory((PNG**)&pFileList[I], &FileSizeList[I], &pImageList[I], (PNG_PARAMETERS*)pParameters);
			if (ErrorCode != CAPI_ERROR_NONE)
			{
				nImages = (U16)I;
				goto exit_func;
			}
			Length += (U32)FileSizeList[I];
		}
	}
	else if (Format == ICO_FORMAT_BMP)
	{
		for (I = 0; I < nImages; I++)
		{
			ErrorCode = ico_Create_BMP_Image((BMPV3**)&pFileList[I], &FileSizeList[I], &pImageList[I]);
			if (ErrorCode != CAPI_ERROR_NONE)
			{
				nImages = (U16)I;
				goto exit_func;
			}
			Length += (U32)FileSizeList[I];
		}
	}

	Length += sizeof(ICO) + (sizeof(ICO_ENTRY) * nImages);

	pIcoFile = (ICO*)capi_malloc(Length);
	if (pIcoFile == 0)
	{
		ErrorCode = CAPI_ERROR_OUT_OF_MEMORY;
		goto exit_func;
	}

	pIcoFile->Reserved = 0;
	pIcoFile->Type = 1;
	pIcoFile->nImages = nImages;

	pIcoEntry = (ICO_ENTRY*)((size_t)pIcoFile + sizeof(ICO));
	pThisFile = (void*)((size_t)pIcoFile + sizeof(ICO) + (sizeof(ICO_ENTRY) * nImages));

	for (I = 0; I < nImages; I++)
	{
		if (pImageList[I].Width > 255) pIcoEntry->Width = 0;
		else pIcoEntry->Width = (U8)pImageList[I].Width;

		if (pImageList[I].Height > 255) pIcoEntry->Height = 0;
		else pIcoEntry->Height = (U8)pImageList[I].Height;

		pIcoEntry->nPaletteColors = 0;
		pIcoEntry->Reserved = 0;
		pIcoEntry->Planes = 1;
		pIcoEntry->BitsPerPixel = 0;

		pIcoEntry->ImageSize = (U32)FileSizeList[I];
		pIcoEntry->ImageOffset = (U32) ((size_t)pThisFile - (size_t)pIcoFile);

		capi_memcopy(pThisFile, pFileList[I], (size_t)FileSizeList[I]);
		pThisFile = (void*)((size_t)pThisFile + FileSizeList[I]);

		pIcoEntry = (ICO_ENTRY*)((size_t)pIcoEntry + sizeof(ICO_ENTRY));
	}

exit_func:

	for (I = 0; I < nImages; I++)
	{
		capi_free(pFileList[I]);
	}

	*ppFilePointer = pIcoFile;
	*pFileSize = Length;

	return ErrorCode;
}
