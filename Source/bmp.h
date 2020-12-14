
/******************************************************************************************
*
* Name: bmp.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares common BMP (bitmap) structures and functions.
*
*******************************************************************************************/

#ifndef BMP_H
#define BMP_H

#include "image.h"

/*
*
* These structures are used by the BMP format supported in this library
*
*/

PACK(STRUCT(BMPV2)
{
	U32 HeaderSize;
	U16 Width;
	U16 Height;
	U16 Planes;
	U16 BitsPerPixel;
});

PACK(STRUCT(BMPV3)
{
	U32 HeaderSize;
	I32 Width;
	I32 Height;
	U16 Planes;
	U16 BitsPerPixel;
	U32 Compression;
	U32 ImageSize;
	I32 HorResolution;
	I32 VerResolution;
	U32 ColorPalette;
	U32 ImportantColors;
});

PACK(STRUCT(BMPV4)
{
	U32 HeaderSize;
	I32 Width;
	I32 Height;
	U16 Planes;
	U16 BitsPerPixel;
	U32 Compression;
	U32 ImageSize;
	I32 HorResolution;
	I32 VerResolution;
	U32 ColorPalette;
	U32 ImportantColors;
	U32 RedMask;
	U32 GreenMask;
	U32 BlueMask;
	U32 AlphaMask;
	U32 CsType;
	I32 RedX;
	I32 RedY;
	I32 RedZ;
	I32 GreenX;
	I32 GreenY;
	I32 GreenZ;
	I32 BlueX;
	I32 BlueY;
	I32 BlueZ;
	U32 GammaRed;
	U32 GammaGreen;
	U32 GammaBlue;
});

PACK(STRUCT(BMPINFOHEADER)
{
	U32 HeaderSize;
	I32 Width;
	I32 Height;
	U16 Planes;
	U16 BitsPerPixel;
	U32 Compression;
	U32 ImageSize;
	I32 HorResolution;
	I32 VerResolution;
	U32 ColorPalette;
	U32 ImportantColors;
	U32 RedMask;
	U32 GreenMask;
	U32 BlueMask;
	U32 AlphaMask;
	U32 CsType;
	I32 RedX;
	I32 RedY;
	I32 RedZ;
	I32 GreenX;
	I32 GreenY;
	I32 GreenZ;
	I32 BlueX;
	I32 BlueY;
	I32 BlueZ;
	U32 GammaRed;
	U32 GammaGreen;
	U32 GammaBlue;
	U32 Intent;
	U32 ProfileData;
	U32 ProfileSize;
	U32 Reserved;
});

/*
*
* These helper functions are used by the BMP format supported in this library
*
*/

CAPI_SUBFUNC(U32) bmp_ctz(U32 Value);

CAPI_SUBFUNC(U8) bmp_color_from_mask(U32 Color, U32 Mask);

CAPI_SUBFUNC(void) bmp_encode_palette(PIXEL_BGR* pDesBGR, PIXEL_BGRA* pDesBGRA, PIXEL* pSrcPalette, U32 PaletteLen);

CAPI_SUBFUNC(void) bmp_decode_palette(PIXEL* pDesPalette, PIXEL_BGR* pSrcBGR, PIXEL_BGRA* pSrcBGRA, U32 PaletteLen);

CAPI_SUBFUNC(I32) bmp_decode_rle4(U8* In, U32 InLen, PIXEL* pPalette, PIXEL_BGRA* Out, U32 OutLen, U32 Width);

CAPI_SUBFUNC(I32) bmp_decode_rle8(U8* In, U32 InLen, PIXEL* pPalette, PIXEL_BGRA* Out, U32 OutLen, U32 Width);

#endif /* BMP_H */
