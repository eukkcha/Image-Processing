#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("Subsampling.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	BITMAPFILEHEADER bmpFile2;
	BITMAPINFOHEADER bmpInfo2;
	FILE* inputFile2 = NULL;
	inputFile2 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
	fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

	int width = bmpInfo1.biWidth;
	int height = bmpInfo1.biHeight;
	int size = bmpInfo1.biSizeImage;
	int bitCnt = bmpInfo1.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);

	int width2 = bmpInfo2.biWidth;
	int height2 = bmpInfo2.biHeight;
	int size2 = bmpInfo2.biSizeImage;
	int bitCnt2 = bmpInfo2.biBitCount;
	int stride2 = (((bitCnt / 8) * width2) + 3 / 4 * 4);

	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size, inputFile1);

	unsigned char* inputImg2 = NULL;
	inputImg2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	fread(inputImg2, sizeof(unsigned char), size2, inputFile2);

	unsigned char* y1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	unsigned char* y2 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			y1[j * width + i] = inputImg1[j * stride + 3 * i + 0];

	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y2[j * width2 + i] = inputImg2[j * stride2 + 3 * i + 0];

	int ratio = 1;
	int width3 = bmpInfo1.biWidth << ratio;
	int height3 = bmpInfo1.biHeight << ratio;
	int stride3 = ((bitCnt / 8) * width3 + 3) / 4 * 4;
	int size3 = stride3 * height3;
	printf("w : %d(%d)\nh : %d\ns : %d\n\n", width, stride, height, size);
	printf("w : %d(%d)\nh : %d\ns : %d\n\n", width2, stride2, height2, size2);
	printf("w : %d(%d)\nh : %d\ns : %d\n\n", width3, stride3, height3, size3);

	unsigned char* y3 = (unsigned char*)calloc(size3, sizeof(unsigned char));
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			y3[(2 * j) * width3 + (2 * i)] = y1[j * width + i];
			y3[(2 * j) * width3 + (2 * i + 1)] = y1[j * width + i];
			y3[(2 * j + 1) * width3 + (2 * i)] = y1[j * width + i];
			y3[(2 * j + 1) * width3 + (2 * i + 1)] = y1[j * width + i];
		}

	double mse = 0, psnr;
	for (int j = 0; j < height3; j++)
		for (int i = 0; i < width3; i++)
			mse += (double)((y3[j * width3 + i] - y2[j * width2 + i]) * (y3[j * width3 + i] - y2[j * width2 + i]));
	mse /= (width3 * height3);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);

	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size3, sizeof(unsigned char));

	for (int j = 0; j < height3; j++)
		for (int i = 0; i < width3; i++) {
			outputImg1[j * stride3 + 3 * i + 0] = y3[j * width3 + i];
			outputImg1[j * stride3 + 3 * i + 1] = y3[j * width3 + i];
			outputImg1[j * stride3 + 3 * i + 2] = y3[j * width3 + i];
		}

	FILE* outputFile1 = fopen("Nearest_Neighbor_Interpolation.bmp", "wb");
	bmpInfo1.biWidth = width3;
	bmpInfo1.biHeight = height3;
	bmpInfo1.biSizeImage = size3;
	bmpFile1.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + size2;
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size2, outputFile1);

	free(inputImg1);
	fclose(inputFile1);
	free(inputImg2);
	fclose(inputFile2);

	free(y1);
	free(y2);
	free(y3);
	free(outputImg1);
	fclose(outputFile1);
}
