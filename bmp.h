// BMP-related data types based on Microsoft's own

#include <stdio.h>
#include <stdint.h>

// aliases for C/C++ primitive data types
// https://msdn.microsoft.com/en-us/library/cc230309.aspx
typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

// information about the type, size, and layout of a file
// https://msdn.microsoft.com/en-us/library/dd183374(v=vs.85).aspx
typedef struct
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

// information about the dimensions and color format
// https://msdn.microsoft.com/en-us/library/dd183376(v=vs.85).aspx
typedef struct
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

// relative intensities of red, green, and blue
// https://msdn.microsoft.com/en-us/library/dd162939(v=vs.85).aspx
typedef struct
{
    BYTE rgbtBlue;
    BYTE rgbtGreen;
    BYTE rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;

// bitmap enumeration
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-wmf/4e588f70-bd92-4a6f-b77f-35d0feaf7a57
typedef  enum
 {
   BI_RGB = 0x0000,
   BI_RLE8 = 0x0001,
   BI_RLE4 = 0x0002,
   BI_BITFIELDS = 0x0003,
   BI_JPEG = 0x0004,
   BI_PNG = 0x0005,
   BI_CMYK = 0x000B,
   BI_CMYKRLE8 = 0x000C,
   BI_CMYKRLE4 = 0x000D
 } __attribute__((__packed__))
Compression;


 typedef struct 
 {
     BITMAPFILEHEADER bf;
     BITMAPINFOHEADER bi;
     RGBTRIPLE *rgbt;
 } __attribute__((__packed__))
BITMAP;




BITMAPFILEHEADER getbmpFileHeader(FILE *fptr);

BITMAPINFOHEADER getbmpInfoHeader(FILE *fptr);

RGBTRIPLE *getbmpRgb(FILE *fptr, int biHeight, int biWidth, int padding);

int calculatePadding(int biWidth);

BITMAP getbmpFromFile(FILE *fptr);

void writeBmp(FILE *fptr, BITMAP bmp, int padding);

