
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

RGBTRIPLE *getbmpRgb(FILE *fptr, int biHeight, int biWidth, int padding, int backwards)
{
    int len = abs(biHeight) * biWidth;
    RGBTRIPLE *rgbt = malloc(len * sizeof(RGBTRIPLE));

    if (!backwards)
        biHeight = abs(biHeight);

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

BITMAP getbmpFromFile(FILE *fptr, int backwards)
{
    BITMAP bmp  = {.bf = getbmpFileHeader(fptr), .bi = getbmpInfoHeader(fptr)};
    int padding = calculatePadding(bmp.bi.biWidth);
    bmp.rgbt = getbmpRgb(fptr, bmp.bi.biHeight, bmp.bi.biWidth, padding, backwards);

    return bmp;
}

int calculatePadding(int biWidth)
{
    int padding = (4 - (biWidth* sizeof(RGBTRIPLE) ) % 4) % 4;
    return padding;
}


void writeBmp(FILE *fptr, BITMAP bmp, int padding, int backwards)
{
    // write file header
    fwrite(&bmp.bf, sizeof(BITMAPFILEHEADER), 1, fptr);

    // write file info header
    fwrite(&bmp.bi, sizeof(BITMAPINFOHEADER), 1, fptr);

    int biHeight = bmp.bi.biHeight;
    if (!backwards)
        biHeight = abs(biHeight);

    int k = 0;
    for (int i = 0; i < biHeight; i++)
    {
        for (int j = 0; j < bmp.bi.biWidth; j++)
        {
            RGBTRIPLE rgbt = *(bmp.rgbt + k);
            
            fwrite(&rgbt, sizeof(RGBTRIPLE), 1, fptr);
            k++;
        }

        // write padding
        for (int p = 0; p < padding; p++)
            fputc(0x00, fptr);
    }
}
