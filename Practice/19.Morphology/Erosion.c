#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// Input File1 (Threshold)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("Threshold.bmp", "rb");
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

	// Function y1 : Original
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// y2: Erosion
	unsigned char* y2 = calloc(size1, sizeof(unsigned char));
	
	// Erosion Filter
	int ErosionF[3][3] = { {255, 255, 255},
						   {255, 255, 255},
						   {255, 255, 255} };

	// Erosion
	for (int j = 1; j < height1 - 1; j++)
		for (int i = 1; i < width1 - 1; i++)
		{
			int flag = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					if (ErosionF[m][n] == 255 && y1[j - 1 + m * width1 + i - 1 + m] == 0)
						flag++;

			if (flag > 0)
				y2[j * width1 + i] = 0;
		}

	// Ouput Code //
	// outputImg1: result
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = y2[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 1] = y2[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 2] = y2[j * width1 + i];
		}

	// output to BMP file
	FILE* outputFile1 = fopen("Output.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

	// free memory
	free(inputImg1);
	fclose(inputFile1);

	free(y1);
	free(y2);

	free(outputImg1);
	fclose(outputFile1);
}
