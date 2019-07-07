
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

BITMAPFILEHEADER getbmpFileHeader(FILE *fptr)
{
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, fptr);
    return bf;
}

BITMAPINFOHEADER getbmpInfoHeader(FILE *fptr)
{
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, fptr);
    return bi;
}

RGBTRIPLE *getbmpRgb(FILE *fptr, int biHeight, int biWidth, int padding)
{
    biHeight = abs(biHeight);
    int len = biHeight * biWidth;
    RGBTRIPLE *rgbt = malloc(len * sizeof(RGBTRIPLE));

    int k = 0;
    for (int i = 0; i < biHeight; i++)
    {
        for (int j = 0; j < biWidth; j++)
        {
            RGBTRIPLE r;
            fread(&r, sizeof(RGBTRIPLE), 1, fptr);

            *(rgbt + k) = r;
            k++;
        }

        // skip padding
        fseek(fptr, padding, SEEK_CUR);
    }

    return rgbt;
}   

BITMAP getBitmapFromFile(FILE *fptr)
{
    BITMAP bmp  = {.bf = getbmpFileHeader(fptr), .bi = getbmpInfoHeader(fptr)};
    int padding = calculatePadding(bmp.bi.biWidth);
    bmp.rgbt = getbmpRgb(fptr, bmp.bi.biHeight, bmp.bi.biWidth, padding);

    return bmp;
}

int calculatePadding(int biWidth)
{
    int padding = (4 - (biWidth* sizeof(RGBTRIPLE) ) % 4) % 4;
    return padding;
}

void writeBitmap(FILE *ftpr, BITMAP bmp, int padding)
{
    // write file header.
    fwrite(&bmp.bf, sizeof(BITMAPFILEHEADER), 1, ftpr);
    // write file info header.
    fwrite(&bmp.bi, sizeof(BITMAPINFOHEADER), 1, ftpr);

    int counter = 0; 
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bmp.bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < bmp.bi.biWidth; j++)
        {
            // read RGB triple from infile
            RGBTRIPLE triple = *(bmp.rgbt + counter);
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, ftpr);
            counter++;
        }

        // write padding
        for (int p = 0; p < padding; p++)
        {
            fputc(0x00, ftpr);
        }
            
    }
}