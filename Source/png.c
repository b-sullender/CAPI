
/******************************************************************************************
*
* Name: png.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines common PNG (Portable Network Graphics) functions.
*
*******************************************************************************************/

#include "png.h"

static const U32 ADAM7IX[7] = { 0, 4, 0, 2, 0, 1, 0 }; /* X Start Values */
static const U32 ADAM7IY[7] = { 0, 0, 4, 0, 2, 0, 1 }; /* Y Start Values */
static const U32 ADAM7DX[7] = { 8, 8, 4, 4, 2, 2, 1 }; /* X Delta Values */
static const U32 ADAM7DY[7] = { 8, 8, 8, 4, 4, 2, 2 }; /* Y Delta Values */

CAPI_SUBFUNC(I32) png_AbsL(I32 X)
{
	if (X < 0) return -X;
	else return X;
}

CAPI_SUBFUNC(U32) png_get_dimensions(PNG* pPngFile, U32* pHeight)
{
	U32 Width, ChunkSize;
	PNG_IHDR* pIhdr;

	ChunkSize = ToLittleEndian_32(&pPngFile->FirstChunk.Length);
	if (ChunkSize != sizeof(PNG_IHDR)) return 0;

	pIhdr = (PNG_IHDR*)((size_t)&pPngFile->FirstChunk + sizeof(PNG_CHUNK));

	Width = ToLittleEndian_32(&pIhdr->Width);
	if (pHeight != 0) *pHeight = ToLittleEndian_32(&pIhdr->Height);

	return Width;
}

CAPI_SUBFUNC(U32) png_GetBitsPerPixel(U32 ColorType, U32 BitDepth)
{
	switch (ColorType)
	{
	case 0:
		switch (BitDepth)
		{
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			return BitDepth;
		default:
			return 0;
		}
	case 2:
		switch (BitDepth)
		{
		case 8:
		case 16:
			return 3 * BitDepth;
		default:
			return 0;
		}
	case 3:
		switch (BitDepth)
		{
		case 1:
		case 2:
		case 4:
		case 8:
			return BitDepth;
		default:
			return 0;
		}
	case 4:
		switch (BitDepth)
		{
		case 8:
		case 16:
			return 2 * BitDepth;
		default:
			return 0;
		}
	case 6:
		switch (BitDepth)
		{
		case 8:
		case 16:
			return 4 * BitDepth;
		default:
			return 0;
		}
	default:
		return 0;
	}
}

CAPI_SUBFUNC(U8) png_IsChunk(U8* Chunk1, U8* Chunk2)
{
	U8 I, IsChunk;

	IsChunk = TRUE;

	for (I = 0; I < 4; I++)
	{
		if (Chunk1[I] != Chunk2[I]) IsChunk = FALSE;
	}

	return IsChunk;
}

CAPI_SUBFUNC(U32) png_GetCombinedIdatSize(PNG_CHUNK* FirstChunk)
{
	U32 ChunkSize, ZlibSize;
	PNG_CHUNK* ThisChunk, * NextChunk;

	ZlibSize = 0;
	ThisChunk = FirstChunk;

	do
	{
		ChunkSize = ToLittleEndian_32(&ThisChunk->Length);
		NextChunk = (PNG_CHUNK*)((size_t)ThisChunk + ChunkSize + 12);
		if (png_IsChunk(ThisChunk->ChuckType, (U8*)"IDAT")) ZlibSize += ChunkSize;
		ThisChunk = NextChunk;
	} while (!png_IsChunk(ThisChunk->ChuckType, (U8*)"IEND"));

	return ZlibSize;
}

CAPI_SUBFUNC(U8) png_PaethPredictor(U16 A, U16 B, U16 C)
{
	U16 PA, PB, PC;

	PA = (U16)png_AbsL(B - C);
	PB = (U16)png_AbsL(A - C);
	PC = (U16)png_AbsL(A + B - C - C);

	if (PC < PA && PC < PB) return (U8)C;
	else if (PB < PA) return (U8)B;
	else return (U8)A;
}

CAPI_SUBFUNC(void) png_encode_filter_type_0(U8* pData, U32 Stride, U32 ScanLines)
{
	U32 I;

	while (ScanLines-- != 0)
	{
		for (I = Stride - 1; I != 0; I--) pData[I] = pData[I - 1];
		pData[0] = 0;

		pData += Stride;
	}
}

CAPI_SUBFUNC(I32) png_decode_scanLine_filter_type_0(U8* Recon, U8* ScanLine, U8* Precon, U32 ByteWidth, U8 FilterType, U32 Length)
{
	U32 I;

	switch (FilterType)
	{
	case 0:
		for (I = 0; I < Length; I++) Recon[I] = ScanLine[I];
		break;
	case 1:
		for (I = 0; I < ByteWidth; I++) Recon[I] = ScanLine[I];
		for (I = ByteWidth; I < Length; I++) Recon[I] = ScanLine[I] + Recon[I - ByteWidth];
		break;
	case 2:
		if (Precon)
		{
			for (I = 0; I < Length; I++) Recon[I] = ScanLine[I] + Precon[I];
		}
		else
		{
			for (I = 0; I < Length; I++) Recon[I] = ScanLine[I];
		}
		break;
	case 3:
		if (Precon)
		{
			for (I = 0; I < ByteWidth; I++) Recon[I] = ScanLine[I] + Precon[I] / 2;
			for (I = ByteWidth; I < Length; I++) Recon[I] = ScanLine[I] + ((Recon[I - ByteWidth] + Precon[I]) / 2);
		}
		else
		{
			for (I = 0; I < ByteWidth; I++) Recon[I] = ScanLine[I];
			for (I = ByteWidth; I < Length; I++) Recon[I] = ScanLine[I] + Recon[I - ByteWidth] / 2;
		}
		break;
	case 4:
		if (Precon)
		{
			for (I = 0; I < ByteWidth; I++) Recon[I] = (ScanLine[I] + Precon[I]);
			for (I = ByteWidth; I < Length; I++)
			{
				Recon[I] = (ScanLine[I] + png_PaethPredictor(Recon[I - ByteWidth], Precon[I], Precon[I - ByteWidth]));
			}
		}
		else
		{
			for (I = 0; I < ByteWidth; I++) Recon[I] = ScanLine[I];
			for (I = ByteWidth; I < Length; I++) Recon[I] = (ScanLine[I] + Recon[I - ByteWidth]);
		}
		break;
	default: return CAPI_ERROR_INVALID_FILE_FORMAT; /* Error: Unexisting filter type given */
	}

	return CAPI_ERROR_NONE;
}

CAPI_SUBFUNC(I32) png_decode_filter_type_0(U8* Out, U8* In, U32 Width, U32 Height, U32 BPP)
{
	U8* PrevLine, FilterType;
	U32 ByteWidth, LineBytes, H, OutIndex, InIndex;
	I32 retVal;

	PrevLine = 0;

	ByteWidth = (BPP + 7) / 8;
	LineBytes = (Width * BPP + 7) / 8;

	for (H = 0; H < Height; H++)
	{
		OutIndex = LineBytes * H;
		InIndex = (1 + LineBytes) * H;
		FilterType = In[InIndex];

		retVal = png_decode_scanLine_filter_type_0(&Out[OutIndex], &In[InIndex + 1], PrevLine, ByteWidth, FilterType, LineBytes);
		if (retVal != CAPI_ERROR_NONE) return retVal;

		PrevLine = &Out[OutIndex];
	}

	return CAPI_ERROR_NONE;
}

CAPI_SUBFUNC(U8) png_ReadBitFromReversedStream(U32* BitPointer, U8* BitStream)
{
	U8 Result;

	Result = (U8)((BitStream[(*BitPointer) >> 3] >> (7 - ((*BitPointer) & 0x7))) & 1);
	*BitPointer += 1;

	return Result;
}

CAPI_SUBFUNC(void) png_SetBitOfReversedStream(U32* BitPointer, U8* BitStream, U8 Bit)
{
	if (Bit == 0) BitStream[(*BitPointer) >> 3] &= (U8)(~(1 << (7 - ((*BitPointer) & 0x7))));
	else BitStream[(*BitPointer) >> 3] |= (1 << (7 - ((*BitPointer) & 0x7)));

	*BitPointer += 1;
}

CAPI_SUBFUNC(void) png_RemovePaddingBits(U8* Out, U8* In, U32 oLineBits, U32 iLineBits, U32 Height)
{
	U8 Bit;
	U32 X, Y, Diff, iBP, oBP;

	iBP = oBP = 0;
	Diff = iLineBits - oLineBits;
	for (Y = 0; Y < Height; Y++)
	{
		for (X = 0; X < oLineBits; X++)
		{
			Bit = png_ReadBitFromReversedStream(&iBP, In);
			png_SetBitOfReversedStream(&oBP, Out, Bit);
		}
		iBP += Diff;
	}
}

CAPI_SUBFUNC(void) png_Adam7_GetPassValues(U32* Passw, U32* Passh, U32* FilterPassStart, U32* PaddedPassStart, U32* PassStart, U32 Width, U32 Height, U32 BPP)
{
	U32 I;

	for (I = 0; I < 7; I++)
	{
		Passw[I] = (Width + ADAM7DX[I] - ADAM7IX[I] - 1) / ADAM7DX[I];
		Passh[I] = (Height + ADAM7DY[I] - ADAM7IY[I] - 1) / ADAM7DY[I];
		if (Passw[I] == 0) Passh[I] = 0;
		if (Passh[I] == 0) Passw[I] = 0;
	}

	FilterPassStart[0] = PaddedPassStart[0] = PassStart[0] = 0;

	for (I = 0; I < 6; I++)
	{
		FilterPassStart[I + 1] = FilterPassStart[I] + ((Passw[I] && Passh[I]) ? Passh[I] * (1 + (Passw[I] * BPP + 7) / 8) : 0);
		PaddedPassStart[I + 1] = PaddedPassStart[I] + Passh[I] * ((Passw[I] * BPP + 7) / 8);
		PassStart[I + 1] = PassStart[I] + (Passh[I] * Passw[I] * BPP + 7) / 8;
	}
}

CAPI_SUBFUNC(void) png_Adam7_Deinterlace(U8* Out, U8* In, U32 Width, U32 Height, U32 BPP)
{
	U8 Bit;
	U32 I, X, Y, B, PixelInStart, PixelOutStart, oBP, iBP, iLineBits, oLineBits, ByteWidth;
	U32 Passw[7], Passh[7], FilterPassStart[7], PaddedPassStart[7], PassStart[7];

	png_Adam7_GetPassValues(Passw, Passh, FilterPassStart, PaddedPassStart, PassStart, Width, Height, BPP);

	if (BPP > 7)
	{
		for (I = 0; I < 7; I++)
		{
			ByteWidth = BPP / 8;

			for (Y = 0; Y < Passh[I]; Y++)
			{
				for (X = 0; X < Passw[I]; X++)
				{
					PixelInStart = PassStart[I] + (Y * Passw[I] + X) * ByteWidth;
					PixelOutStart = ((ADAM7IY[I] + Y * ADAM7DY[I]) * Width + ADAM7IX[I] + X * ADAM7DX[I]) * ByteWidth;

					for (B = 0; B < ByteWidth; B++)
					{
						Out[PixelOutStart + B] = In[PixelInStart + B];
					}
				}
			}
		}
	}
	else
	{
		for (I = 0; I < 7; I++)
		{
			iLineBits = BPP * Passw[I];
			oLineBits = BPP * Width;

			for (Y = 0; Y < Passh[I]; Y++)
			{
				for (X = 0; X < Passw[I]; X++)
				{
					iBP = (8 * PassStart[I]) + (Y * iLineBits + X * BPP);
					oBP = (ADAM7IY[I] + Y * ADAM7DY[I]) * oLineBits + (ADAM7IX[I] + X * ADAM7DX[I]) * BPP;

					for (B = 0; B < BPP; B++)
					{
						Bit = png_ReadBitFromReversedStream(&iBP, In);
						png_SetBitOfReversedStream(&oBP, Out, Bit);
					}
				}
			}
		}
	}
}

CAPI_SUBFUNC(U8) png_down_scale_16bit(U32 Color)
{
	return (U8)((255 * Color) / 65535);
}

CAPI_SUBFUNC(void) png_generate_color_table(PIXEL* pPalette, PIXEL_RGB* pPaletteRGB, U32 PaletteLen, U8* ptRNS, U32 tRNSLen)
{
	U32 I;

	if (PaletteLen > 256) PaletteLen = 256;

	for (I = 0; I < PaletteLen; I++)
	{
		pPalette[I].Red = pPaletteRGB[I].Red;
		pPalette[I].Green = pPaletteRGB[I].Green;
		pPalette[I].Blue = pPaletteRGB[I].Blue;

		if (I < tRNSLen) pPalette[I].Alpha = ptRNS[I];
		else pPalette[I].Alpha = 0xFF;
	}
}

CAPI_SUBFUNC(U8) png_lookup_8bit_alpha_from_table(U8 Color, U16* ptRNS, U32 tRNSLen)
{
	U32 i;

	for (i = 0; i < tRNSLen; i++)
	{
		if ((ptRNS[i] >> 8) == Color) return 0;
	}

	return 0xFF;
}

CAPI_SUBFUNC(U8) png_lookup_16bit_alpha_from_table(U16 Color, U16* ptRNS, U32 tRNSLen)
{
	U32 i;

	for (i = 0; i < tRNSLen; i++)
	{
		if (ptRNS[i] == Color) return 0;
	}

	return 0xFF;
}

CAPI_SUBFUNC(U8) png_lookup_24bit_alpha_from_table(PIXEL_RGB* Color, PIXEL_RGB16* ptRNS, U32 tRNSLen)
{
	U32 i;

	for (i = 0; i < tRNSLen; i++)
	{
		if (((ptRNS[i].Red >> 8) == Color->Red) && ((ptRNS[i].Green >> 8) == Color->Green) && ((ptRNS[i].Blue >> 8) == Color->Blue)) return 0;
	}

	return 0xFF;
}

CAPI_SUBFUNC(U8) png_lookup_48bit_alpha_from_table(PIXEL_RGB16* Color, PIXEL_RGB16* ptRNS, U32 tRNSLen)
{
	U32 i;

	for (i = 0; i < tRNSLen; i++)
	{
		if ((ptRNS[i].Red == Color->Red) && (ptRNS[i].Green == Color->Green) && (ptRNS[i].Blue == Color->Blue)) return 0;
	}

	return 0xFF;
}

CAPI_SUBFUNC(void) png_generate_grey_table(PIXEL* pPalette, U8 BPP, U16* ptRNS, U32 tRNSLen)
{
	U8 I, Color;

	I = Color = 0;

	switch (BPP)
	{
	case 1:
		pPalette[0].Red = 0;
		pPalette[0].Green = 0;
		pPalette[0].Blue = 0;
		pPalette[0].Alpha = png_lookup_8bit_alpha_from_table(0, ptRNS, tRNSLen);
		pPalette[1].Red = 0xFF;
		pPalette[1].Green = 0xFF;
		pPalette[1].Blue = 0xFF;
		pPalette[1].Alpha = png_lookup_8bit_alpha_from_table(1, ptRNS, tRNSLen);
		break;
	case 4:
		for (; I < 16; I++)
		{
			pPalette[I].Red = Color;
			pPalette[I].Green = Color;
			pPalette[I].Blue = Color;
			pPalette[I].Alpha = png_lookup_8bit_alpha_from_table(I, ptRNS, tRNSLen);
			Color += 17;
		}
		break;
	case 2:
		for (; I < 4; I++)
		{
			pPalette[I].Red = Color;
			pPalette[I].Green = Color;
			pPalette[I].Blue = Color;
			pPalette[I].Alpha = png_lookup_8bit_alpha_from_table(I, ptRNS, tRNSLen);
			Color += 85;
		}
		break;
	default:
		break;
	}
}

CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromMemory(PNG* pPngFile, size_t FileSize, U32* pHeight)
{
	U32 Width, ChunkSize;
	PNG_IHDR* pIhdr;

	if ((pPngFile == 0) || (FileSize == 0)) return 0;
	if (pPngFile->Signature != 0xA1A0A0D474E5089) return 0;

	ChunkSize = ToLittleEndian_32(&pPngFile->FirstChunk.Length);
	if (ChunkSize != sizeof(PNG_IHDR)) return 0;

	pIhdr = (PNG_IHDR*)((size_t)&pPngFile->FirstChunk + sizeof(PNG_CHUNK));

	Width = ToLittleEndian_32(&pIhdr->Width);
	if (pHeight != 0) *pHeight = ToLittleEndian_32(&pIhdr->Height);

	return Width;
}

CAPI_FUNC(U32) capi_Get_PNG_DimensionsFromFile(FILE* pPngFile, U32* pHeight)
{
	PNG ThisPng;
	PNG_IHDR PngIhdr;
	U32 ChunkSize, Width;
	U32 crc32chk;

	if (pPngFile == 0) return 0;

	if (fseek(pPngFile, 0, SEEK_SET) != 0) return 0;
	if (fread(&ThisPng, 1, sizeof(PNG), pPngFile) != sizeof(PNG)) return 0;

	if (ThisPng.Signature != 0xA1A0A0D474E5089) return 0;
	if (fread(&PngIhdr, 1, sizeof(PNG_IHDR), pPngFile) != sizeof(PNG_IHDR)) return 0;
	if (fread(&crc32chk, 1, 4, pPngFile) != 4) return 0;

	ChunkSize = ToLittleEndian_32(&ThisPng.FirstChunk.Length);
	if (ChunkSize != sizeof(PNG_IHDR)) return 0;

	crc32chk = ToLittleEndian_32(&crc32chk);
	if (crc32(crc32(0, ThisPng.FirstChunk.ChuckType, 4), (const Bytef*)&PngIhdr, sizeof(PNG_IHDR)) != crc32chk) return 0;

	Width = ToLittleEndian_32(&PngIhdr.Width);
	if (pHeight != 0) *pHeight = ToLittleEndian_32(&PngIhdr.Height);

	return Width;
}

CAPI_FUNC(I32) capi_Load_PNG_FromMemory(IMAGE* pImage, U32 Alignment, PNG* pPngFile, size_t FileSize)
{
	PNG_IHDR* pIhdr;
	PNG_CHUNK* NextChunk, * ThisChunk;
	I32 ErrorCode;
	U32 Width, Height, BPP, ChunkSize, PaletteLen, tRNSLen, SourceSize, zLibSize, Stride, i;
	U32 Passw[7], Passh[7], FilterPassStart[7], PaddedPassStart[7], PassStart[7];
	U8* IDATS, * UncompressedData, * pUnfilteredImage, Color, Alpha;
	uLong UncompressedSize;
	PIXEL* pDestination, pPalette[256];
	PIXEL_RGB* pPaletteRGB;
	void* pSource, * ptRNS;
	U16 Color16;

	if ((pImage == 0) || (pPngFile == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if (pPngFile->Signature != 0xA1A0A0D474E5089) return CAPI_ERROR_INVALID_FILE_FORMAT;

	pIhdr = (PNG_IHDR*)((size_t)&pPngFile->FirstChunk + sizeof(PNG_CHUNK));
	ChunkSize = ToLittleEndian_32(&pPngFile->FirstChunk.Length);

	if (ChunkSize != sizeof(PNG_IHDR)) return CAPI_ERROR_INVALID_FILE_FORMAT;
	if (crc32(0, pPngFile->FirstChunk.ChuckType, sizeof(PNG_IHDR) + 4) != ToLittleEndian_32((pIhdr + 1))) return CAPI_ERROR_INVALID_CRC32;

	if ((pIhdr->CompressionMethod != 0) || (pIhdr->FilterMethod != 0)) return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	if ((pIhdr->InterlaceMethod != 0) && (pIhdr->InterlaceMethod != 1)) return CAPI_ERROR_UNIMPLEMENTED_FEATURE;

	Width = ToLittleEndian_32(&pIhdr->Width);
	Height = ToLittleEndian_32(&pIhdr->Height);

	BPP = png_GetBitsPerPixel(pIhdr->ColourType, pIhdr->BitDepth);
	if (BPP == 0) return CAPI_ERROR_INVALID_FILE_FORMAT;

	zLibSize = png_GetCombinedIdatSize(&pPngFile->FirstChunk);

	IDATS = (U8*)capi_malloc(zLibSize);
	if (IDATS == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	zLibSize = tRNSLen = PaletteLen = 0;

	ptRNS = 0;
	pPaletteRGB = 0;

	ThisChunk = (PNG_CHUNK*)((size_t)&pPngFile->FirstChunk + ChunkSize + 12);

	do
	{
		ChunkSize = ToLittleEndian_32(&ThisChunk->Length);
		NextChunk = (PNG_CHUNK*)((size_t)ThisChunk + ChunkSize + 12);

		if (crc32(0, ThisChunk->ChuckType, ChunkSize + 4) != ToLittleEndian_32(((size_t)(ThisChunk + 1) + ChunkSize)))
		{
			capi_free(IDATS);
			return CAPI_ERROR_INVALID_CRC32;
		}

		if (png_IsChunk(ThisChunk->ChuckType, (U8*)"IDAT"))
		{
			capi_memcopy(&IDATS[zLibSize], (U8*)((size_t)ThisChunk + 8), ChunkSize);
			zLibSize += ChunkSize;
		}
		else if (png_IsChunk(ThisChunk->ChuckType, (U8*)"PLTE"))
		{
			pPaletteRGB = (PIXEL_RGB*)((size_t)ThisChunk + 8);
			PaletteLen = ChunkSize / 3;
		}
		else if (png_IsChunk(ThisChunk->ChuckType, (U8*)"tRNS"))
		{
			ptRNS = (U8*)((size_t)ThisChunk + 8);
			tRNSLen = ChunkSize;
		}

		ThisChunk = NextChunk;
	} while (!png_IsChunk(ThisChunk->ChuckType, (U8*)"IEND"));

	ErrorCode = z_inf_mem((void**)&UncompressedData, &UncompressedSize, IDATS, zLibSize);
	capi_free(IDATS);

	if (ErrorCode != Z_OK)
	{
		return CAPI_ERROR_ZLIB_FAILED;
	}

	if (BPP <= 32) SourceSize = (Width * Height) * 4;
	else SourceSize = (Width * (BPP / 8)) * Height;

	pUnfilteredImage = (U8*)capi_malloc(SourceSize);

	if (pUnfilteredImage == 0)
	{
		capi_free(UncompressedData);
		return CAPI_ERROR_OUT_OF_MEMORY;
	}

	switch (pIhdr->InterlaceMethod)
	{
	case 0:
		ErrorCode = png_decode_filter_type_0(pUnfilteredImage, UncompressedData, Width, Height, BPP);
		break;
	case 1:
	{
		png_Adam7_GetPassValues(Passw, Passh, FilterPassStart, PaddedPassStart, PassStart, Width, Height, BPP);
		for (i = 0; i < 7; i++)
		{
			ErrorCode = png_decode_filter_type_0(&UncompressedData[PaddedPassStart[i]], &UncompressedData[FilterPassStart[i]], Passw[i], Passh[i], BPP);
			if (ErrorCode != CAPI_ERROR_NONE) goto png_decoding_done;

			if (BPP < 8)
			{
				png_RemovePaddingBits(&UncompressedData[PassStart[i]], &UncompressedData[PaddedPassStart[i]], Passw[i] * BPP, ((Passw[i] * BPP + 7) / 8) * 8, Passh[i]);
			}
		}
		png_Adam7_Deinterlace(pUnfilteredImage, UncompressedData, Width, Height, BPP);
		break;
	}
	default:
		break;
	}

png_decoding_done:

	capi_free(UncompressedData);

	if (ErrorCode != CAPI_ERROR_NONE)
	{
		capi_free(pUnfilteredImage);
		return ErrorCode;
	}

	pDestination = capi_CreateImage(pImage, Width, Height, 0, Alignment);
	if (pDestination == 0)
	{
		capi_free(pUnfilteredImage);
		return CAPI_ERROR_OUT_OF_MEMORY;
	}

	pSource = pUnfilteredImage;
	Stride = ((((Width * BPP) + 7) & ~7) >> 3);

	switch (pIhdr->ColourType)
	{
	case 0: // Greyscale.
	{
		tRNSLen /= 2;
		png_generate_grey_table(pPalette, pIhdr->BitDepth, (U16*)ptRNS, tRNSLen);

		switch (pIhdr->BitDepth)
		{
		case 1:
			image_decode_1bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 2:
			image_decode_2bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 4:
			image_decode_4bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 8:
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					Color = ((U8*)pSource)[i];
					pDestination[i].Red = Color;
					pDestination[i].Green = Color;
					pDestination[i].Blue = Color;
					pDestination[i].Alpha = png_lookup_8bit_alpha_from_table(Color, (U16*)ptRNS, tRNSLen);
				}

				pSource = (U8*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
			break;
		}
		case 16:
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					Color16 = ((U16*)pSource)[i];
					pDestination[i].Alpha = png_lookup_16bit_alpha_from_table(Color16, (U16*)ptRNS, tRNSLen);
					Color16 = ToLittleEndian_16(&Color16);
					Color = png_down_scale_16bit(Color16);
					pDestination[i].Red = Color;
					pDestination[i].Green = Color;
					pDestination[i].Blue = Color;
				}

				pSource = (U16*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case 2: // Truecolor.
	{
		tRNSLen /= sizeof(PIXEL_RGB16);

		if (pIhdr->BitDepth == 8)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					pDestination[i].Red = ((PIXEL_RGB*)pSource)[i].Red;
					pDestination[i].Green = ((PIXEL_RGB*)pSource)[i].Green;
					pDestination[i].Blue = ((PIXEL_RGB*)pSource)[i].Blue;
					pDestination[i].Alpha = png_lookup_24bit_alpha_from_table(&((PIXEL_RGB*)pSource)[i], (PIXEL_RGB16*)ptRNS, tRNSLen);
				}

				pSource = (PIXEL_RGB*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		else if (pIhdr->BitDepth == 16)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					pDestination[i].Red = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGB16*)pSource)[i].Red));
					pDestination[i].Green = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGB16*)pSource)[i].Green));
					pDestination[i].Blue = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGB16*)pSource)[i].Blue));
					pDestination[i].Alpha = png_lookup_48bit_alpha_from_table(&((PIXEL_RGB16*)pSource)[i], (PIXEL_RGB16*)ptRNS, tRNSLen);
				}

				pSource = (PIXEL_RGB16*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		break;
	}
	case 3: // Indexed-colour (paletted)
	{
		png_generate_color_table(pPalette, pPaletteRGB, PaletteLen, (U8*)ptRNS, tRNSLen);

		switch (pIhdr->BitDepth)
		{
		case 1:
			image_decode_1bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 2:
			image_decode_2bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 4:
			image_decode_4bit_Stream(pImage, pUnfilteredImage, Stride, pPalette);
			break;
		case 8:
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					Color = ((U8*)pSource)[i];
					pDestination[i].Red = pPalette[Color].Red;
					pDestination[i].Green = pPalette[Color].Green;
					pDestination[i].Blue = pPalette[Color].Blue;
					pDestination[i].Alpha = pPalette[Color].Alpha;
				}

				pSource = (U8*)pSource + Stride;
				pDestination += pImage->ScanLine;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case 4: // Greyscale with alpha.
	{
		if (pIhdr->BitDepth == 8)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					Color = ((PIXEL_GREY*)pSource)[i].Color;
					Alpha = ((PIXEL_GREY*)pSource)[i].Alpha;
					pDestination[i].Red = Color;
					pDestination[i].Green = Color;
					pDestination[i].Blue = Color;
					pDestination[i].Alpha = Alpha;
				}

				pSource = (PIXEL_GREY*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		else if (pIhdr->BitDepth == 16)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					Color = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_GREY16*)pSource)[i].Color));
					Alpha = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_GREY16*)pSource)[i].Alpha));
					pDestination[i].Red = Color;
					pDestination[i].Green = Color;
					pDestination[i].Blue = Color;
					pDestination[i].Alpha = Alpha;
				}

				pSource = (PIXEL_GREY16*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		break;
	}
	case 6: // Truecolor with alpha.
	{
		if (pIhdr->BitDepth == 8)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					pDestination[i].Red = ((PIXEL_RGBA*)pSource)[i].Red;
					pDestination[i].Green = ((PIXEL_RGBA*)pSource)[i].Green;
					pDestination[i].Blue = ((PIXEL_RGBA*)pSource)[i].Blue;
					pDestination[i].Alpha = ((PIXEL_RGBA*)pSource)[i].Alpha;
				}

				pSource = (PIXEL_RGBA*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		else if (pIhdr->BitDepth == 16)
		{
			while (Height-- != 0)
			{
				for (i = 0; i < Width; i++)
				{
					pDestination[i].Red = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGBA16*)pSource)[i].Red));
					pDestination[i].Green = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGBA16*)pSource)[i].Green));
					pDestination[i].Blue = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGBA16*)pSource)[i].Blue));
					pDestination[i].Alpha = png_down_scale_16bit(ToLittleEndian_16(&((PIXEL_RGBA16*)pSource)[i].Alpha));
				}

				pSource = (PIXEL_RGBA16*)pSource + Width;
				pDestination += pImage->ScanLine;
			}
		}
		break;
	}
	default:
		break;
	}

	capi_free(pUnfilteredImage);

	return CAPI_ERROR_NONE;
}

CAPI_FUNC(I32) capi_Create_PNG_ToMemory(PNG** ppFilePointer, U64* pFileSize, IMAGE* pImage, PNG_PARAMETERS* pParameters)
{
	PNG* pPngFile;
	PNG_IHDR* pIhdr;
	PNG_CHUNK* ThisChunk;
	U32 Width, Height, PaletteLen, Size, BPP, ScanLine, Stride, I, X, Y, ChunkSize, ret;
	uLong zLen;
	U8 BitDepth, ColourType, * zOut;
	PIXEL* pSource, pPalette[256];
	PIXEL_RGB* pPaletteRGB;
	void* pData, * pDestination;
	z_crc_t crc32var;

	if ((ppFilePointer == 0) || (pFileSize == 0) || (pImage == 0)) return CAPI_ERROR_INVALID_PARAMETER;

	if ((pParameters->CompressionMethod != 0) || (pParameters->FilterMethod != 0)) return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	if (pParameters->InterlaceMethod > 1) return CAPI_ERROR_UNIMPLEMENTED_FEATURE;

	pSource = pImage->Pixels;
	if (pSource == 0) return CAPI_ERROR_INVALID_PARAMETER;

	ScanLine = pImage->ScanLine;

	Width = pImage->Width;
	Height = pImage->Height;

	if (image_get_transparency_level(pImage) > 1)
	{
		BPP = 32;
		PaletteLen = 0;
		ColourType = 6;
		BitDepth = 8;
	}
	else
	{
		PaletteLen = image_generate_palette_from_image(pPalette, pImage);
		if ((I32)PaletteLen == -1)
		{
			BPP = 24;
			PaletteLen = 0;
			ColourType = 2;
			BitDepth = 8;
		}
		else
		{
			if (PaletteLen <= 2) BPP = 1;
			else if (PaletteLen <= 4) BPP = 2;
			else if (PaletteLen <= 16) BPP = 4;
			else BPP = 8;

			ColourType = 3;
			BitDepth = (U8)BPP;
		}
	}

	Stride = ((((Width * BPP) + 7) & ~7) >> 3);
	if (pParameters->FilterMethod == 0) Stride += 1;

	if (pParameters->InterlaceMethod != 0)
	{
		return CAPI_ERROR_UNIMPLEMENTED_FEATURE;
	}

	Size = (U32)Stride * Height;
	pData = capi_malloc(Size);

	if (pData == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	switch (BPP)
	{
	case 1:
	{
		image_encode_1bit_Stream((U8*)pData, pImage, Stride, pPalette);
		break;
	}
	case 2:
	{
		image_encode_2bit_Stream((U8*)pData, pImage, Stride, pPalette);
		break;
	}
	case 4:
	{
		image_encode_4bit_Stream((U8*)pData, pImage, Stride, pPalette);
		break;
	}
	case 8:
	{
		Y = Height;
		pDestination = pData;

		while (Y-- != 0)
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
		Y = Height;
		pDestination = pData;

		while (Y-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				((PIXEL_RGB*)pDestination)[X].Red = pSource[X].Red;
				((PIXEL_RGB*)pDestination)[X].Green = pSource[X].Green;
				((PIXEL_RGB*)pDestination)[X].Blue = pSource[X].Blue;
			}

			pSource += ScanLine;
			pDestination = (void*)((size_t)pDestination + Stride);
		}
		break;
	}
	case 32:
	{
		Y = Height;
		pDestination = pData;

		while (Y-- != 0)
		{
			for (X = 0; X < Width; X++)
			{
				((PIXEL_RGBA*)pDestination)[X].Red = pSource[X].Red;
				((PIXEL_RGBA*)pDestination)[X].Green = pSource[X].Green;
				((PIXEL_RGBA*)pDestination)[X].Blue = pSource[X].Blue;
				((PIXEL_RGBA*)pDestination)[X].Alpha = pSource[X].Alpha;
			}

			pSource += ScanLine;
			pDestination = (void*)((size_t)pDestination + Stride);
		}
		break;
	}
	default:
		break;
	}

	if (pParameters->FilterMethod == 0) png_encode_filter_type_0((U8*)pData, Stride, Height);
	// if (InterlaceMethod == 1) TODO!!!

	ret = z_def_mem((void**)&zOut, &zLen, pData, Size, pParameters->Level);
	capi_free(pData);

	if (ret != Z_OK) return CAPI_ERROR_ZLIB_FAILED;

	// **                                                    ** //
	// ****  Here we calculate how much memory is needed!  **** //
	// **                                                    ** //

	Size = sizeof(PNG) + sizeof(PNG_IHDR) + 4; // PNG Header and the IHDR chunk
	Size += sizeof(PNG_CHUNK) + 4; // For the IEND chunk

	//Size += sizeof(PNG_CHUNK) + 1 + 4; // For the sRGB chunk

	if (PaletteLen != 0)
	{
		Size += sizeof(PNG_CHUNK) + (sizeof(PIXEL_RGB) * PaletteLen) + 4; // For the PLTE chunk
	}

	X = zLen / pParameters->IDAT_Size;

	Size += (sizeof(PNG_CHUNK) + pParameters->IDAT_Size + 4) * X;
	if ((zLen % pParameters->IDAT_Size) != 0)
	{
		Size += sizeof(PNG_CHUNK) + (zLen % pParameters->IDAT_Size) + 4;
		X++;
	}

	pPngFile = (PNG*)capi_malloc(Size);
	if (pPngFile == 0)
	{
		capi_free(zOut);
		return CAPI_ERROR_OUT_OF_MEMORY;
	}

	// **                                   ** //
	// ****  Here we setup all the data!  **** //
	// **                                   ** //

	pPngFile->Signature = 0xA1A0A0D474E5089;

	// **** Setup IHDR chunk **** //

	ChunkSize = sizeof(PNG_IHDR);
	pPngFile->FirstChunk.Length = ToBigEndian_32(&ChunkSize);
	pPngFile->FirstChunk.ChuckType[0] = 'I';
	pPngFile->FirstChunk.ChuckType[1] = 'H';
	pPngFile->FirstChunk.ChuckType[2] = 'D';
	pPngFile->FirstChunk.ChuckType[3] = 'R';

	pIhdr = (PNG_IHDR*)((size_t)&pPngFile->FirstChunk + sizeof(PNG_CHUNK));
	pIhdr->Width = ToBigEndian_32(&Width);
	pIhdr->Height = ToBigEndian_32(&Height);
	pIhdr->BitDepth = BitDepth;
	pIhdr->ColourType = ColourType;
	pIhdr->CompressionMethod = pParameters->CompressionMethod;
	pIhdr->FilterMethod = pParameters->FilterMethod;
	pIhdr->InterlaceMethod = pParameters->InterlaceMethod;
	crc32var = crc32(0, pPngFile->FirstChunk.ChuckType, sizeof(PNG_IHDR) + 4);
	*(U32*)(pIhdr + 1) = ToBigEndian_32(&crc32var);

	ThisChunk = (PNG_CHUNK*)((size_t)&pPngFile->FirstChunk + ChunkSize + 12);

	// **** Setup sRGB chunk **** //

	/*
	ChunkSize = 1;
	ThisChunk->Length = ToBigEndian_32(&ChunkSize);
	ThisChunk->ChuckType[0] = 's';
	ThisChunk->ChuckType[1] = 'R';
	ThisChunk->ChuckType[2] = 'G';
	ThisChunk->ChuckType[3] = 'B';

	*(U8*)((size_t)ThisChunk + 8) = 0;

	crc32var = crc32(0, ThisChunk->ChuckType, ChunkSize + 4);
	*(U32*)((size_t)(ThisChunk+1)+ChunkSize) = ToBigEndian_32(&crc32var);
	ThisChunk = (PNG_CHUNK*) ((size_t)ThisChunk + ChunkSize + 12);
	*/

	// **** Setup PLTE chunk if needed **** //

	if (PaletteLen != 0)
	{
		ChunkSize = sizeof(PIXEL_RGB) * PaletteLen;
		ThisChunk->Length = ToBigEndian_32(&ChunkSize);
		ThisChunk->ChuckType[0] = 'P';
		ThisChunk->ChuckType[1] = 'L';
		ThisChunk->ChuckType[2] = 'T';
		ThisChunk->ChuckType[3] = 'E';
		pPaletteRGB = (PIXEL_RGB*)((size_t)ThisChunk + 8);

		for (I = 0; I < PaletteLen; I++)
		{
			pPaletteRGB[I].Red = pPalette[I].Red;
			pPaletteRGB[I].Green = pPalette[I].Green;
			pPaletteRGB[I].Blue = pPalette[I].Blue;
		}

		crc32var = crc32(0, ThisChunk->ChuckType, ChunkSize + 4);
		*(U32*)((size_t)(ThisChunk + 1) + ChunkSize) = ToBigEndian_32(&crc32var);
		ThisChunk = (PNG_CHUNK*)((size_t)ThisChunk + ChunkSize + 12);
	}

	// **** Setup IDAT chunks **** //

	for (I = 0; I < X; I++)
	{
		ChunkSize = (zLen > pParameters->IDAT_Size) ? pParameters->IDAT_Size : zLen;
		ThisChunk->Length = ToBigEndian_32(&ChunkSize);
		ThisChunk->ChuckType[0] = 'I';
		ThisChunk->ChuckType[1] = 'D';
		ThisChunk->ChuckType[2] = 'A';
		ThisChunk->ChuckType[3] = 'T';

		capi_memcopy((U8*)((size_t)ThisChunk + 8), &zOut[I * pParameters->IDAT_Size], ChunkSize);
		zLen -= pParameters->IDAT_Size;

		crc32var = crc32(0, ThisChunk->ChuckType, ChunkSize + 4);
		*(U32*)((size_t)(ThisChunk + 1) + ChunkSize) = ToBigEndian_32(&crc32var);
		ThisChunk = (PNG_CHUNK*)((size_t)ThisChunk + ChunkSize + 12);
	}

	// **** Setup the last chunk "IEND" **** //

	ThisChunk->Length = 0;
	ThisChunk->ChuckType[0] = 'I';
	ThisChunk->ChuckType[1] = 'E';
	ThisChunk->ChuckType[2] = 'N';
	ThisChunk->ChuckType[3] = 'D';

	crc32var = crc32(0, ThisChunk->ChuckType, 4);
	*(U32*)(ThisChunk + 1) = ToBigEndian_32(&crc32var);

	// ** DONE! ** //

	capi_free(zOut);

	*ppFilePointer = pPngFile;
	*pFileSize = Size;

	return CAPI_ERROR_NONE;
}
