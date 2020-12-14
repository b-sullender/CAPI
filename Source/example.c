
/******************************************************************************************
*
* Name: example.c
*
* Created by  Brian Sullender
*
* Description:
*  Test and demonstrate the CAPI module.
*
*******************************************************************************************/

#include "example.h"

#ifdef __GNUC__
STRUCT(THREAD_WINDOW_INTERNAL)
{
    int ExitMessageHandler;
};
#endif

#ifdef _MSC_VER
static U32 is_WindowClass_registered = FALSE;
STRUCT(THREAD_WINDOW_INTERNAL)
{
    HINSTANCE hInstance;
    HWND hWnd;
    HDC BufferHDC;
    BITMAPINFO* pDisplayBitmap;
    HBITMAP hBitmap;
};
#endif

STRUCT(THREAD_WINDOW)
{
    U32 SlotUsed;
    THREAD_WINDOW_INTERNAL sys_internals;
    int ClientWidth;
    int ClientHeight;
    IMAGE FrameBuffer;
    U32 App_Command;
    U32 TestImageIndex;    // The index of our test image
    U8 StretchAlgorithm;   // The algorithm to use to stretch our test image
    U32 NewImageWidth;     // The new width to stretch our test image to
    U32 NewImageHeight;    // The new height to stretch out test image to
};

static THREAD_WINDOW app_windows[MAX_THREAD_WINDOWS];

static int is_BMP_images_loaded = FALSE;
static int is_JPG_images_loaded = FALSE;
static int is_PNG_images_loaded = FALSE;
static int is_ICO_images_loaded = FALSE;
static int is_STRETCH_images_loaded = FALSE;

STRUCT(TEST_IMAGE_DATA)
{
    STRING FilePath[PATH_BUFFER_SIZE];
    STRING Description[IMAGE_DESCRIPTION_SIZE];
    IMAGE TestImage;
};

static TEST_IMAGE_DATA BMP_TestImages[64];
static TEST_IMAGE_DATA JPG_TestImages[6];
static TEST_IMAGE_DATA PNG_TestImages[59];
static TEST_IMAGE_DATA ICO_TestImages[11];
static TEST_IMAGE_DATA STRETCH_TestImages[2];

void TestApp_OutMessage(const STRING* Msg, BOOL NewLine)
{
    app_printf(Msg);
    if (NewLine == TRUE) app_printf(STR("\n\r"));
    fflush(stdout);
}

THREAD_WINDOW* TestApp_GetOpenThreadWindowSlot()
{
    U32 I;

    for (I = 0; I < MAX_THREAD_WINDOWS; I++)
    {
        if (app_windows[I].SlotUsed == FALSE) return &app_windows[I];
    }

    return 0;
}

void* LoadFile(const STRING* FilePath, U64* pFileLength)
{
    FILE* Stream;
    size_t BufferLength;
    void* pThisFile;
    void* pNewBlock;

    if ((FilePath == 0) || (pFileLength == 0)) return 0;

    *pFileLength = 0;

    Stream = str_fopen(FilePath, STR("rb"));
    if (Stream == 0) return 0;

    pThisFile = 0;
    BufferLength = 0;

    do
    {
        BufferLength += 0x1000;

        pNewBlock = capi_realloc(pThisFile, BufferLength);
        if (pNewBlock == 0)
        {
            if (pThisFile != 0) capi_free(pThisFile);
            fclose(Stream);
            return 0;
        }

        pThisFile = pNewBlock;

        *pFileLength += fread(&((U8*)pThisFile)[*pFileLength], 1, 0x1000, Stream);
    }
    while (!(feof(Stream)));

    fclose(Stream);

    return pThisFile;
}

I32 SaveFile(const STRING* FilePath, void* pFilePointer, U64 FileLength)
{
    FILE* Stream;
    size_t nBytesWrite;
    I32 ErrorCode;

    if ((FilePath == 0) || (pFilePointer == 0) || (FileLength == 0)) return CAPI_ERROR_INVALID_PARAMETER;

    ErrorCode = CAPI_ERROR_NONE;

    Stream = str_fopen(FilePath, STR("w+b"));
    if (Stream == 0) return CAPI_ERROR_ACCESS_DENIED;

    nBytesWrite = fwrite(pFilePointer, 1, (size_t)FileLength, Stream);
    if (nBytesWrite != FileLength)
    {
        ErrorCode = CAPI_ERROR_ACCESS_DENIED;
        goto exit_func;
    }

exit_func:
    fclose(Stream);

    return ErrorCode;
}

void SetupFilePath(STRING* pFilePath, const STRING* pDirectory, const STRING* pFile)
{
    str_copy_s(pFilePath, PATH_BUFFER_SIZE, pDirectory);
    str_append_s(pFilePath, PATH_BUFFER_SIZE, pFile);
}

void Load_TestImage(TEST_IMAGE_DATA* TestImage, BOOL FailTest)
{
    void* pFileData;
    U64 FileLength;
    I32 ErrorCode;

    pFileData = LoadFile(TestImage->FilePath, &FileLength);
    if (pFileData == 0)
    {
        TestApp_OutMessage(STR("ERROR: LoadFile failed to load "), FALSE);
        TestApp_OutMessage(TestImage->FilePath, TRUE);
        return;
    }

    if (FailTest == FALSE)
    {
        ErrorCode = capi_LoadImageFromMemory(&TestImage->TestImage, IMAGE_DEFAULT_ALIGNMENT, pFileData, FileLength);
        if (ErrorCode != CAPI_ERROR_NONE)
        {
            TestApp_OutMessage(STR("ERROR: capi_LoadImageFromMemory failed to load "), FALSE);
            TestApp_OutMessage(TestImage->FilePath, TRUE);
        }
    }
    else
    {
        ErrorCode = capi_LoadImageFromMemory(&TestImage->TestImage, IMAGE_DEFAULT_ALIGNMENT, pFileData, FileLength);
        if (ErrorCode == CAPI_ERROR_NONE)
        {
            TestApp_OutMessage(STR("ERROR: capi_LoadImageFromMemory had a FALSE success with "), FALSE);
            TestApp_OutMessage(TestImage->FilePath, TRUE);
        }
    }

    capi_free(pFileData);
}

void Load_BMP_TestImages(const STRING* pDirectory)
{
    if (is_BMP_images_loaded == TRUE)
    {
        TestApp_OutMessage(STR("Warning: Load_BMP_TestImages already executed. Skipping this step!"), TRUE);
        return;
    }

    // 1-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[0].FilePath, pDirectory, STR("/bmpTestSuite/g/pal1.bmp"));
    Load_TestImage(&BMP_TestImages[0], FALSE);
    SetupFilePath(BMP_TestImages[1].FilePath, pDirectory, STR("/bmpTestSuite/g/pal1wb.bmp"));
    Load_TestImage(&BMP_TestImages[1], FALSE);
    SetupFilePath(BMP_TestImages[2].FilePath, pDirectory, STR("/bmpTestSuite/g/pal1bg.bmp"));
    Load_TestImage(&BMP_TestImages[2], FALSE);

    // Questionable formatted 1-Bit and 2-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[3].FilePath, pDirectory, STR("/bmpTestSuite/q/pal1p1.bmp"));
    Load_TestImage(&BMP_TestImages[3], FALSE);
    SetupFilePath(BMP_TestImages[4].FilePath, pDirectory, STR("/bmpTestSuite/q/pal2.bmp"));
    Load_TestImage(&BMP_TestImages[4], FALSE);
    SetupFilePath(BMP_TestImages[5].FilePath, pDirectory, STR("/bmpTestSuite/q/pal2color.bmp"));
    Load_TestImage(&BMP_TestImages[5], FALSE);

    // 4-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[6].FilePath, pDirectory, STR("/bmpTestSuite/g/pal4.bmp"));
    Load_TestImage(&BMP_TestImages[6], FALSE);
    SetupFilePath(BMP_TestImages[7].FilePath, pDirectory, STR("/bmpTestSuite/g/pal4gs.bmp"));
    Load_TestImage(&BMP_TestImages[7], FALSE);
    SetupFilePath(BMP_TestImages[8].FilePath, pDirectory, STR("/bmpTestSuite/g/pal4rle.bmp"));
    Load_TestImage(&BMP_TestImages[8], FALSE);

    // Questionable formatted 4-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[9].FilePath, pDirectory, STR("/bmpTestSuite/q/pal4rletrns.bmp"));
    Load_TestImage(&BMP_TestImages[9], FALSE);
    SetupFilePath(BMP_TestImages[10].FilePath, pDirectory, STR("/bmpTestSuite/q/pal4rlecut.bmp"));
    Load_TestImage(&BMP_TestImages[10], FALSE);

    // 8-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[11].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8.bmp"));
    Load_TestImage(&BMP_TestImages[11], FALSE);
    SetupFilePath(BMP_TestImages[12].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8-0.bmp"));
    Load_TestImage(&BMP_TestImages[12], FALSE);
    SetupFilePath(BMP_TestImages[13].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8gs.bmp"));
    Load_TestImage(&BMP_TestImages[13], FALSE);
    SetupFilePath(BMP_TestImages[14].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8rle.bmp"));
    Load_TestImage(&BMP_TestImages[14], FALSE);
    SetupFilePath(BMP_TestImages[15].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8topdown.bmp"));
    Load_TestImage(&BMP_TestImages[15], FALSE);
    SetupFilePath(BMP_TestImages[16].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8os2.bmp"));
    Load_TestImage(&BMP_TestImages[16], FALSE);
    SetupFilePath(BMP_TestImages[17].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8v4.bmp"));
    Load_TestImage(&BMP_TestImages[17], FALSE);
    SetupFilePath(BMP_TestImages[18].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8v5.bmp"));
    Load_TestImage(&BMP_TestImages[18], FALSE);

    // Questionable formatted 8-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[19].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8rletrns.bmp"));
    Load_TestImage(&BMP_TestImages[19], FALSE);
    SetupFilePath(BMP_TestImages[20].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8rlecut.bmp"));
    Load_TestImage(&BMP_TestImages[20], FALSE);
    SetupFilePath(BMP_TestImages[21].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8offs.bmp"));
    Load_TestImage(&BMP_TestImages[21], FALSE);
    SetupFilePath(BMP_TestImages[22].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8oversizepal.bmp"));
    Load_TestImage(&BMP_TestImages[22], FALSE);
    SetupFilePath(BMP_TestImages[23].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2-sz.bmp"));
    Load_TestImage(&BMP_TestImages[23], FALSE);
    SetupFilePath(BMP_TestImages[24].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2-hs.bmp"));
    Load_TestImage(&BMP_TestImages[24], FALSE);
    SetupFilePath(BMP_TestImages[25].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2sp.bmp"));
    Load_TestImage(&BMP_TestImages[25], FALSE);
    SetupFilePath(BMP_TestImages[26].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2v2.bmp"));
    Load_TestImage(&BMP_TestImages[26], FALSE);
    SetupFilePath(BMP_TestImages[27].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2v2-16.bmp"));
    Load_TestImage(&BMP_TestImages[27], FALSE);
    SetupFilePath(BMP_TestImages[28].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2v2-sz.bmp"));
    Load_TestImage(&BMP_TestImages[28], FALSE);
    SetupFilePath(BMP_TestImages[29].FilePath, pDirectory, STR("/bmpTestSuite/q/pal8os2v2-40sz.bmp"));
    Load_TestImage(&BMP_TestImages[29], FALSE);

    // 16-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[30].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb16.bmp"));
    Load_TestImage(&BMP_TestImages[30], FALSE);
    SetupFilePath(BMP_TestImages[31].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb16bfdef.bmp"));
    Load_TestImage(&BMP_TestImages[31], FALSE);
    SetupFilePath(BMP_TestImages[32].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb16-565.bmp"));
    Load_TestImage(&BMP_TestImages[32], FALSE);
    SetupFilePath(BMP_TestImages[33].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb16-565pal.bmp"));
    Load_TestImage(&BMP_TestImages[33], FALSE);

    // Questionable formatted 16-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[34].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb16faketrns.bmp"));
    Load_TestImage(&BMP_TestImages[34], FALSE);
    SetupFilePath(BMP_TestImages[35].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb16-231.bmp"));
    Load_TestImage(&BMP_TestImages[35], FALSE);
    SetupFilePath(BMP_TestImages[36].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb16-3103.bmp"));
    Load_TestImage(&BMP_TestImages[36], FALSE);
    SetupFilePath(BMP_TestImages[37].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba16-4444.bmp"));
    Load_TestImage(&BMP_TestImages[37], FALSE);
    SetupFilePath(BMP_TestImages[38].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba16-5551.bmp"));
    Load_TestImage(&BMP_TestImages[38], FALSE);
    SetupFilePath(BMP_TestImages[39].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba16-1924.bmp"));
    Load_TestImage(&BMP_TestImages[39], FALSE);

    // 24-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[40].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb24.bmp"));
    Load_TestImage(&BMP_TestImages[40], FALSE);
    SetupFilePath(BMP_TestImages[41].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb24pal.bmp"));
    Load_TestImage(&BMP_TestImages[41], FALSE);

    // Questionable formatted 24-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[42].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb24largepal.bmp"));
    Load_TestImage(&BMP_TestImages[42], FALSE);
    SetupFilePath(BMP_TestImages[43].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb24prof.bmp"));
    Load_TestImage(&BMP_TestImages[43], FALSE);
    SetupFilePath(BMP_TestImages[44].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb24prof2.bmp"));
    Load_TestImage(&BMP_TestImages[44], FALSE);
    SetupFilePath(BMP_TestImages[45].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb24lprof.bmp"));
    Load_TestImage(&BMP_TestImages[45], FALSE);

    // 32-Bit Images expected to always work

    SetupFilePath(BMP_TestImages[46].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb32.bmp"));
    Load_TestImage(&BMP_TestImages[46], FALSE);
    SetupFilePath(BMP_TestImages[47].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb32bfdef.bmp"));
    Load_TestImage(&BMP_TestImages[47], FALSE);
    SetupFilePath(BMP_TestImages[48].FilePath, pDirectory, STR("/bmpTestSuite/g/rgb32bf.bmp"));
    Load_TestImage(&BMP_TestImages[48], FALSE);

    // Questionable formatted 32-Bit images that CAPI expects to work

    SetupFilePath(BMP_TestImages[49].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb32h52.bmp"));
    Load_TestImage(&BMP_TestImages[49], FALSE);
    SetupFilePath(BMP_TestImages[50].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb32-xbgr.bmp"));
    Load_TestImage(&BMP_TestImages[50], FALSE);
    SetupFilePath(BMP_TestImages[51].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb32fakealpha.bmp"));
    Load_TestImage(&BMP_TestImages[51], FALSE);
    SetupFilePath(BMP_TestImages[52].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb32-111110.bmp"));
    Load_TestImage(&BMP_TestImages[52], FALSE);
    SetupFilePath(BMP_TestImages[53].FilePath, pDirectory, STR("/bmpTestSuite/q/rgb32-7187.bmp"));
    Load_TestImage(&BMP_TestImages[53], FALSE);
    SetupFilePath(BMP_TestImages[54].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32-1.bmp"));
    Load_TestImage(&BMP_TestImages[54], FALSE);
    SetupFilePath(BMP_TestImages[55].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32-2.bmp"));
    Load_TestImage(&BMP_TestImages[55], FALSE);
    SetupFilePath(BMP_TestImages[56].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32-1010102.bmp"));
    Load_TestImage(&BMP_TestImages[56], FALSE);
    SetupFilePath(BMP_TestImages[57].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32-81284.bmp"));
    Load_TestImage(&BMP_TestImages[57], FALSE);
    SetupFilePath(BMP_TestImages[58].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32-61754.bmp"));
    Load_TestImage(&BMP_TestImages[58], FALSE);
    SetupFilePath(BMP_TestImages[59].FilePath, pDirectory, STR("/bmpTestSuite/q/rgba32h56.bmp"));
    Load_TestImage(&BMP_TestImages[59], FALSE);

    // Images expected to always load that are different sizes

    SetupFilePath(BMP_TestImages[60].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8nonsquare.bmp"));
    Load_TestImage(&BMP_TestImages[60], FALSE);
    SetupFilePath(BMP_TestImages[61].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8w126.bmp"));
    Load_TestImage(&BMP_TestImages[61], FALSE);
    SetupFilePath(BMP_TestImages[62].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8w125.bmp"));
    Load_TestImage(&BMP_TestImages[62], FALSE);
    SetupFilePath(BMP_TestImages[63].FilePath, pDirectory, STR("/bmpTestSuite/g/pal8w124.bmp"));
    Load_TestImage(&BMP_TestImages[63], FALSE);

    is_BMP_images_loaded = TRUE;
}

void Load_JPG_TestImages(const STRING* pDirectory)
{
    if (is_JPG_images_loaded == TRUE)
    {
        TestApp_OutMessage(STR("Warning: Load_JPG_TestImages already executed. Skipping this step!"), TRUE);
        return;
    }

    SetupFilePath(JPG_TestImages[0].FilePath, pDirectory, STR("/jpgTestSuite/jpg100quality.jpg"));
    Load_TestImage(&JPG_TestImages[0], FALSE);
    SetupFilePath(JPG_TestImages[1].FilePath, pDirectory, STR("/jpgTestSuite/jpg85quality.jpg"));
    Load_TestImage(&JPG_TestImages[1], FALSE);
    SetupFilePath(JPG_TestImages[2].FilePath, pDirectory, STR("/jpgTestSuite/jpg75quality.jpg"));
    Load_TestImage(&JPG_TestImages[2], FALSE);
    SetupFilePath(JPG_TestImages[3].FilePath, pDirectory, STR("/jpgTestSuite/jpg50quality.jpg"));
    Load_TestImage(&JPG_TestImages[3], FALSE);
    SetupFilePath(JPG_TestImages[4].FilePath, pDirectory, STR("/jpgTestSuite/jpg25quality.jpg"));
    Load_TestImage(&JPG_TestImages[4], FALSE);
    SetupFilePath(JPG_TestImages[5].FilePath, pDirectory, STR("/jpgTestSuite/jpg15quality.jpg"));
    Load_TestImage(&JPG_TestImages[5], FALSE);

    is_JPG_images_loaded = TRUE;
}

void Load_PNG_TestImages(const STRING* pDirectory)
{
    TEST_IMAGE_DATA FailTestImage;

    if (is_PNG_images_loaded == TRUE)
    {
        TestApp_OutMessage(STR("Warning: Load_PNG_TestImages already executed. Skipping this step!"), TRUE);
        return;
    }

    // Basic formats

    SetupFilePath(PNG_TestImages[0].FilePath, pDirectory, STR("/pngTestSuite/basn0g01.png"));
    Load_TestImage(&PNG_TestImages[0], FALSE); // 1-bit Black & White
    SetupFilePath(PNG_TestImages[1].FilePath, pDirectory, STR("/pngTestSuite/basn0g02.png"));
    Load_TestImage(&PNG_TestImages[1], FALSE); // 2-bit Grayscale
    SetupFilePath(PNG_TestImages[2].FilePath, pDirectory, STR("/pngTestSuite/basn0g04.png"));
    Load_TestImage(&PNG_TestImages[2], FALSE); // 4-bit Grayscale
    SetupFilePath(PNG_TestImages[3].FilePath, pDirectory, STR("/pngTestSuite/basn0g08.png"));
    Load_TestImage(&PNG_TestImages[3], FALSE); // 8-bit Grayscale
    SetupFilePath(PNG_TestImages[4].FilePath, pDirectory, STR("/pngTestSuite/basn0g16.png"));
    Load_TestImage(&PNG_TestImages[4], FALSE); // 16-bit Grayscale

    SetupFilePath(PNG_TestImages[5].FilePath, pDirectory, STR("/pngTestSuite/basn2c08.png"));
    Load_TestImage(&PNG_TestImages[5], FALSE); // 3x8 bits RGB color
    SetupFilePath(PNG_TestImages[6].FilePath, pDirectory, STR("/pngTestSuite/basn2c16.png"));
    Load_TestImage(&PNG_TestImages[6], FALSE); // 3x16 bits RGB color

    SetupFilePath(PNG_TestImages[7].FilePath, pDirectory, STR("/pngTestSuite/basn3p01.png"));
    Load_TestImage(&PNG_TestImages[7], FALSE); // 1-bit Paletted
    SetupFilePath(PNG_TestImages[8].FilePath, pDirectory, STR("/pngTestSuite/basn3p02.png"));
    Load_TestImage(&PNG_TestImages[8], FALSE); // 2-bit Paletted
    SetupFilePath(PNG_TestImages[9].FilePath, pDirectory, STR("/pngTestSuite/basn3p04.png"));
    Load_TestImage(&PNG_TestImages[9], FALSE); // 4-bit Paletted
    SetupFilePath(PNG_TestImages[10].FilePath, pDirectory, STR("/pngTestSuite/basn3p08.png"));
    Load_TestImage(&PNG_TestImages[10], FALSE); // 8-bit Paletted

    SetupFilePath(PNG_TestImages[11].FilePath, pDirectory, STR("/pngTestSuite/basn4a08.png"));
    Load_TestImage(&PNG_TestImages[11], FALSE); // 8-bit Grayscale + 8-bit Alpha
    SetupFilePath(PNG_TestImages[12].FilePath, pDirectory, STR("/pngTestSuite/basn4a16.png"));
    Load_TestImage(&PNG_TestImages[12], FALSE); // 16-bit Grayscale + 16-bit Alpha

    SetupFilePath(PNG_TestImages[13].FilePath, pDirectory, STR("/pngTestSuite/basn6a08.png"));
    Load_TestImage(&PNG_TestImages[13], FALSE); // 3x8 bits RGB color + 8-bit Alpha
    SetupFilePath(PNG_TestImages[14].FilePath, pDirectory, STR("/pngTestSuite/basn6a16.png"));
    Load_TestImage(&PNG_TestImages[14], FALSE); // 3x16 bits RGB color + 16-bit Alpha

    // Basic formats (Interlacing)

    SetupFilePath(PNG_TestImages[15].FilePath, pDirectory, STR("/pngTestSuite/basi0g01.png"));
    Load_TestImage(&PNG_TestImages[15], FALSE); // 1-bit Black & White
    SetupFilePath(PNG_TestImages[16].FilePath, pDirectory, STR("/pngTestSuite/basi0g02.png"));
    Load_TestImage(&PNG_TestImages[16], FALSE); // 2-bit Grayscale
    SetupFilePath(PNG_TestImages[17].FilePath, pDirectory, STR("/pngTestSuite/basi0g04.png"));
    Load_TestImage(&PNG_TestImages[17], FALSE); // 4-bit Grayscale
    SetupFilePath(PNG_TestImages[18].FilePath, pDirectory, STR("/pngTestSuite/basi0g08.png"));
    Load_TestImage(&PNG_TestImages[18], FALSE); // 8-bit Grayscale
    SetupFilePath(PNG_TestImages[19].FilePath, pDirectory, STR("/pngTestSuite/basi0g16.png"));
    Load_TestImage(&PNG_TestImages[19], FALSE); // 16-bit Grayscale

    SetupFilePath(PNG_TestImages[20].FilePath, pDirectory, STR("/pngTestSuite/basi2c08.png"));
    Load_TestImage(&PNG_TestImages[20], FALSE); // 3x8 bits RGB color
    SetupFilePath(PNG_TestImages[21].FilePath, pDirectory, STR("/pngTestSuite/basi2c16.png"));
    Load_TestImage(&PNG_TestImages[21], FALSE); // 3x16 bits RGB color

    SetupFilePath(PNG_TestImages[22].FilePath, pDirectory, STR("/pngTestSuite/basi3p01.png"));
    Load_TestImage(&PNG_TestImages[22], FALSE); // 1-bit Paletted
    SetupFilePath(PNG_TestImages[23].FilePath, pDirectory, STR("/pngTestSuite/basi3p02.png"));
    Load_TestImage(&PNG_TestImages[23], FALSE); // 2-bit Paletted
    SetupFilePath(PNG_TestImages[24].FilePath, pDirectory, STR("/pngTestSuite/basi3p04.png"));
    Load_TestImage(&PNG_TestImages[24], FALSE); // 4-bit Paletted
    SetupFilePath(PNG_TestImages[25].FilePath, pDirectory, STR("/pngTestSuite/basi3p08.png"));
    Load_TestImage(&PNG_TestImages[25], FALSE); // 8-bit Paletted

    SetupFilePath(PNG_TestImages[26].FilePath, pDirectory, STR("/pngTestSuite/basi4a08.png"));
    Load_TestImage(&PNG_TestImages[26], FALSE); // 8-bit Grayscale + 8-bit Alpha
    SetupFilePath(PNG_TestImages[27].FilePath, pDirectory, STR("/pngTestSuite/basi4a16.png"));
    Load_TestImage(&PNG_TestImages[27], FALSE); // 16-bit Grayscale + 16-bit Alpha

    SetupFilePath(PNG_TestImages[28].FilePath, pDirectory, STR("/pngTestSuite/basi6a08.png"));
    Load_TestImage(&PNG_TestImages[28], FALSE); // 3x8 bits RGB color + 8-bit Alpha
    SetupFilePath(PNG_TestImages[29].FilePath, pDirectory, STR("/pngTestSuite/basi6a16.png"));
    Load_TestImage(&PNG_TestImages[29], FALSE); // 3x16 bits RGB color + 16-bit Alpha

    // Image filtering

    SetupFilePath(PNG_TestImages[30].FilePath, pDirectory, STR("/pngTestSuite/f00n0g08.png"));
    Load_TestImage(&PNG_TestImages[30], FALSE); // Grayscale, filter-type 0
    SetupFilePath(PNG_TestImages[31].FilePath, pDirectory, STR("/pngTestSuite/f00n2c08.png"));
    Load_TestImage(&PNG_TestImages[31], FALSE); // Color, filter-type 0
    SetupFilePath(PNG_TestImages[32].FilePath, pDirectory, STR("/pngTestSuite/f01n0g08.png"));
    Load_TestImage(&PNG_TestImages[32], FALSE); // Grayscale, filter-type 1
    SetupFilePath(PNG_TestImages[33].FilePath, pDirectory, STR("/pngTestSuite/f01n2c08.png"));
    Load_TestImage(&PNG_TestImages[33], FALSE); // Color, filter-type 1
    SetupFilePath(PNG_TestImages[34].FilePath, pDirectory, STR("/pngTestSuite/f02n0g08.png"));
    Load_TestImage(&PNG_TestImages[34], FALSE); // Grayscale, filter-type 2
    SetupFilePath(PNG_TestImages[35].FilePath, pDirectory, STR("/pngTestSuite/f02n2c08.png"));
    Load_TestImage(&PNG_TestImages[35], FALSE); // Color, filter-type 2
    SetupFilePath(PNG_TestImages[36].FilePath, pDirectory, STR("/pngTestSuite/f03n0g08.png"));
    Load_TestImage(&PNG_TestImages[36], FALSE); // Grayscale, filter-type 3
    SetupFilePath(PNG_TestImages[37].FilePath, pDirectory, STR("/pngTestSuite/f03n2c08.png"));
    Load_TestImage(&PNG_TestImages[37], FALSE); // Color, filter-type 3
    SetupFilePath(PNG_TestImages[38].FilePath, pDirectory, STR("/pngTestSuite/f04n0g08.png"));
    Load_TestImage(&PNG_TestImages[38], FALSE); // Grayscale, filter-type 4
    SetupFilePath(PNG_TestImages[39].FilePath, pDirectory, STR("/pngTestSuite/f04n2c08.png"));
    Load_TestImage(&PNG_TestImages[39], FALSE); // Color, filter-type 4

    SetupFilePath(PNG_TestImages[40].FilePath, pDirectory, STR("/pngTestSuite/f99n0g04.png"));
    Load_TestImage(&PNG_TestImages[40], FALSE); // Bit-depth 4, filter changing per scanline

    // Transparency

    SetupFilePath(PNG_TestImages[41].FilePath, pDirectory, STR("/pngTestSuite/tbbn0g04.png"));
    Load_TestImage(&PNG_TestImages[41], FALSE); // Transparent, black background
    SetupFilePath(PNG_TestImages[42].FilePath, pDirectory, STR("/pngTestSuite/tbbn2c16.png"));
    Load_TestImage(&PNG_TestImages[42], FALSE); // Transparent, blue background
    SetupFilePath(PNG_TestImages[43].FilePath, pDirectory, STR("/pngTestSuite/tbbn3p08.png"));
    Load_TestImage(&PNG_TestImages[43], FALSE); // Transparent, black background
    SetupFilePath(PNG_TestImages[44].FilePath, pDirectory, STR("/pngTestSuite/tbgn2c16.png"));
    Load_TestImage(&PNG_TestImages[44], FALSE); // Transparent, green background
    SetupFilePath(PNG_TestImages[45].FilePath, pDirectory, STR("/pngTestSuite/tbgn3p08.png"));
    Load_TestImage(&PNG_TestImages[45], FALSE); // Transparent, light-gray background
    SetupFilePath(PNG_TestImages[46].FilePath, pDirectory, STR("/pngTestSuite/tbrn2c08.png"));
    Load_TestImage(&PNG_TestImages[46], FALSE); // Transparent, red background
    SetupFilePath(PNG_TestImages[47].FilePath, pDirectory, STR("/pngTestSuite/tbwn0g16.png"));
    Load_TestImage(&PNG_TestImages[47], FALSE); // Transparent, white background
    SetupFilePath(PNG_TestImages[48].FilePath, pDirectory, STR("/pngTestSuite/tbwn3p08.png"));
    Load_TestImage(&PNG_TestImages[48], FALSE); // Transparent, white background
    SetupFilePath(PNG_TestImages[49].FilePath, pDirectory, STR("/pngTestSuite/tbyn3p08.png"));
    Load_TestImage(&PNG_TestImages[49], FALSE); // Transparent, yellow background
    SetupFilePath(PNG_TestImages[50].FilePath, pDirectory, STR("/pngTestSuite/tp0n0g08.png"));
    Load_TestImage(&PNG_TestImages[50], FALSE); // Not transparent (logo on gray)
    SetupFilePath(PNG_TestImages[51].FilePath, pDirectory, STR("/pngTestSuite/tp0n2c08.png"));
    Load_TestImage(&PNG_TestImages[51], FALSE); // Not transparent (logo on gray)
    SetupFilePath(PNG_TestImages[52].FilePath, pDirectory, STR("/pngTestSuite/tp0n3p08.png"));
    Load_TestImage(&PNG_TestImages[52], FALSE); // Not transparent (logo on gray)
    SetupFilePath(PNG_TestImages[53].FilePath, pDirectory, STR("/pngTestSuite/tp1n3p08.png"));
    Load_TestImage(&PNG_TestImages[53], FALSE); // Transparent, no background
    SetupFilePath(PNG_TestImages[54].FilePath, pDirectory, STR("/pngTestSuite/tm3n3p02.png"));
    Load_TestImage(&PNG_TestImages[54], FALSE); // Multiple levels of transparency 3 entries

    // Zlib compression

    SetupFilePath(PNG_TestImages[55].FilePath, pDirectory, STR("/pngTestSuite/z00n2c08.png"));
    Load_TestImage(&PNG_TestImages[55], FALSE); // Color, compression level 0 (none)
    SetupFilePath(PNG_TestImages[56].FilePath, pDirectory, STR("/pngTestSuite/z03n2c08.png"));
    Load_TestImage(&PNG_TestImages[56], FALSE); // Color, compression level 3
    SetupFilePath(PNG_TestImages[57].FilePath, pDirectory, STR("/pngTestSuite/z06n2c08.png"));
    Load_TestImage(&PNG_TestImages[57], FALSE); // Color, compression level 6 (default)
    SetupFilePath(PNG_TestImages[58].FilePath, pDirectory, STR("/pngTestSuite/z09n2c08.png"));
    Load_TestImage(&PNG_TestImages[58], FALSE); // Color, compression level 9 (maximum)

    // Corrupted files

    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xs1n0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // Signature byte 1 MSBit reset to zero
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xs2n0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // Signature byte 2 is a Q''
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xs4n0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // Signature byte 4 lowercase
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xs7n0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // 7th byte a space instead of control-Z
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xcrn0g04.png"));
    Load_TestImage(&FailTestImage, TRUE); // Added cr bytes
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xlfn0g04.png"));
    Load_TestImage(&FailTestImage, TRUE); // Added if bytes
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xhdn0g08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Incorrect IHDR checksum
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xc1n0g08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Color type 1
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xc9n2c08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Color type 9
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xd0n2c08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Bit-depth 0
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xd3n2c08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Bit-depth 3
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xd9n2c08.png"));
    Load_TestImage(&FailTestImage, TRUE); // Bit-depth 99
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xdtn0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // Missing IDAT chunck
    SetupFilePath(FailTestImage.FilePath, pDirectory, STR("/pngTestSuite/xcsn0g01.png"));
    Load_TestImage(&FailTestImage, TRUE); // Incorrect IDAT checksum

    is_PNG_images_loaded = TRUE;
}

void Load_ICO_TestImages(const STRING* pDirectory)
{
    if (is_ICO_images_loaded == TRUE)
    {
        TestApp_OutMessage(STR("Warning: Load_ICO_TestImages already executed. Skipping this step!"), TRUE);
        return;
    }

    SetupFilePath(ICO_TestImages[0].FilePath, pDirectory, STR("/icoTestSuite/bmpicotest32bpp.ico"));
    Load_TestImage(&ICO_TestImages[0], FALSE);

    SetupFilePath(ICO_TestImages[1].FilePath, pDirectory, STR("/icoTestSuite/bmpicotest24bpp.ico"));
    Load_TestImage(&ICO_TestImages[1], FALSE);

    SetupFilePath(ICO_TestImages[2].FilePath, pDirectory, STR("/icoTestSuite/bmpicotest8bpp.ico"));
    Load_TestImage(&ICO_TestImages[2], FALSE);

    SetupFilePath(ICO_TestImages[3].FilePath, pDirectory, STR("/icoTestSuite/bmpicotest4bpp.ico"));
    Load_TestImage(&ICO_TestImages[3], FALSE);

    SetupFilePath(ICO_TestImages[4].FilePath, pDirectory, STR("/icoTestSuite/bmpaicotest32bpp.ico"));
    Load_TestImage(&ICO_TestImages[4], FALSE);

    SetupFilePath(ICO_TestImages[5].FilePath, pDirectory, STR("/icoTestSuite/bmpaicotest24bpp.ico"));
    Load_TestImage(&ICO_TestImages[5], FALSE);

    SetupFilePath(ICO_TestImages[6].FilePath, pDirectory, STR("/icoTestSuite/bmpaicotest8bpp.ico"));
    Load_TestImage(&ICO_TestImages[6], FALSE);

    SetupFilePath(ICO_TestImages[7].FilePath, pDirectory, STR("/icoTestSuite/bmpaicotest4bpp.ico"));
    Load_TestImage(&ICO_TestImages[7], FALSE);

    SetupFilePath(ICO_TestImages[8].FilePath, pDirectory, STR("/icoTestSuite/bmpaicotest1bpp.ico"));
    Load_TestImage(&ICO_TestImages[8], FALSE);

    SetupFilePath(ICO_TestImages[9].FilePath, pDirectory, STR("/icoTestSuite/pngicotest1.ico"));
    Load_TestImage(&ICO_TestImages[9], FALSE);

    SetupFilePath(ICO_TestImages[10].FilePath, pDirectory, STR("/icoTestSuite/pngicotest2.ico"));
    Load_TestImage(&ICO_TestImages[10], FALSE);

    is_ICO_images_loaded = TRUE;
}

void Load_STRETCH_TestImages(const STRING* pDirectory)
{
    if (is_STRETCH_images_loaded == TRUE)
    {
        return;
    }

    SetupFilePath(STRETCH_TestImages[0].FilePath, pDirectory, STR("/stretchTest/stretchTestSmall.png"));
    Load_TestImage(&STRETCH_TestImages[0], FALSE);

    SetupFilePath(STRETCH_TestImages[1].FilePath, pDirectory, STR("/stretchTest/stretchTestBig.jpg"));
    Load_TestImage(&STRETCH_TestImages[1], FALSE);

    is_STRETCH_images_loaded = TRUE;
}

void Draw_BMP_TestImages(IMAGE* pDisplay, U8 Alpha)
{
    U32 I, X, Y;

    I = 0;

    X = 15;
    Y = 15;

    // 1-Bit and 2-Bit images

    for (; I < 6; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 4-Bit images

    for (; I < 11; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 8-Bit images (set 1)

    for (; I < 19; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 8-Bit images (set 2)

    for (; I < 30; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 16-Bit images

    for (; I < 40; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 32-Bit images (set 1)

    for (; I < 49; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // 32-Bit images (set 2)

    for (; I < 60; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }

    X = 15;
    Y += 15 + 64;

    // Different sizes

    for (; I < 64; I++)
    {
        capi_DrawImageA(pDisplay, &BMP_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }
}

void Draw_JPG_TestImages(IMAGE* pDisplay, U8 Alpha)
{
    U32 I, X, Y;

    I = 0;

    X = 15;
    Y = 15;

    for (; I < 6; I++)
    {
        capi_DrawImageA(pDisplay, &JPG_TestImages[I].TestImage, X, Y, Alpha);
        X += 8 + 127;
    }
}

void Draw_PNG_TestImages(IMAGE* pDisplay, U8 Alpha)
{
    U32 I, X, Y;

    I = 0;

    X = 15;
    Y = 15;

    for (; I < 15; I++) // Basic formats
    {
        capi_DrawImageA(pDisplay, &PNG_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 32;
    }

    X += 15 + 32;
    Y = 15;

    for (; I < 30; I++) // Basic formats (Interlacing)
    {
        capi_DrawImageA(pDisplay, &PNG_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 32;
    }

    X += 15 + 32;
    Y = 15;

    for (; I < 41; I++) // Image filtering
    {
        capi_DrawImageA(pDisplay, &PNG_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 32;
    }

    X += 15 + 32;
    Y = 15;

    for (; I < 55; I++) // Transparency
    {
        capi_DrawImageA(pDisplay, &PNG_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 32;
    }

    X += 15 + 32;
    Y = 15;

    for (; I < 59; I++) // Zlib compression
    {
        capi_DrawImageA(pDisplay, &PNG_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 32;
    }
}

void Draw_ICO_TestImages(IMAGE* pDisplay, U8 Alpha)
{
    U32 I, X, Y;

    I = 0;

    X = 15;
    Y = 15;

    for (; I < 9; I++)
    {
        capi_DrawImageA(pDisplay, &ICO_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 64;
    }

    X += 15 + 64;
    Y = 15;

    for (; I < 11; I++)
    {
        capi_DrawImageA(pDisplay, &ICO_TestImages[I].TestImage, X, Y, Alpha);
        Y += 8 + 256;
    }
}

void Draw_STRETCH_TestImages(THREAD_WINDOW* thisWindow, IMAGE* pDisplay, U8 Alpha)
{
    U32 X, Y;

    X = 15;
    Y = 15;

    if (thisWindow->StretchAlgorithm != 255)
    {
        capi_DrawImageEx(pDisplay, &STRETCH_TestImages[thisWindow->TestImageIndex].TestImage, X, Y, thisWindow->StretchAlgorithm, thisWindow->NewImageWidth, thisWindow->NewImageHeight);
    }
    else
    {
        capi_DrawImageEx(pDisplay, &STRETCH_TestImages[thisWindow->TestImageIndex].TestImage, X, Y, DRAW_RESIZE_LINEAR, thisWindow->NewImageWidth, thisWindow->NewImageHeight);
        Y += 8 + thisWindow->NewImageHeight;
        capi_DrawImageEx(pDisplay, &STRETCH_TestImages[thisWindow->TestImageIndex].TestImage, X, Y, DRAW_RESIZE_BOXSAMP, thisWindow->NewImageWidth, thisWindow->NewImageHeight);
    }
}

void Draw_RECTS_LINES(IMAGE* pDisplay)
{
    U32 X, Y;

    CRECT BorderLine;

    BorderLine.left = 0;
    BorderLine.top = 0;
    BorderLine.right = pDisplay->Width - 1;
    BorderLine.bottom = pDisplay->Height - 1;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(255, 0, 0, 255), 2, 0);

    // Outlines only

    X = 15;
    Y = 15;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 1, 0);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 2, 0);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 3, 0);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 4, 0);

    // Outlines with fill

    X = 15;
    Y = 15 + 45 + 15;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 1, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 2, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 3, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 4, COLOR(0, 255, 0, 255));

    // Fill only

    X = 15;
    Y = 15 + 45 + 15 + 45 + 15;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, 0, 1, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, 0, 2, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, 0, 3, COLOR(0, 255, 0, 255));

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawRect(pDisplay, &BorderLine, 0, 4, COLOR(0, 255, 0, 255));

    // Draw lines NW to SE (RED)

    X = 30 + (8 * 3) + (45 * 4);
    Y = 15;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(255, 0, 0, 255), 1);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(255, 0, 0, 255), 2);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(255, 0, 0, 255), 3);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(255, 0, 0, 255), 4);

    // Draw lines NE to SW (BLUE)

    X = 30 + (8 * 3) + (45 * 5);
    Y = 30 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left - 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 1);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left - 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 2);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left - 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 3);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left - 45;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 255, 255), 4);

    // Draw lines W to E (GREEN)

    X = 30 + (8 * 3) + (45 * 4);
    Y = 30 + 15 + (45 * 2) + 23;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 255, 0, 255), 1);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 255, 0, 255), 2);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 255, 0, 255), 3);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left + 45;
    BorderLine.bottom = BorderLine.top;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 255, 0, 255), 4);

    // Draw lines N to S (BLACK)

    X = 30 + (8 * 3) + (45 * 4) + 23;
    Y = 30 + (15 * 2) + (45 * 3);

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 0, 255), 1);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 0, 255), 2);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 0, 255), 3);

    X += 8 + 45;

    BorderLine.left = X;
    BorderLine.top = Y;
    BorderLine.right = BorderLine.left;
    BorderLine.bottom = BorderLine.top + 45;
    capi_DrawLine(pDisplay, &BorderLine, COLOR(0, 0, 0, 255), 4);
}

void Window_Paint_Handler(THREAD_WINDOW* thisWindow, IMAGE* pDisplay)
{
    capi_FillImage(pDisplay, COLOR(195, 195, 195, 255));

    if (thisWindow->App_Command == APP_COMMAND_BMP)
    {
        Draw_BMP_TestImages(pDisplay, 255);
    }
    else if (thisWindow->App_Command == APP_COMMAND_JPG)
    {
        Draw_JPG_TestImages(pDisplay, 255);
    }
    else if (thisWindow->App_Command == APP_COMMAND_PNG)
    {
        Draw_PNG_TestImages(pDisplay, 255);
    }
    else if (thisWindow->App_Command == APP_COMMAND_ICO)
    {
        Draw_ICO_TestImages(pDisplay, 255);
    }
    else if (thisWindow->App_Command == APP_COMMAND_STRETCH)
    {
        Draw_STRETCH_TestImages(thisWindow, pDisplay, 255);
    }
    else if (thisWindow->App_Command == APP_COMMAND_DRAW)
    {
        Draw_RECTS_LINES(pDisplay);
    }
}

I32 ReadResolution(STRING* input, U32* pWidth, U32* pHeight)
{
    U32 I, charCnt;
    STRING strBuffer[64];

    // read Width:

    charCnt = 0;

    for (I = 0; charCnt < 64; I++)
    {
        if (input[I] == 0) return FALSE;
        if ((charCnt == 0) && (input[I] == ' ')) continue;
        if (input[I] == ' ') break;
        if (input[I] == 'x') break;
        if (input[I] == 'X') break;
        if ((input[I] < 0x30) || (input[I] > 0x39)) return FALSE;
        strBuffer[charCnt] = input[I];
        charCnt++;
    }

    strBuffer[charCnt] = 0;
    app_sscanf_s(strBuffer, STR("%u"), pWidth);

    // read Height

    charCnt = 0;

    for (I++; charCnt < 64; I++)
    {
        if (input[I] == 0) break;
        if ((charCnt == 0) && (input[I] == ' ')) continue;
        if (input[I] == ' ') break;
        if ((input[I] < 0x30) || (input[I] > 0x39)) return FALSE;
        strBuffer[charCnt] = input[I];
        charCnt++;
    }

    strBuffer[charCnt] = 0;
    app_sscanf_s(strBuffer, STR("%u"), pHeight);

    return TRUE;
}

U32 ReadParameter(STRING* input, U32 startIndex, STRING* pParameterBuffer, U32 MaxChars)
{
    U32 I, charCnt;

    charCnt = 0;

    for (I = startIndex; charCnt < MaxChars; I++)
    {
        if (input[I] == 0) break;
        if ((charCnt == 0) && (input[I] == ' ')) continue;
        if (input[I] == ' ') break;
        if (input[I] == '\r') continue;
        if (input[I] == '\n') continue;
        pParameterBuffer[charCnt] = input[I];
        charCnt++;
    }

    pParameterBuffer[charCnt] = 0;

    return I;
}

THREAD_WINDOW* TestApp_EnterInputLoop()
{
    THREAD_WINDOW* thisWindow;
    STRING input[128];
    STRING parameter[64];
    U32 Index;

    while (TRUE)
    {
        input[0] = 0;
        if (app_fgets(input, 127, stdin))
        {
            Index = ReadParameter(input, 0, parameter, 64);

            if (str_cmp(parameter, STR("help")) == 0)
            {
                TestApp_OutMessage(STR("bmp - Display BMP test images"), TRUE);
                TestApp_OutMessage(STR("jpg - Display JPG test images"), TRUE);
                TestApp_OutMessage(STR("png - Display PNG test images"), TRUE);
                TestApp_OutMessage(STR("ico - Display ICO test images"), TRUE);
                TestApp_OutMessage(STR("stretch - Stretch and display test image [Parameters]: image algorithm size"), TRUE);
                TestApp_OutMessage(STR("draw - Test drawing functions"), TRUE);
                TestApp_OutMessage(STR("test - Run programmer defined test code"), TRUE);
                TestApp_OutMessage(STR("cls - Clear console"), TRUE);
                TestApp_OutMessage(STR("exit - Exit the test program"), TRUE);
                TestApp_OutMessage(STR("SEE the HELP.txt file for more details"), TRUE);
            }
            else if (str_cmp(parameter, STR("test")) == 0)
            {
                // Your test code here!
            }
            else if (str_cmp(parameter, STR("bmp")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_BMP;

                thisWindow->ClientWidth = TESTAPP_WINDOW_BMP_WIDTH;
                thisWindow->ClientHeight = TESTAPP_WINDOW_BMP_HEIGHT;
                Load_BMP_TestImages(IMAGES_SOURCE_PATH);

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("jpg")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_JPG;

                thisWindow->ClientWidth = TESTAPP_WINDOW_JPG_WIDTH;
                thisWindow->ClientHeight = TESTAPP_WINDOW_JPG_HEIGHT;
                Load_JPG_TestImages(IMAGES_SOURCE_PATH);

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("png")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_PNG;

                thisWindow->ClientWidth = TESTAPP_WINDOW_PNG_WIDTH;
                thisWindow->ClientHeight = TESTAPP_WINDOW_PNG_HEIGHT;
                Load_PNG_TestImages(IMAGES_SOURCE_PATH);

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("ico")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_ICO;

                thisWindow->ClientWidth = TESTAPP_WINDOW_ICO_WIDTH;
                thisWindow->ClientHeight = TESTAPP_WINDOW_ICO_HEIGHT;
                Load_ICO_TestImages(IMAGES_SOURCE_PATH);

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("stretch")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                Index = ReadParameter(input, Index, parameter, 64);
                if (parameter[0] == 0)
                {
                    TestApp_OutMessage(STR("Missging parameters: image algorithm size"), TRUE);
                    continue;
                }

                if (str_cmp(parameter, STR("small")) == 0)
                {
                    thisWindow->TestImageIndex = 0;
                }
                else if (str_cmp(parameter, STR("big")) == 0)
                {
                    thisWindow->TestImageIndex = 1;
                }
                else
                {
                    TestApp_OutMessage(STR("Invalid parameter: "), FALSE);
                    TestApp_OutMessage(parameter, TRUE);
                    continue;
                }

                Index = ReadParameter(input, Index, parameter, 64);
                if (parameter[0] == 0)
                {
                    TestApp_OutMessage(STR("Missging parameters: algorithm size"), TRUE);
                    continue;
                }

                if (str_cmp(parameter, STR("all")) == 0)
                {
                    thisWindow->StretchAlgorithm = 255;
                }
                else if (str_cmp(parameter, STR("linear")) == 0)
                {
                    thisWindow->StretchAlgorithm = DRAW_RESIZE_LINEAR;
                }
                else if (str_cmp(parameter, STR("boxsamp")) == 0)
                {
                    thisWindow->StretchAlgorithm = DRAW_RESIZE_BOXSAMP;
                }
                else
                {
                    TestApp_OutMessage(STR("Invalid parameter: "), FALSE);
                    TestApp_OutMessage(parameter, TRUE);
                    continue;
                }

                Index = ReadParameter(input, Index, parameter, 64);
                if (parameter[0] == 0)
                {
                    TestApp_OutMessage(STR("Missging parameter: size"), TRUE);
                    continue;
                }

                if (!ReadResolution(parameter, &thisWindow->NewImageWidth, &thisWindow->NewImageHeight))
                {
                    TestApp_OutMessage(STR("Invalid parameter: "), FALSE);
                    TestApp_OutMessage(parameter, TRUE);
                    continue;
                }

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_STRETCH;

                if (thisWindow->StretchAlgorithm == 255) // all
                {
                    thisWindow->ClientWidth = 30 + thisWindow->NewImageWidth;
                    thisWindow->ClientHeight = 30 + (thisWindow->NewImageHeight * 2) + 8;
                }
                else
                {
                    thisWindow->ClientWidth = 30 + thisWindow->NewImageWidth;
                    thisWindow->ClientHeight = 30 + thisWindow->NewImageHeight;
                }
                Load_STRETCH_TestImages(IMAGES_SOURCE_PATH);

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("draw")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                capi_memset(thisWindow, 0, sizeof(THREAD_WINDOW));

                thisWindow->SlotUsed = TRUE;
                thisWindow->App_Command = APP_COMMAND_DRAW;

                thisWindow->ClientWidth = TESTAPP_WINDOW_DRAW_WIDTH;
                thisWindow->ClientHeight = TESTAPP_WINDOW_DRAW_HEIGHT;

                return thisWindow;
            }
            else if (str_cmp(parameter, STR("cls")) == 0)
            {
                ConsoleClear();
            }
            else if (str_cmp(parameter, STR("exit")) == 0)
            {
                thisWindow = TestApp_GetOpenThreadWindowSlot();
                thisWindow->App_Command = APP_COMMAND_EXIT;
                return thisWindow;
            }
            else
            {
                TestApp_OutMessage(STR("Invalid command, use help for command list"), TRUE);
            }
        }
    }
}

#ifdef __GNUC__

void* TestApp_ThreadWindowWorker(THREAD_WINDOW* thisWindow)
{
    Display* dpy;
    XVisualInfo vinfo;
    XSetWindowAttributes attrs;
    Window parent;
    Visual* visual;
    Window win;
    XImage* ximage;
    XEvent event;
    XGCValues gcv;
    GC NormalGC;
    int depth, snum, ScreenWidth, ScreenHeight;

    dpy = XOpenDisplay(NULL);
    if (dpy == NULL)
    {
        TestApp_OutMessage(STR("ERROR: Cannot open display."), TRUE);
        return 0;
    }

    if (!XMatchVisualInfo(dpy, XDefaultScreen(dpy), 24, TrueColor, &vinfo))
    {
        TestApp_OutMessage(STR("ERROR: XMatchVisualInfo Failed."), TRUE);
        return 0;
    }

    parent = XDefaultRootWindow(dpy);

    XSync(dpy, True);

    visual = vinfo.visual;
    depth = vinfo.depth;

    attrs.colormap = XCreateColormap(dpy, XDefaultRootWindow(dpy), visual, AllocNone);
    attrs.background_pixel = 0;
    attrs.border_pixel = 0;
    attrs.bit_gravity = StaticGravity;

    capi_CreateImage(&thisWindow->FrameBuffer, thisWindow->ClientWidth, thisWindow->ClientHeight, 0, 1);

    win = XCreateWindow(dpy, parent, 100, 100, thisWindow->ClientWidth, thisWindow->ClientHeight, 0, depth, InputOutput,
                        visual, CWBackPixel | CWColormap | CWBorderPixel | CWBitGravity, &attrs);

    XChangeProperty(dpy, win, XInternAtom(dpy, "_NET_WM_NAME", False), XInternAtom(dpy, "UTF8_STRING", False),
                    8, PropModeReplace, (unsigned char*)WinTitle, str_len(WinTitle));

    Window_Paint_Handler(thisWindow, &thisWindow->FrameBuffer);

    ximage = XCreateImage(dpy, visual, depth, ZPixmap, 0, (char*)thisWindow->FrameBuffer.Pixels, thisWindow->ClientWidth, thisWindow->ClientHeight, 8, thisWindow->ClientWidth * 4);
    if (ximage == NULL)
    {
        TestApp_OutMessage(STR("ERROR: XCreateImage Failed."), TRUE);
        return 0;
    }

    XSync(dpy, True);
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);

    gcv.graphics_exposures = 0;
    NormalGC = XCreateGC(dpy, parent, GCGraphicsExposures, &gcv);
    if (NormalGC == NULL)
    {
        TestApp_OutMessage(STR("ERROR: XCreateGC Failed."), TRUE);
        return 0;
    }

    Atom wmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(dpy, win, &wmDeleteWindow, 1);

    XMapWindow(dpy, win);

    snum = XDefaultScreen(dpy);
    ScreenWidth = XDisplayWidth(dpy, snum);
    ScreenHeight = XDisplayHeight(dpy, snum);
    XMoveWindow(dpy, win, (ScreenWidth - thisWindow->ClientWidth) / 2, (ScreenHeight - thisWindow->ClientHeight) / 2);

    thisWindow->sys_internals.ExitMessageHandler = FALSE;

    while (thisWindow->sys_internals.ExitMessageHandler == FALSE)
    {
        XNextEvent(dpy, &event);

        switch (event.type)
        {
        case Expose:
            XPutImage(dpy, win, NormalGC, ximage, 0, 0, 0, 0, thisWindow->ClientWidth, thisWindow->ClientHeight);
            break;
        case ConfigureNotify:
            break;
        case ClientMessage:
            if (event.xclient.data.l[0] == wmDeleteWindow)
            {
                thisWindow->sys_internals.ExitMessageHandler = TRUE;
            }
            break;
        default:
            break;
        }
    }

    XFreeGC(dpy, NormalGC);
    XCloseDisplay(dpy);

    return 0;
}

ConsoleEntryPoint()
{
    const STRING* pVersion;
    THREAD_WINDOW* thisWindow;
    pthread_t tId;

    SetConsoleTitle(STR("Test App Input/Output"));

    capi_Version(&pVersion);
    TestApp_OutMessage(STR("welcome - CAPI ver "), FALSE);
    TestApp_OutMessage(pVersion, TRUE);

    capi_memset(app_windows, 0, sizeof(THREAD_WINDOW) * MAX_THREAD_WINDOWS);

enter_input_loop:
    thisWindow = TestApp_EnterInputLoop();
    if (thisWindow->App_Command == APP_COMMAND_EXIT) goto exit_TestApp;

    pthread_create(&tId, 0, (void *(*)(void *))TestApp_ThreadWindowWorker, thisWindow);

    goto enter_input_loop;

exit_TestApp:
    return 0;
}

#endif

#ifdef _MSC_VER

THREAD_WINDOW* TestApp_FindThreadWindowFromHwnd(HWND hWnd)
{
    U32 I;

    for (I = 0; I < MAX_THREAD_WINDOWS; I++)
    {
        if (app_windows[I].SlotUsed == TRUE)
        {
            if (app_windows[I].sys_internals.hWnd == hWnd) return &app_windows[I];
        }
    }

    return 0;
}

int SetupWindowFrameBuffer(THREAD_WINDOW* thisWindow, HDC WindowHDC, RECT* ClientRt)
{
    if (thisWindow->sys_internals.BufferHDC != 0)
    {
        DeleteDC(thisWindow->sys_internals.BufferHDC);
        thisWindow->sys_internals.BufferHDC = 0;
    }

    if (thisWindow->sys_internals.pDisplayBitmap != 0)
    {
        capi_free(thisWindow->sys_internals.pDisplayBitmap);
        thisWindow->sys_internals.pDisplayBitmap = 0;
    }

    if (thisWindow->sys_internals.hBitmap != 0)
    {
        DeleteObject(thisWindow->sys_internals.hBitmap);
        thisWindow->sys_internals.hBitmap = 0;
    }

    thisWindow->sys_internals.pDisplayBitmap = (BITMAPINFO*)capi_malloc(sizeof(BITMAPINFOHEADER));
    if (thisWindow->sys_internals.pDisplayBitmap == 0) return 1;

    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biWidth = ClientRt->right;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biHeight = -ClientRt->bottom;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biPlanes = 1;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biBitCount = 32;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biCompression = 0;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biSizeImage = 0;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biXPelsPerMeter = 0;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biYPelsPerMeter = 0;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biClrUsed = 0;
    thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biClrImportant = 0;

    thisWindow->sys_internals.BufferHDC = CreateCompatibleDC(WindowHDC);
    if (thisWindow->sys_internals.BufferHDC == 0)
    {
        capi_free(thisWindow->sys_internals.pDisplayBitmap);
        return 2;
    }

    thisWindow->sys_internals.hBitmap = CreateDIBSection(thisWindow->sys_internals.BufferHDC, thisWindow->sys_internals.pDisplayBitmap, 0, (void**)&thisWindow->FrameBuffer.Pixels, 0, 0);
    if (thisWindow->sys_internals.hBitmap == 0)
    {
        DeleteDC(thisWindow->sys_internals.BufferHDC);
        capi_free(thisWindow->sys_internals.pDisplayBitmap);
        return 3;
    }

    GdiFlush();

    thisWindow->FrameBuffer.ScanLine = ClientRt->right;
    thisWindow->FrameBuffer.Width = ClientRt->right;
    thisWindow->FrameBuffer.Height = ClientRt->bottom;

    SelectObject(thisWindow->sys_internals.BufferHDC, thisWindow->sys_internals.hBitmap);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC WinDC;
    PAINTSTRUCT ps;
    RECT WinArea;
    RECT ClientArea;
    int BorderWidth;
    int BorderHeight;
    THREAD_WINDOW* thisWindow;

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_PAINT:
    {
        WinDC = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &ClientArea);

        thisWindow = TestApp_FindThreadWindowFromHwnd(hWnd);

        if ((ClientArea.right == 0) || (ClientArea.bottom == 0)) goto exit_paint_message;
        if ((ClientArea.right != thisWindow->FrameBuffer.Width) || (ClientArea.bottom != thisWindow->FrameBuffer.Height))
        {
            if (SetupWindowFrameBuffer(thisWindow, WinDC, &ClientArea) != 0) goto exit_paint_message;
        }

        Window_Paint_Handler(thisWindow, &thisWindow->FrameBuffer);

        SetDIBits(thisWindow->sys_internals.BufferHDC, thisWindow->sys_internals.hBitmap, 0,
                  thisWindow->sys_internals.pDisplayBitmap->bmiHeader.biHeight, thisWindow->FrameBuffer.Pixels, thisWindow->sys_internals.pDisplayBitmap, 0);
        BitBlt(WinDC, 0, 0, ClientArea.right, ClientArea.bottom, thisWindow->sys_internals.BufferHDC, 0, 0, 0x00CC0020);

exit_paint_message:
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_CREATE:
    {
        thisWindow = TestApp_FindThreadWindowFromHwnd(0);
        thisWindow->sys_internals.hWnd = hWnd;

        GetWindowRect(hWnd, &WinArea);
        GetClientRect(hWnd, &ClientArea);

        BorderWidth = WinArea.right - ClientArea.right;
        BorderHeight = WinArea.bottom - ClientArea.bottom;

        SetWindowPos(hWnd, NULL,
                     0, 0,
                     BorderWidth + thisWindow->ClientWidth, BorderHeight + thisWindow->ClientHeight, SWP_NOMOVE | SWP_NOZORDER);

        GetWindowRect(hWnd, &WinArea);

        SetWindowPos(hWnd, NULL,
                     (GetSystemMetrics(SM_CXSCREEN) - (WinArea.right - WinArea.left)) / 2,
                     (GetSystemMetrics(SM_CYSCREEN) - (WinArea.bottom - WinArea.top)) / 2,
                     0, 0, SWP_NOSIZE | SWP_NOZORDER);
        break;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

DWORD WINAPI TestApp_ThreadWindowWorker(THREAD_WINDOW* thisWindow)
{
    MSG msg;
    WNDCLASSEX wcex;

    thisWindow->sys_internals.hInstance = GetModuleHandle(NULL);

    if (is_WindowClass_registered == FALSE)
    {
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = thisWindow->sys_internals.hInstance;
        wcex.hIcon = NULL;
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = WinClassName;
        wcex.hIconSm = NULL;

        if (!RegisterClassEx(&wcex))
        {
            TestApp_OutMessage(STR("ERROR: Failed to register the window class."), TRUE);
            return 0;
        }
    }

    is_WindowClass_registered = TRUE;

    CreateWindowEx(0, WinClassName, WinTitle,
                   WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX | WS_MAXIMIZEBOX,
                   0, 0, 700, 500, NULL, NULL, thisWindow->sys_internals.hInstance, NULL);

    if (!thisWindow->sys_internals.hWnd)
    {
        TestApp_OutMessage(STR("ERROR: Failed to create the window."), TRUE);
        return 0;
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    thisWindow->SlotUsed = FALSE;

    return 0;
}

ConsoleEntryPoint()
{
    STRING* pVersion;
    THREAD_WINDOW* thisWindow;

    SetConsoleTitle(STR("Test App Input/Output"));

    capi_Version(&pVersion);
    TestApp_OutMessage(STR("welcome - CAPI ver "), FALSE);
    TestApp_OutMessage(pVersion, TRUE);

    capi_memset(app_windows, 0, sizeof(THREAD_WINDOW) * MAX_THREAD_WINDOWS);

enter_input_loop:
    thisWindow = TestApp_EnterInputLoop();
    if (thisWindow->App_Command == APP_COMMAND_EXIT) goto exit_TestApp;

    CreateThread(0, 0, TestApp_ThreadWindowWorker, thisWindow, 0, 0);

    goto enter_input_loop;

exit_TestApp:
    return 0;
}

#endif
