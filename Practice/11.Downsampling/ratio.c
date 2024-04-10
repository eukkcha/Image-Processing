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
	inputFile1 = fopen("AIcenterY.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	int width = bmpInfo1.biWidth;
	int height = bmpInfo1.biHeight;
	int size = bmpInfo1.biSizeImage;
	int bitCnt = bmpInfo1.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);

	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size, inputFile1);

	unsigned char* y1 = (unsigned char*)calloc(size, sizeof(unsigned char));

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			y1[j * width + i] = inputImg1[j * stride + 3 * i + 0];

	int ratio = 2;
	int width2 = bmpInfo1.biWidth >> ratio;
	int height2 = bmpInfo1.biHeight >> ratio;
	int stride2 = ((bitCnt / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	unsigned char* y2 = (unsigned char*)calloc(width2 * height2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y2[j * width2 + i] = y1[(j << ratio) * width + (i << ratio)];

	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = y2[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = y2[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = y2[j * width2 + i];
		}

	FILE* outputFile1 = fopen("ratio.bmp", "wb");
	bmpInfo1.biWidth = width2;
	bmpInfo1.biHeight = height2;
	bmpInfo1.biSizeImage = size2;
	bmpFile1.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + size2;
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size2, outputFile1);

	free(inputImg1);
	fclose(inputFile1);

	free(y2);
	free(outputImg1);
	fclose(outputFile1);
}
