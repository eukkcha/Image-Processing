#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable : 4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // I/O Code //
    // inputFile1: AICenterY.bmp
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE *inputFile1 = NULL;
    inputFile1 = fopen("reconEncY.bmp", "rb");
    fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
    fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    // File1 Size Info
    int width1 = bmpInfo1.biWidth;
    int height1 = bmpInfo1.biHeight;
    int size1 = bmpInfo1.biSizeImage;
    int bitCnt1 = bmpInfo1.biBitCount;
    int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

    // InputImg1
    unsigned char *inputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

    // DPCM Based Decoder code //
    int *e = (int *)calloc(size1, sizeof(int));    // Function e: Prediction Error
    int *qtz = (int *)calloc(size1, sizeof(int));  // Function qtz: Quantization
    int *iqtz = (int *)calloc(size1, sizeof(int)); // Function iqtz: Inverse Quantization
    int *r = (int *)calloc(size1, sizeof(int));    // Function r: Reconstruction

    // Open bitstream.txt for reading
    FILE *bitstream = fopen("bitstream.txt", "rb");

    // outputImg1: r(dec)
    unsigned char *outputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));

    int p;      // predicted
    int q = 20; // Quantization parameter
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
        {
            if (i == 0)
                p = 128; // Prediction with a predfined value for the first pixel of each row

            // Inverse Binarization
            char bin[5];
            fscanf(bitstream, "%4s", bin);
            if (strcmp(bin, "0000") == 0)
                qtz[j * width1 + i] = -7;
            else if (strcmp(bin, "0001") == 0)
                qtz[j * width1 + i] = -6;
            else if (strcmp(bin, "0010") == 0)
                qtz[j * width1 + i] = -5;
            else if (strcmp(bin, "0011") == 0)
                qtz[j * width1 + i] = -4;
            else if (strcmp(bin, "0100") == 0)
                qtz[j * width1 + i] = -3;
            else if (strcmp(bin, "0101") == 0)
                qtz[j * width1 + i] = -2;
            else if (strcmp(bin, "0110") == 0)
                qtz[j * width1 + i] = -1;
            else if (strcmp(bin, "0111") == 0)
                qtz[j * width1 + i] = 0;
            else if (strcmp(bin, "1000") == 0)
                qtz[j * width1 + i] = 1;
            else if (strcmp(bin, "1001") == 0)
                qtz[j * width1 + i] = 2;
            else if (strcmp(bin, "1010") == 0)
                qtz[j * width1 + i] = 3;
            else if (strcmp(bin, "1011") == 0)
                qtz[j * width1 + i] = 4;
            else if (strcmp(bin, "1100") == 0)
                qtz[j * width1 + i] = 5;
            else if (strcmp(bin, "1101") == 0)
                qtz[j * width1 + i] = 6;
            else if (strcmp(bin, "1110") == 0)
                qtz[j * width1 + i] = 7;
            else if (strcmp(bin, "1111") == 0)
                qtz[j * width1 + i] = 8;

            iqtz[j * width1 + i] = qtz[j * width1 + i] * q; // Inverse Quantization
            r[j * width1 + i] = iqtz[j * width1 + i] + p;   // Reconstruction
            p = r[j * width1 + i];                          // Update prediction

            // outputImg1: r(dec)
            outputImg1[j * stride1 + 3 * i + 0] = r[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 1] = r[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 2] = r[j * width1 + i];
        }

    // outputFile1: reconDecY.bmp
    FILE *outputFile1 = fopen("reconDecY.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

    // PSNR Code //
    // Function o: original
    int *o = (int *)calloc(size1, sizeof(int));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            o[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

    double mse = 0, psnr;
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            mse += (double)((o[j * width1 + i] - r[j * width1 + i]) * (o[j * width1 + i] - r[j * width1 + i]));
    mse /= (width1 * height1);
    psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
    printf("%.2lfdB(%.2lf)\n", psnr, mse);

    // close bitstream.txt
    fclose(bitstream);

    // free memory
    free(inputImg1);
    fclose(inputFile1);

    free(e);
    free(qtz);
    free(iqtz);
    free(r);

    free(outputImg1);
    fclose(outputFile1);
}
