
/******************************************************************************************
*
* Name: image.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares common image structures and functions.
*
*******************************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "CAPI.h"

/*
*
* These PIXEL structures are used by the image formats supported in this library
*
*/

PACK(1, STRUCT(PIXEL_RGB)
{
	U8 Red;
	U8 Green;
	U8 Blue;
});

PACK(1, STRUCT(PIXEL_RGBA)
{
	U8 Red;
	U8 Green;
	U8 Blue;
	U8 Alpha;
});

PACK(1, STRUCT(PIXEL_RGB16)
{
	U16 Red;
	U16 Green;
	U16 Blue;
});

PACK(1, STRUCT(PIXEL_RGBA16)
{
	U16 Red;
	U16 Green;
	U16 Blue;
	U16 Alpha;
});

PACK(1, STRUCT(PIXEL_BGR)
{
	U8 Blue;
	U8 Green;
	U8 Red;
});

PACK(1, STRUCT(PIXEL_BGRA)
{
	U8 Blue;
	U8 Green;
	U8 Red;
	U8 Alpha;
});

PACK(1, STRUCT(PIXEL_GREY)
{
	U8 Color;
	U8 Alpha;
});

PACK(1, STRUCT(PIXEL_GREY16)
{
	U16 Color;
	U16 Alpha;
});

/*
*
* These helper functions are used by the image formats supported in this library
*
*/

/*
*
image_find_color_in_palette - Find a color in a palette (image.c)
* pColor [The color to find]
* pPalette [Pointer to a PIXEL array that is the palette to search]
* Length [The number of items in the palette]
* returns a pointer to a color in a palette. 0 if not found
*
*/
CAPI_SUBFUNC(PIXEL*) image_find_color_in_palette(PIXEL* pColor, PIXEL* pPalette, U32 Length);

/*
*
image_generate_palette_from_image - Generate a palette from a image (image.c)
* pPalette [Pointer to a PIXEL array that has 256 items]
* pImage [The image used to fill the palette]
* returns the number of colors placed in the palette or -1 if there are more than 256 colors in the image
*
*/
CAPI_SUBFUNC(I32) image_generate_palette_from_image(PIXEL* pPalette, IMAGE* pImage);

/*
*
image_get_transparency_level - Gets the level of transparency (image.c)
* pImage [The image to get the transparency level]
* returns 0 for an error, 1 for ZERO transparency, 2 for FULL transparency, 3 for ZERO & FULL transparency, 4 for anything else
*
*/
CAPI_SUBFUNC(U8) image_get_transparency_level(IMAGE* pImage);

/*
*
image_encode_1bit_Stream - Encode a standard image to a 1-bit stream (image.c)
* pDestination [Pointer to a U8 array that will be the encoded stream]
* pImage [The source image to be encoded]
* Stride [The stride to use for the encoded stream]
* pPalette [The palette used to convert from the standard image to the encoded stream]
*
*/
CAPI_SUBFUNC(void) image_encode_1bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette);

/*
*
image_encode_2bit_Stream - Encode a standard image to a 2-bit stream (image.c)
* pDestination [Pointer to a U8 array that will be the encoded stream]
* pImage [The source image to be encoded]
* Stride [The stride to use for the encoded stream]
* pPalette [The palette used to convert from the standard image to the encoded stream]
*
*/
CAPI_SUBFUNC(void) image_encode_2bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette);

/*
*
image_encode_4bit_Stream - Encode a standard image to a 4-bit stream (image.c)
* pDestination [Pointer to a U8 array that will be the encoded stream]
* pImage [The source image to be encoded]
* Stride [The stride to use for the encoded stream]
* pPalette [The palette used to convert from the standard image to the encoded stream]
*
*/
CAPI_SUBFUNC(void) image_encode_4bit_Stream(U8* pDestination, IMAGE* pImage, size_t Stride, PIXEL* pPalette);

/*
*
image_decode_1bit_Stream - Decode a 1-bit stream to a standard image (image.c)
* pImage [The image to be filled with the stream]
* pSource [Pointer to a U8 array that is the stream to decode]
* Stride [The stride used by the stream]
* pPalette [The palette used to convert from the encoded stream to the standard image]
*
*/
CAPI_SUBFUNC(void) image_decode_1bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette);

/*
*
image_decode_2bit_Stream - Decode a 2-bit stream to a standard image (image.c)
* pImage [The image to be filled with the stream]
* pSource [Pointer to a U8 array that is the stream to decode]
* Stride [The stride used by the stream]
* pPalette [The palette used to convert from the encoded stream to the standard image]
*
*/
CAPI_SUBFUNC(void) image_decode_2bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette);

/*
*
image_decode_4bit_Stream - Decode a 4-bit stream to a standard image (image.c)
* pImage [The image to be filled with the stream]
* pSource [Pointer to a U8 array that is the stream to decode]
* Stride [The stride used by the stream]
* pPalette [The palette used to convert from the encoded stream to the standard image]
*
*/
CAPI_SUBFUNC(void) image_decode_4bit_Stream(IMAGE* pImage, U8* pSource, size_t Stride, PIXEL* pPalette);

/*
*
image_apply_icAND - Apply AND masks to a image pixels (image.c)
* pImage [The image to apply the masks to]
* p_icAND_mask [Pointer to a U8 array that is the AND masks]
* mask_Stride [The stride used by the AND mask array]
*
*/
CAPI_SUBFUNC(void) image_apply_icAND(IMAGE* pImage, U8* p_icAND_mask, size_t mask_Stride);

/*
*
image_test_adjust_alpha_pixels - Test a image transparency and adjust it (image.c)
* pImage [The image to be tested and adjusted]
* If (all) pixels are fully transparent then (all) pixels are changed to be fully opaque
*
*/
CAPI_SUBFUNC(void) image_test_adjust_alpha_pixels(IMAGE* pImage);

#endif /* IMAGE_H */
