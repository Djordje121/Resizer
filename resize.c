/** Resize input bitmap file by n factor.
 *  Output result to file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bmp.h"
#include "resize.h"

int main(int argc, char *argv[])
{
    char ch;
    int resize = 0;
    const char *infile;
    const char *outfile;

    if (argc != 4)
    {
        printf("Ussage: <infile path> <outfile path> <n>\n");
        return 1;
    }

    infile = argv[1];
    outfile = argv[2];
    resize = atoi(argv[3]);
    if (resize <= 0)
    {
        printf("Invalid resize factor\n");
        return 2;
    }
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "%s %s\n", "can't open file ", infile);
        return 2;
    }
    
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fprintf(stderr, "%s %s\n", "can't open file ", outfile);
        return 2;
    }

    // Resize logic.
    BITMAP currentBitmap = getBitmapFromFile(inptr);

    BITMAP resizedBitmap = resizeBmp(currentBitmap, resize);
    int padding = calculatePadding(resizedBitmap.bi.biWidth);
    writeBitmap(outptr, resizedBitmap, padding);
    
    printf("Resized successfull\n");

	// close streams.
	fclose(inptr);
	fclose(outptr);
    // free allocated space.
    free(currentBitmap.rgbt);
    free(resizedBitmap.rgbt);

    return 0;
}

BITMAP resizeBmp(BITMAP inputBitmap, int n)
{
    BITMAP resizedBmp = { inputBitmap.bf, inputBitmap.bi, NULL};
    // Resize .bmp by n factor.
    resizedBmp.bi.biWidth = inputBitmap.bi.biWidth * n;
    resizedBmp.bi.biHeight = inputBitmap.bi.biHeight * n;
    // Calculate resized bitmap padding.
    int currentBiPadding = calculatePadding(resizedBmp.bi.biWidth);
    // Temporary storrage.
    int resizedBiWidth = abs(resizedBmp.bi.biWidth);
    int resizedBiHeight = abs(resizedBmp.bi.biHeight);
    // Update .bmp header info.
    resizedBmp.bi.biSizeImage = (sizeof(RGBTRIPLE) * resizedBiWidth + currentBiPadding) *  resizedBiHeight;
    resizedBmp.bf.bfSize = resizedBmp.bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // Resize each pixel by n factor
    RGBTRIPLE *rgbtArr = malloc(resizedBiHeight * resizedBiWidth * sizeof(RGBTRIPLE));
    int k = 0;
    int c_rgbt = 0;

    for (int i = 0, h = abs(inputBitmap.bi.biHeight); i < h; i++)
    {
        for (int j = 0, w = abs(inputBitmap.bi.biWidth); j < w; j++)
        {
            // Read pixel from bmp.
            RGBTRIPLE rgbt = inputBitmap.rgbt[k];     
            // Resize pixel by n factor.
            for (int r = 0; r < n; r++)
            {
                rgbtArr[c_rgbt] = rgbt;
                c_rgbt++;
            }   
            k++;         
        }
        // Repeat n times resizing each pixel by n factor.
        for (int r = 0; r < n - 1; r++)
        {
            for (int j = 0, w = abs(inputBitmap.bi.biWidth); j < w; j++)
            {
                RGBTRIPLE rgbt = rgbtArr[c_rgbt - resizedBmp.bi.biWidth];
                for (int c = 0; c < n; c++)
                {
                    rgbtArr[c_rgbt] = rgbt; 
                    c_rgbt++;            
                }
            }
        }
    }
    // Update bitmap pixels.
    resizedBmp.rgbt = rgbtArr;
    return resizedBmp;
}

