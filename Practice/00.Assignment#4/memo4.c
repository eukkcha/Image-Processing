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
	// 영상1 불러오기 (원본에 subsample & noise한 사진)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("testY5n.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// 영상2 불러오기 (원본)
	BITMAPFILEHEADER bmpFile2;
	BITMAPINFOHEADER bmpInfo2;
	FILE* inputFile2 = NULL;
	inputFile2 = fopen("testY5.bmp", "rb");
	fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
	fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

	// 영상1 크기 정보 (256 x 256)
	int width1 = bmpInfo1.biWidth;
	int height1 = bmpInfo1.biHeight;
	int size1 = bmpInfo1.biSizeImage;
	int bitCnt1 = bmpInfo1.biBitCount;
	int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

	// 인풋이미지1 선언
	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

	// 함수 y1 : subsample & noise된 사진
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Upsample Code //

	// Upsample Size (1024 x 1024)
	int ratio = 2; // The scaling factor (2^ratio)
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// Function y2 : Upsample
	unsigned char* y2 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// Bilinear Interpolation
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			// 원본 이미지에 대응하는 좌표
			double origY = j / (double)(1 << ratio);
			double origX = i / (double)(1 << ratio);

			// 좌표 정수 부분
			int x01 = (int)origX;
			int y01 = (int)origY;
			int x02 = x01 + 1;
			int y02 = y01 + 1;
			if (x02 >= width1) x02 = width1 - 1;
			if (y02 >= height1) y02 = height1 - 1;

			// 좌표 소수 부분
			double dx = origX - x01;
			double dy = origY - y01;

			// 4개 픽셀의 가중평균
			double value = (1 - dx) * (1 - dy) * y1[y01 * width1 + x01] +
				dx * (1 - dy) * y1[y01 * width1 + x02] +
				(1 - dx) * dy * y1[y02 * width1 + x01] +
				dx * dy * y1[y02 * width1 + x02];

			y2[j * width2 + i] = (unsigned char)value;
		}

	// Upsample Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Denoise Code //

	// result : Gaussian Filter
	unsigned char* result = calloc(size2, sizeof(unsigned char));

	// Set Gaussian filter Size
	double kernel[9][9];
	int kernelSize = 9;

	double sum = 0.0;
	double sigma = 1.7; // The optimal Gaussian standard deviation
	int offset = kernelSize / 2;
	// Caculate Gaussian values
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
	
	// Denoise Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PSNR Code //

	// Function inputImg2 (Original.bmp)
	unsigned char* inputImg2 = NULL;
	inputImg2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	fread(inputImg2, sizeof(unsigned char), size2, inputFile2);

	// Function y : Original
	unsigned char* y = NULL;
	y = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y[j * width2 + i] = inputImg2[j * stride2 + 3 * i + 0];

	// PSNR1 (Original - Result)
	double mse = 0, psnr;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse += (double)((y[j * width2 + i] - result[j * width2 + i]) * (y[j * width2 + i] - result[j * width2 + i]));
	mse /= (width2 * height2);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);

	// PSNR Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Function outputImg1
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// outputImg = result
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result[j * width2 + i];
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
	free(y1); // Subsample & Noise
	free(y2); // Apply Bilinear Interpolation
	free(result); // Apply Gaussian Filter

	free(outputImg1);
	fclose(outputFile1);
}
