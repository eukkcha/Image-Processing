#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// I/O Code //
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// File1 Size Info 
	int width1 = bmpInfo1.biWidth;
	int height1 = bmpInfo1.biHeight;
	int size1 = bmpInfo1.biSizeImage;
	int bitCnt1 = bmpInfo1.biBitCount;
	int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

	// InputImg1
	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

	// Function y1: 
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// Encoding Code //
	// bitstream.txt File
	FILE* bitstream = fopen("bitstream.txt", "w");

	// Encoding
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			// Conversion from 8 to 4 bit-depth
			int value = y1[j * width1 + i] / 16 * 16;
			// Generation of bitstream with fixed-length codes
			if (value == 0) fprintf(bitstream, "0000");
			else if (value == 16) fprintf(bitstream, "0001");
			else if (value == 32) fprintf(bitstream, "0010");
			else if (value == 48) fprintf(bitstream, "0011");
			else if (value == 64) fprintf(bitstream, "0100");
			else if (value == 80) fprintf(bitstream, "0101");
			else if (value == 96) fprintf(bitstream, "0110");
			else if (value == 112) fprintf(bitstream, "0111");
			else if (value == 128) fprintf(bitstream, "1000");
			else if (value == 144) fprintf(bitstream, "1001");
			else if (value == 160) fprintf(bitstream, "1010");
			else if (value == 176) fprintf(bitstream, "1011");
			else if (value == 192) fprintf(bitstream, "1100");
			else if (value == 208) fprintf(bitstream, "1101");
			else if (value == 224) fprintf(bitstream, "1110");
			else if (value == 240) fprintf(bitstream, "1111");
		}

	// free memory
	free(inputImg1);
	fclose(inputFile1);

	free(y1);

	fclose(bitstream);
}
