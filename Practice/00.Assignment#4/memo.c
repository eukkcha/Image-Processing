#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main(int argc, char* argv[])
{
    // Input File1 (subsampled & noised)
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE* inputFile1 = NULL;
    inputFile1 = fopen("testY5n.bmp", "rb");
    fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
    fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    // Input File2 (Original)
    BITMAPFILEHEADER bmpFile2;
    BITMAPINFOHEADER bmpInfo2;
    FILE* inputFile2 = NULL;
    inputFile2 = fopen("testY5.bmp", "rb");
    fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
    fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

    // File1 Size Info (256 x 256)
    int width1 = bmpInfo1.biWidth;
    int height1 = bmpInfo1.biHeight;
    int size1 = bmpInfo1.biSizeImage;
    int bitCnt1 = bmpInfo1.biBitCount;
    int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

    // InputImg1
    unsigned char* inputImg1 = NULL;
    inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

    // Function y1 : subsampled & noised
    unsigned char* y1 = NULL;
    y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// Denoise Code //
    // y2: Denoise(Gaussian Filter)
    unsigned char* y2 = calloc(size1, sizeof(unsigned char));

    // Set Gaussian Filter Kernel Size
    double kernel[9][9];
    int kernelSize = 9, offset = kernelSize / 2;

    double sum = 0.0;
    double sigma = 0.5; // The optimal Gaussian standard deviation
    // Calculate Gaussian values
    for (int m = -offset; m <= offset; m++)
        for (int n = -offset; n <= offset; n++) {
            kernel[m + offset][n + offset] = exp(-(m * m + n * n) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            sum += kernel[m + offset][n + offset];
        }
    for (int m = 0; m < kernelSize; m++)
        for (int n = 0; n < kernelSize; n++)
            kernel[m][n] /= sum;

    // Convolution
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
        {
            double value = 0.0;
            for (int m = -offset; m <= offset; m++)
                for (int n = -offset; n <= offset; n++) {
                    int py = j + m, px = i + n;
                    if (px >= 0 && px < width1 && py >= 0 && py < height1)
                        value += y1[py * width1 + px] * kernel[m + offset][n + offset];
                }

            y2[j * width1 + i] = (unsigned char)value;
        }

    // Upsample Code //
    // Upsample Size Info (1024 x 1024)
    int ratio = 2; // The scaling factor (2^ratio)
    int width2 = bmpInfo1.biWidth << ratio;
    int height2 = bmpInfo1.biHeight << ratio;
    int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
    int size2 = stride2 * height2;

    // Function y3: Upsample(Bilinear Interpolation)
    unsigned char* y3 = (unsigned char*)calloc(size2, sizeof(unsigned char));

    // Bilinear Interpolation
    for (int j = 0; j < height2; j++)
        for (int i = 0; i < width2; i++)
        {
            // Coordinates corresponding to the original image
            double origY = j / (double)(1 << ratio), origX = i / (double)(1 << ratio);
            // Integer part
            int x01 = (int)origX, y01 = (int)origY;
            int x02 = x01 + 1, y02 = y01 + 1;
            if (x02 >= width1) x02 = width1 - 1;
            if (y02 >= height1) y02 = height1 - 1;
            // Fractional part
            double dx = origX - x01, dy = origY - y01;

            // Weighted average of the 4 pixels
            double value = (1 - dx) * (1 - dy) * y2[y01 * width1 + x01] +
                dx * (1 - dy) * y2[y01 * width1 + x02] +
                (1 - dx) * dy * y2[y02 * width1 + x01] +
                dx * dy * y2[y02 * width1 + x02];

            y3[j * width2 + i] = (unsigned char)value;
        }

    // PSNR Code //
    // Function inputImg2 (Original.bmp)
    unsigned char* inputImg2 = NULL;
    inputImg2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
    fread(inputImg2, sizeof(unsigned char), size2, inputFile2);

    // Function y: Original
    unsigned char* y = NULL;
    y = (unsigned char*)calloc(size2, sizeof(unsigned char));
    for (int j = 0; j < height2; j++)
        for (int i = 0; i < width2; i++)
            y[j * width2 + i] = inputImg2[j * stride2 + 3 * i + 0];

    // PSNR1 (Original - Result)
    double mse = 0, psnr;
    for (int j = 0; j < height2; j++)
        for (int i = 0; i < width2; i++)
            mse += (double)((y[j * width2 + i] - y3[j * width2 + i]) * (y[j * width2 + i] - y3[j * width2 + i]));
    mse /= (width2 * height2);
    psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
    printf("%.2lf(%.2lf)\n", psnr, mse);

    // Ouput Code //
    // outputImg1: result
    unsigned char* outputImg1 = NULL;
    outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));
    for (int j = 0; j < height2; j++)
        for (int i = 0; i < width2; i++) {
            outputImg1[j * stride2 + 3 * i + 0] = y3[j * width2 + i];
            outputImg1[j * stride2 + 3 * i + 1] = y3[j * width2 + i];
            outputImg1[j * stride2 + 3 * i + 2] = y3[j * width2 + i];
        }

    // output to BMP file
    FILE* outputFile1 = fopen("Result.bmp", "wb");
    bmpInfo1.biWidth = width2;
    bmpInfo1.biHeight = height2;
    bmpInfo1.biSizeImage = size2;
    bmpFile1.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + size2;
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size2, outputFile1);

    // free memory
    free(inputImg1);
    fclose(inputFile1);
    free(inputImg2);
    fclose(inputFile2);

    free(y);  // Original
    free(y1); // Subsampled & Noised
    free(y2); // Gaussian Filter
    free(y3); // Bilinear Interpolation

    free(outputImg1);
    fclose(outputFile1);
}
