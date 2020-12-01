
/******************************************************************************************
*
* Name: png.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares common PNG (Portable Network Graphics) structures and functions.
*
*******************************************************************************************/

#ifndef PNG_H
#define PNG_H

#include "image.h"

/*
*
* These structures are used by the PNG format supported in this library
*
*/

PACK(1, STRUCT(PNG_IHDR)
{
	U32 Width;
	U32 Height;
	U8 BitDepth;
	U8 ColourType;
	U8 CompressionMethod;
	U8 FilterMethod;
	U8 InterlaceMethod;
});

/*
*
* These helper functions are used by the PNG format supported in this library
*
*/

CAPI_SUBFUNC(I32) png_AbsL(I32 X);

CAPI_SUBFUNC(U32) png_get_dimensions(PNG* pPngFile, U32* pHeight);

CAPI_SUBFUNC(U32) png_GetBitsPerPixel(U32 ColorType, U32 BitDepth);

CAPI_SUBFUNC(U8) png_IsChunk(U8* Chunk1, U8* Chunk2);

CAPI_SUBFUNC(U32) png_GetCombinedIdatSize(PNG_CHUNK* FirstChunk);

CAPI_SUBFUNC(U8) png_PaethPredictor(U16 A, U16 B, U16 C);

CAPI_SUBFUNC(void) png_encode_filter_type_0(U8* pData, U32 Stride, U32 ScanLines);

CAPI_SUBFUNC(I32) png_decode_scanLine_filter_type_0(U8* Recon, U8* ScanLine, U8* Precon, U32 ByteWidth, U8 FilterType, U32 Length);

CAPI_SUBFUNC(I32) png_decode_filter_type_0(U8* Out, U8* In, U32 Width, U32 Height, U32 BPP);

CAPI_SUBFUNC(U8) png_ReadBitFromReversedStream(U32* BitPointer, U8* BitStream);

CAPI_SUBFUNC(void) png_SetBitOfReversedStream(U32* BitPointer, U8* BitStream, U8 Bit);

CAPI_SUBFUNC(void) png_RemovePaddingBits(U8* Out, U8* In, U32 oLineBits, U32 iLineBits, U32 Height);

CAPI_SUBFUNC(void) png_Adam7_GetPassValues(U32* Passw, U32* Passh, U32* FilterPassStart, U32* PaddedPassStart, U32* PassStart, U32 Width, U32 Height, U32 BPP);

CAPI_SUBFUNC(void) png_Adam7_Deinterlace(U8* Out, U8* In, U32 Width, U32 Height, U32 BPP);

CAPI_SUBFUNC(U8) png_down_scale_16bit(U32 Color);

CAPI_SUBFUNC(void) png_generate_color_table(PIXEL* pPalette, PIXEL_RGB* pPaletteRGB, U32 PaletteLen, U8* ptRNS, U32 tRNSLen);

CAPI_SUBFUNC(U8) png_lookup_8bit_alpha_from_table(U8 Color, U16* ptRNS, U32 tRNSLen);

CAPI_SUBFUNC(U8) png_lookup_16bit_alpha_from_table(U16 Color, U16* ptRNS, U32 tRNSLen);

CAPI_SUBFUNC(U8) png_lookup_24bit_alpha_from_table(PIXEL_RGB* Color, PIXEL_RGB16* ptRNS, U32 tRNSLen);

CAPI_SUBFUNC(U8) png_lookup_48bit_alpha_from_table(PIXEL_RGB16* Color, PIXEL_RGB16* ptRNS, U32 tRNSLen);

CAPI_SUBFUNC(void) png_generate_grey_table(PIXEL* pPalette, U8 BPP, U16* ptRNS, U32 tRNSLen);

#endif /* PNG_H */
