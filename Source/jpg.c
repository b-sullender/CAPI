
/******************************************************************************************
*
* Name: jpg.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines common JPG (jpeg) functions.
*
*******************************************************************************************/

#include "jpg.h"

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	struct my_jpeg_error_mgr* myerr = (struct my_jpeg_error_mgr*)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromMemory(JPG* pJpgFile, size_t FileSize, U32* pHeight)
{
	U32 Width;

	struct jpeg_decompress_struct cinfo;
	struct my_jpeg_error_mgr jerr;

	if (pJpgFile == 0) return 0;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (U8*)pJpgFile, FileSize);

	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	Width = cinfo.output_width;
	if (pHeight != 0) *pHeight = cinfo.output_height;

	jpeg_destroy_decompress(&cinfo);

	return Width;
}

CAPI_FUNC(U32) capi_Get_JPG_DimensionsFromFile(FILE* pJpgFile, U32* pHeight)
{
	U32 Width;

	struct jpeg_decompress_struct cinfo;
	struct my_jpeg_error_mgr jerr;

	if (pJpgFile == 0) return 0;

	if (fseek(pJpgFile, 0, SEEK_SET) != 0) return 0;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pJpgFile);

	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return 0;
	jpeg_start_decompress(&cinfo);

	Width = cinfo.output_width;
	if (pHeight != 0) *pHeight = cinfo.output_height;

	jpeg_destroy_decompress(&cinfo);

	return Width;
}

CAPI_FUNC(I32) capi_Load_JPG_FromMemory(IMAGE* pImage, U32 Alignment, JPG* pJpgFile, size_t FileSize)
{
	I32 ErrorCode;
	U32 x, y, channels, data_size, i, ScanLine;
	U8* jdata, * rowptr[1];
	PIXEL* pDestination;
	void* pSource;

	struct jpeg_decompress_struct cinfo;
	struct my_jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		return CAPI_ERROR_JPEG_FAILED;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (U8*)pJpgFile, FileSize);

	if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) return CAPI_ERROR_JPEG_FAILED;

	channels = cinfo.num_components;
	if ((channels != 3) && (channels != 4)) return CAPI_ERROR_UNIMPLEMENTED_FEATURE;

	jpeg_start_decompress(&cinfo);

	x = cinfo.output_width;
	y = cinfo.output_height;

	data_size = (x * y) * 4;

	pDestination = capi_CreateImage(pImage, x, y, 0, Alignment);
	if (pDestination == 0)
	{
		jpeg_destroy_decompress(&cinfo);
		return CAPI_ERROR_OUT_OF_MEMORY;
	}

	jdata = (U8*)capi_malloc(data_size);
	if (jdata == 0)
	{
		capi_FreeImage(pImage);
		jpeg_destroy_decompress(&cinfo);
		return CAPI_ERROR_OUT_OF_MEMORY;
	}

	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		ErrorCode = CAPI_ERROR_IMAGE_DATA_INCOMPLETE;
		goto jpeg_fatal_error;
	}

	ErrorCode = CAPI_ERROR_NONE;

	while (cinfo.output_scanline < cinfo.output_height)
	{
		rowptr[0] = (U8*)jdata + channels * cinfo.output_width * cinfo.output_scanline;
		jpeg_read_scanlines(&cinfo, rowptr, 1);
	}

	jpeg_finish_decompress(&cinfo);

jpeg_fatal_error:
	jpeg_destroy_decompress(&cinfo);

	pSource = jdata;
	ScanLine = pImage->ScanLine;

	if (channels == 3)
	{
		while (y-- != 0)
		{
			for (i = 0; i < x; i++)
			{
				pDestination[i].Red = ((PIXEL_RGB*)pSource)[i].Red;
				pDestination[i].Green = ((PIXEL_RGB*)pSource)[i].Green;
				pDestination[i].Blue = ((PIXEL_RGB*)pSource)[i].Blue;
				pDestination[i].Alpha = 0xFF;
			}

			pDestination += ScanLine;
			pSource = (void*)((PIXEL_RGB*)pSource + x);
		}
	}
	else if (channels == 4)
	{
		while (y-- != 0)
		{
			for (i = 0; i < x; i++)
			{
				pDestination[i].Red = ((PIXEL_RGBA*)pSource)[i].Red;
				pDestination[i].Green = ((PIXEL_RGBA*)pSource)[i].Green;
				pDestination[i].Blue = ((PIXEL_RGBA*)pSource)[i].Blue;
				pDestination[i].Alpha = 0xFF;
			}

			pDestination += ScanLine;
			pSource = (void*)((PIXEL_RGBA*)pSource + x);
		}
	}

	capi_free(jdata);

	return ErrorCode;
}

CAPI_FUNC(I32) capi_Create_JPG_ToMemory(JPG** ppFilePointer, U64* pFileSize, IMAGE* pImage, U8 Quality)
{
	size_t Size;
	U32 Width, Height, ScanLine, I;
	JSAMPROW row_pointer[1];
	PIXEL_RGB* pImageBuffer;
	PIXEL* pSource;
	JPG* pJPG;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	if ((ppFilePointer == 0) || (pFileSize == 0) || (pImage == 0)) return CAPI_ERROR_INVALID_PARAMETER;
	if ((Quality == 0) || (Quality > 100)) return CAPI_ERROR_INVALID_PARAMETER;

	pSource = pImage->Pixels;
	if (pSource == 0) return CAPI_ERROR_INVALID_PARAMETER;

	ScanLine = pImage->ScanLine;

	Width = pImage->Width;
	Height = pImage->Height;

	pImageBuffer = (PIXEL_RGB*)capi_malloc(sizeof(PIXEL_RGB) * Width);
	if (pImageBuffer == 0) return CAPI_ERROR_OUT_OF_MEMORY;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	pJPG = 0;
	Size = 0;

	jpeg_mem_dest(&cinfo, (U8**)&pJPG, (size_t*)&Size);

	cinfo.image_width = Width;
	cinfo.image_height = Height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, Quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	row_pointer[0] = (JSAMPROW)pImageBuffer;

	while (cinfo.next_scanline < cinfo.image_height)
	{
		for (I = 0; I < Width; I++)
		{
			pImageBuffer[I].Red = pSource[I].Red;
			pImageBuffer[I].Green = pSource[I].Green;
			pImageBuffer[I].Blue = pSource[I].Blue;
		}
		pSource += ScanLine;

		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	capi_free(pImageBuffer);

	*ppFilePointer = pJPG;
	*pFileSize = Size;

	return CAPI_ERROR_NONE;
}
