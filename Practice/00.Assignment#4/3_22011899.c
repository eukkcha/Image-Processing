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
	// Assignment#4 Starting Rule
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	FILE* outputFile1 = NULL;
	inputFile1 = fopen(argv[1], "rb");
    outputFile1 = fopen("22011899.bmp", "wb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

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

	// Upsample Code //
	// Upsample Size Info (1024 x 1024)
	int ratio = 2; // The scaling factor (2^ratio)
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// Function y2: Upsample(Bilinear Interpolation) + Denoise(Median Filter)
	unsigned char* y2 = (unsigned char*)calloc(size2, sizeof(unsigned char));

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
			double value = (1 - dx) * (1 - dy) * y1[y01 * width1 + x01] +
				dx * (1 - dy) * y1[y01 * width1 + x02] +
				(1 - dx) * dy * y1[y02 * width1 + x01] +
				dx * dy * y1[y02 * width1 + x02];

			y2[j * width2 + i] = (unsigned char)value;
		}

	// Denoise Code //
	// Median Filter
	int len = 9;
	int values[9];
	for (int j = 1; j < height2 - 1; j++)
		for (int i = 1; i < width2 - 1; i++)
		{
			int F, k = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++) {
					values[k] = y2[(j - 1 + m) * width2 + (i - 1 + n)];
					k++;
				}

			// Calculate Median
			int m, n, tmp = 0;
			for (m = 0; m < len; m++)
				for (n = 0; n < (len - 1) - m; n++)
					if (values[n] > values[n + 1])
					{
						tmp = values[n];
						values[n] = values[n + 1];
						values[n + 1] = tmp;
					}
			F = values[len / 2];

			y2[j * width2 + i] = F;
		}

	// result: Denoise(Gaussian Filter)
	unsigned char* result = calloc(size2, sizeof(unsigned char));

	// Set Gaussian Filter Kernel Size
	double kernel[9][9];
	int kernelSize = 9, offset = kernelSize / 2;

	double sum = 0.0;
	double sigma = 1.7; // The optimal Gaussian standard deviation
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
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			double value = 0.0;
			for (int m = -offset; m <= offset; m++)
				for (int n = -offset; n <= offset; n++) {
					int py = j + m, px = i + n;
					if (px >= 0 && px < width2 && py >= 0 && py < height2)
						value += y2[py * width2 + px] * kernel[m + offset][n + offset];
				}

			result[j * width2 + i] = (unsigned char)value;
		}

	// Ouput Code //
	// outputImg1: result
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result[j * width2 + i];
		}

	// output to BMP file
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

	free(y1); // Subsample & Noise
	free(y2); // Apply Bilinear Interpolation
	free(result); // Apply Gaussian Filter

	free(outputImg1);
	fclose(outputFile1);
}
