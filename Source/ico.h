
/******************************************************************************************
*
* Name: ico.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares common ICO (icon) structures and functions.
*
*******************************************************************************************/

#ifndef ICO_H
#define ICO_H

#include "image.h"
#include "bmp.h"
#include "png.h"

/*
*
* These structures are used by the ICO format supported in this library
*
*/

PACK(1, STRUCT(ICO_ENTRY)
{
	U8 Width;
	U8 Height;
	U8 nPaletteColors;
	U8 Reserved;
	U16 Planes;
	U16 BitsPerPixel;
	U32 ImageSize;
	U32 ImageOffset;
});

/*
*
* These helper functions are used by the ICO format supported in this library
*
*/

CAPI_SUBFUNC(U32) ico_get_embedded_image_dimensions_mem(void* pEmbeddedFile, U32* pHeight);

CAPI_SUBFUNC(U32) ico_get_embedded_image_dimensions_file(FILE* pIcoFile, U32 ImageOffset, U32* pHeight);

CAPI_SUBFUNC(void) ico_apply_icAND(U8* p_icAND_mask, IMAGE* pImage, size_t mask_Stride);

CAPI_SUBFUNC(I32) ico_Create_BMP_Image(BMPV3** ppFilePointer, U64* pFileSize, IMAGE* pImage);

#endif /* ICO_H */
