
/******************************************************************************************
*
* Name: jpg.h
*
* Created by  Brian Sullender
*
* Description:
*  This file declares common JPG (jpeg) structures and functions.
*
*******************************************************************************************/

#ifndef JPG_H
#define JPG_H

#include "image.h"

/*
*
* These structures are used by the JPG format supported in this library
*
*/

struct my_jpeg_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

/*
*
* These helper functions are used by the JPG format supported in this library
*
*/

METHODDEF(void) my_error_exit(j_common_ptr cinfo);

#endif /* JPG_H */
