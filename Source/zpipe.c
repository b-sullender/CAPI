
/******************************************************************************************
*
* Name: zpipe.c
*
* Created by  Brian Sullender
*
* Description:
*  This file defines wrapper methods for zlib functions.
*
*******************************************************************************************/

#include "CAPI.h"

#define Z_CHUNK 0x4000 // 16 KB
#define Z_DEF_BUFFER_SIZE 0x8000 // 32 KB

uLong z_ReadFromMemory(unsigned char* dest, uLong size, uLong count, unsigned char** src, uLong* srcLen)
{
	uLong nBytesRead;

	nBytesRead = size * count;
	nBytesRead = (*srcLen < nBytesRead) ? *srcLen : nBytesRead;

	capi_memcopy(dest, *src, nBytesRead);

	*src += nBytesRead;
	*srcLen -= nBytesRead;

	return nBytesRead;
}

void* z_WriteToMemory(unsigned char* dest, uLong* desLen, uLong* desPos, uLong size, uLong count, unsigned char* src)
{
	unsigned char* newdest;
	uLong nBytesToWrite;

	nBytesToWrite = size * count;

	if (dest == 0)
	{
		*desPos = 0;
		*desLen = Z_DEF_BUFFER_SIZE;
		dest = (unsigned char*)capi_malloc(*desLen);
		if (dest == 0) return 0;
	}

	if (nBytesToWrite > (*desLen - *desPos))
	{
		*desLen += Z_DEF_BUFFER_SIZE;
		newdest = (unsigned char*)capi_malloc(*desLen);

		if (newdest == 0)
		{
			capi_free(dest);
			return 0;
		}

		capi_memcopy(newdest, dest, *desPos);
		capi_free(dest);

		dest = newdest;
	}

	capi_memcopy(&dest[*desPos], src, nBytesToWrite);
	*desPos += nBytesToWrite;

	return dest;
}

CAPI_FUNC(int) z_def_mem(void** dest, uLong* desLen, void* src, uLong srcLen, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[Z_CHUNK];
	unsigned char out[Z_CHUNK];
	uLong desPos;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = deflateInit2(&strm, level, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
	if (ret != Z_OK) return ret;

	*dest = 0;

	do
	{
		strm.avail_in = (uInt)z_ReadFromMemory(in, 1, Z_CHUNK, (unsigned char**)&src, &srcLen);

		flush = (strm.avail_in < Z_CHUNK) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		do
		{
			strm.avail_out = Z_CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);

			have = Z_CHUNK - strm.avail_out;

			*dest = z_WriteToMemory((unsigned char*)*dest, desLen, &desPos, 1, have, out);
			if (*dest == 0)
			{
				deflateEnd(&strm);
				return Z_MEM_ERROR;
			}
		} while (strm.avail_out == 0);
	} while (flush != Z_FINISH);

	*desLen = strm.total_out;
	deflateEnd(&strm);

	return Z_OK;
}

CAPI_FUNC(int) z_inf_mem(void** dest, uLong* desLen, void* src, uLong srcLen)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[Z_CHUNK];
	unsigned char out[Z_CHUNK];
	uLong desPos;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	ret = inflateInit(&strm);
	if (ret != Z_OK) return ret;

	*dest = 0;

	do
	{
		strm.avail_in = z_ReadFromMemory(in, 1, Z_CHUNK, (unsigned char**)&src, &srcLen);

		if (strm.avail_in == 0) break;
		strm.next_in = in;

		do
		{
			strm.avail_out = Z_CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);

			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				return ret;
			}

			have = Z_CHUNK - strm.avail_out;

			*dest = z_WriteToMemory((unsigned char*)*dest, desLen, &desPos, 1, have, out);
			if (*dest == 0)
			{
				inflateEnd(&strm);
				return Z_MEM_ERROR;
			}
		} while (strm.avail_out == 0);
	} while (ret != Z_STREAM_END);

	*desLen = strm.total_out;
	inflateEnd(&strm);

	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

CAPI_FUNC(int) z_def_file(FILE* dest, FILE* source, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[Z_CHUNK];
	unsigned char out[Z_CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = deflateInit(&strm, level);
	if (ret != Z_OK) return ret;

	do
	{
		strm.avail_in = fread(in, 1, Z_CHUNK, source);

		if (ferror(source))
		{
			deflateEnd(&strm);
			return Z_ERRNO;
		}

		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		do
		{
			strm.avail_out = Z_CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);
			have = Z_CHUNK - strm.avail_out;

			if (fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				deflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
	} while (flush != Z_FINISH);

	deflateEnd(&strm);

	return Z_OK;
}

CAPI_FUNC(int) z_inf_file(FILE* dest, FILE* source)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[Z_CHUNK];
	unsigned char out[Z_CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	ret = inflateInit(&strm);
	if (ret != Z_OK) return ret;

	do
	{
		strm.avail_in = fread(in, 1, Z_CHUNK, source);

		if (ferror(source))
		{
			inflateEnd(&strm);
			return Z_ERRNO;
		}

		if (strm.avail_in == 0) break;
		strm.next_in = in;

		do
		{
			strm.avail_out = Z_CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);

			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				return ret;
			}

			have = Z_CHUNK - strm.avail_out;

			if (fwrite(out, 1, have, dest) != have || ferror(dest))
			{
				inflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
	} while (ret != Z_STREAM_END);

	inflateEnd(&strm);

	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
