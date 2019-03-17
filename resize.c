/** Resize input bitmap file by n factor.
 *  Output result to file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bmp.h"
#include "resize.h"


#define PRINT_INFO (bmpInfo == 0 ? "false" : "true")

int main(int argc, char *argv[])
{
    char ch;
    int bmpInfo = 0;
    int resize = 0;
    const char *infile;
    const char *outfile;

    while ((ch = getopt(argc, argv, "o:c:v")) != -1)
    {
        switch (ch)
        {
            case 'v':
                bmpInfo = 1;
                break;
            case 'o':         
            outfile = optarg;
                break;      
            case 'c':
            resize = atoi(optarg);
            break;
        }
    }

    if (argc < 6)
    {
        printf("Ussage: <infile path> -o <outfile path> -c <n> -v(optional)\n");
        return 1;
    }

    infile = argv[optind];
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

    BITMAP bmp = getbmpFromFile(inptr);

    BITMAP resized = resizeBmp(bmp, resize);
    int padding = calculatePadding(resized.bi.biWidth);
    writeBmp(outptr, resized, padding);
    
    printf("Infile: %s\n", infile);
    printf("Outfile: %s\n", outfile);
    printf("Print bmp info: %s\n", PRINT_INFO);

    if (bmpInfo)
    {
        printf(">>>>>>>>>>>>>>>> INPUT BMP\n");
        printf("Width %d\n", bmp.bi.biHeight);
        printf("Height: %d\n", bmp.bi.biHeight);
        printf("Image size(bytes): %d\n", bmp.bi.biSizeImage);
        printf("File size(bytes): %d\n", bmp.bf.bfSize);
        printf("Bits per pixel %d\n", bmp.bi.biBitCount);
        printf("\n<<<<<<<<<<<<<<<< OUTPUT BMP\n");
        printf("Width %d\n", resized.bi.biHeight);
        printf("Height: %d\n", resized.bi.biHeight);
        printf("Image size(bytes): %d\n", resized.bi.biSizeImage);
        printf("File size(bytes): %d\n", resized.bf.bfSize);
        printf("Bits per pixel %d\n", resized.bi.biBitCount);
    }

    printf("Resized successfull\n");

    return 0;
}



BITMAP resizeBmp(BITMAP bmp, int n)
{
    // Remember current size. 
    int h = abs(bmp.bi.biHeight);
    int w = abs(bmp.bi.biWidth);

    // Resize .bmp by n factor.
    bmp.bi.biWidth *= n;
    bmp.bi.biHeight *= n;

    // Set positive height to avoid bitmap backwards reading.
    int biHeight = abs(bmp.bi.biHeight);
    int padding = calculatePadding(bmp.bi.biWidth);

    // Update .bmp header info.
    bmp.bi.biSizeImage =  (sizeof(RGBTRIPLE) * bmp.bi.biWidth + padding) *  biHeight;
    bmp.bf.bfSize = bmp.bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    // Resize each pixel by n factor
    int bmpLen = biHeight * bmp.bi.biWidth;
    RGBTRIPLE *rgbtArr = malloc(bmpLen * sizeof(RGBTRIPLE));
    int k = 0;
    int x = 0;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            // Read pixel from bmp.
            RGBTRIPLE rgbt = bmp.rgbt[k];     

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
            for(int j = 0; j < w; j++)
            {
                RGBTRIPLE rgbt = rgbtArr[x - bmp.bi.biWidth];
                for (int r = 0; r < n; r++)
                {
                    rgbtArr[x] = rgbt; 
                    x++;            
                }
            }
        }
    }

    // Update bitmap pixels.
    bmp.rgbt = rgbtArr;

    return bmp;
}

