#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // I/O Code //
    // InputFile1 (noised)
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE* inputFile1 = NULL;
    inputFile1 = fopen("AICenterY_CombinedNoise.bmp", "rb");
    fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
    fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    // File1 Size Info (1024 x 1024)
    int width1 = bmpInfo1.biWidth;
    int height1 = bmpInfo1.biHeight;
    int size1 = bmpInfo1.biSizeImage;
    int bitCnt1 = bmpInfo1.biBitCount;
    int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

    // InputImg1 
    unsigned char* inputImg1 = NULL;
    inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

    // Function y1: noised 
    unsigned char* y1 = NULL;
    y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++) 
            y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

    // Padding code //
    // Half Padding Size Info
    int p = (3 - 1) / 2; // Half Padding의 폭p: ((Filter의 폭p) - 1) / 2
    int pwidth = width1 + 2 * p; // Half Padding을 적용한 Size: (height + 2p, width + 2p)
    int pheight = height1 + 2 * p;
    int psize = pwidth * pheight * 3;

    // Function y2: Padding
    unsigned char* y2 = NULL;
    y2 = (unsigned char*)calloc(psize, sizeof(unsigned char));

    // 1. Zero Padding
    for (int j = 0; j < pheight; j++)
        for (int i = 0; i < pwidth; i++)
            y2[j * pwidth + i] = 0;
    // Paste Original Image to Center
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            y2[(p + j) * pwidth + (p + i)] = y1[j * width1 + i];

    // 2. Replicate Padding
    for (int j = 0; j < p; j++)
        for (int i = 0; i < width1; i++) {
            y2[j * pwidth + p + i] = y1[i]; // 상단
            y2[(pheight - 1 - j) * pwidth + p + i] = y1[(height1 - 1) * width1 + i]; // 하단
        }
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < p; i++) {
            y2[(p + j) * pwidth + i] = y1[j * width1]; // 좌측
            y2[(p + j) * pwidth + (pwidth - 1 - i)] = y1[j * width1 + width1 - 1]; // 우측
        }
    for (int j = 0; j < p; j++)
        for (int i = 0; i < p; i++) {
            y2[j * pwidth + i] = y1[0]; // 좌상단
            y2[j * pwidth + pwidth - 1 - i] = y1[width1 - 1]; // 우상단
            y2[(pheight - 1 - j) * pwidth + i] = y1[(height1 - 1) * width1]; // 좌하단
            y2[(pheight - 1 - j) * pwidth + pwidth - 1 - i] = y1[(height1 - 1) * width1 + width1 - 1]; // 우하단
        }

    // 3. Mirror Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width1; i++) {
			y2[(p - 1 - j) * pwidth + p + i] = y1[(j + 1) * width1 + i]; // 상단
			y2[(pheight - p + j) * pwidth + p + i] = y1[(height1 - 2 - j) * width1 + i]; // 하단
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < p; i++) {
			y2[(p + j) * pwidth + p - 1 - i] = y1[j * width1 + 1 + i]; // 좌측
			y2[(p + j) * pwidth + (pwidth - p + i)] = y1[j * width1 + width1 - 2 - i]; // 우측
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			y2[(p - 1 - j) * pwidth + (p - 1 - i)] = y1[(1 + j) * width1 + (1 + i)]; // 좌상단
			y2[(p - 1 - j) * pwidth + pwidth - p + i] = y1[(1 + j) * width1 + width1 - 2 - i]; // 우상단
			y2[(pheight - p + j) * pwidth + p - 1 - i] = y1[(height1 - 2 - j) * width1 + 1 + i]; // 좌하단
			y2[(pheight - p + j) * pwidth + pwidth - p + i] = y1[(height1 - 2 - j) * width1 + width1 - 2 - i]; // 우하단
		}

    // Filter Code //
    // Function y3: Filter
    unsigned char* y3 = NULL;
    y3 = (unsigned char*)calloc(size1, sizeof(unsigned char));

    // Gaussian Filter 3x3
    double Gauss3F[3][3] = { {(double)1 / 16, (double)2 / 16, (double)1 / 16},
                              {(double)2 / 16, (double)4 / 16, (double)2 / 16},
                              {(double)1 / 16, (double)2 / 16, (double)1 / 16} };
    // Mean Filter 3x3
    double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
                            {(double)1 / 9, (double)1 / 9, (double)1 / 9},
                            {(double)1 / 9, (double)1 / 9, (double)1 / 9} };
    // Convolution
    for (int j = p; j < p + height1; j++)
        for (int i = p; i < p + width1; i++) 
        {
            double value = 0.0;
            for (int m = 0; m < 3; m++) // 3x3 Filter
                for (int n = 0; n < 3; n++)
                    value += y2[(j - p + m) * pwidth + (i - p + n)] * Gauss3F[m][n];

            y3[(j - p) * width1 + (i - p)] = (unsigned char)value;
        }

    // PSNR Code //
    // InputFile2 (original)
    BITMAPFILEHEADER bmpFile2;
    BITMAPINFOHEADER bmpInfo2;
    FILE* inputFile2 = NULL;
    inputFile2 = fopen("AICenterY.bmp", "rb");
    fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
    fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

    // InputImg2
    unsigned char* inputImg2 = NULL;
    inputImg2 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    fread(inputImg2, sizeof(unsigned char), size1, inputFile2);

    // Function y: Original
    unsigned char* y = NULL;
    y = (unsigned char*)calloc(size1, sizeof(unsigned char));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            y[j * width1 + i] = inputImg2[j * stride1 + 3 * i + 0];

    // PSNR1: original - noised (y - y1)
    double mse = 0, psnr;
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            mse += (double)((y[j * width1 + i] - y1[j * width1 + i]) * (y[j * width1 + i] - y1[j * width1 + i]));
    mse /= (width1 * height1);
    psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
    printf("[noised PSNR]\n%.2lfdB(%.2lf)\n\n", psnr, mse);
    // 18.13dB

    // PSNR2: original - result (y - y3)
    double mse_result = 0, psnr_result;
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            mse_result += (double)((y[j * width1 + i] - y3[j * width1 + i]) * (y[j * width1 + i] - y3[j * width1 + i]));
    mse_result /= (width1 * height1);
    psnr_result = mse_result != 0.0 ? 10.0 * log10(255 * 255 / mse_result) : 99.99;
    printf("[result PSNR]\n%.2lfdB(%.2lf)\n\n", psnr_result, mse_result);

    // I/O Code //
    // outputImg1: y3
    unsigned char* outputImg1 = NULL;
    outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++) {
            outputImg1[j * stride1 + 3 * i + 0] = y3[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 1] = y3[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 2] = y3[j * width1 + i];
        }

    // output to BMP file
    FILE* outputFile1 = fopen("result.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

    // free memory
    free(inputImg1);
    fclose(inputFile1);
    free(inputImg2);
    fclose(inputFile2);

    free(y);
    free(y1);
    free(y2);
    free(y3);

    free(outputImg1);
    fclose(outputFile1);
}

// // outputImg1: y2 (exclude padding part)
//     unsigned char* outputImg1 = NULL;
//     outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
//     for (int j = 0; j < height1; j++)
//         for (int i = 0; i < width1; i++) {
//             outputImg1[j * stride1 + 3 * i + 0] = y2[(p + j) * pwidth + (p + i)];
//             outputImg1[j * stride1 + 3 * i + 1] = y2[(p + j) * pwidth + (p + i)];
//             outputImg1[j * stride1 + 3 * i + 2] = y2[(p + j) * pwidth + (p + i)];
//         }
