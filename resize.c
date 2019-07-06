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
    BITMAP bmp = getbmpFromFile(inptr);

    BITMAP resized = resizeBmp(bmp, resize);
    int padding = calculatePadding(resized.bi.biWidth);
    writeBmp(outptr, resized, padding);
    
    printf("Infile: %s\n", infile);
    printf("Outfile: %s\n", outfile);
    printf("Resized successfull\n");


	// Close streams.
	fclose(inptr);
	fclose(outptr);

    return 0;
}


BITMAP resizeBmp(BITMAP inputBitmap, int n)
{
    BITMAP resizedBmp = { inputBitmap.bf, inputBitmap.bi };
    // Resize .bmp by n factor.
    resizedBmp.bi.biWidth = inputBitmap.bi.biWidth * n;
    resizedBmp.bi.biHeight = inputBitmap.bi.biHeight * n;
    //  calculate resized bitmap padding.
    int currentBiPadding = calculatePadding(resizedBmp.bi.biWidth);
    // Update .bmp header info.
    resizedBmp.bi.biSizeImage =  (sizeof(RGBTRIPLE) * resizedBmp.bi.biWidth + currentBiPadding) *  abs(resizedBmp.bi.biHeight);
    resizedBmp.bf.bfSize = resizedBmp.bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    // Resize each pixel by n factor
    int currentBmpLen = abs(resizedBmp.bi.biHeight) * resizedBmp.bi.biWidth;
    RGBTRIPLE *rgbtArr = malloc(currentBmpLen * sizeof(RGBTRIPLE));
    int k = 0;
    int x = 0;

    for (int i = 0; i < abs(inputBitmap.bi.biHeight); i++)
    {
        for (int j = 0; j < abs(inputBitmap.bi.biWidth); j++)
        {
            // Read pixel from bmp.
            RGBTRIPLE rgbt = inputBitmap.rgbt[k];     

            // Resize pixel by n factor.
            for (int r = 0; r < n; r++)
            {
                rgbtArr[x] = rgbt;
                x++;
            }   
            k++;         
        }

        // Repeat n times resizing each pixel by n factor.
        for (int r = 0; r < n - 1; r++)
        {
            for(int j = 0; j < abs(inputBitmap.bi.biWidth); j++)
            {
                RGBTRIPLE rgbt = rgbtArr[x - resizedBmp.bi.biWidth];
                for (int r = 0; r < n; r++)
                {
                    rgbtArr[x] = rgbt; 
                    x++;            
                }
            }
        }
    }

    // Update bitmap pixels.
    resizedBmp.rgbt = rgbtArr;

    return resizedBmp;
}

