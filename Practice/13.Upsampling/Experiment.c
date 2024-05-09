#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// 영상1 불러오기 (Subsampling.bmp)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("Subsampling.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// 영상2 불러오기 (AICenterY.bmp)
	BITMAPFILEHEADER bmpFile2;
	BITMAPINFOHEADER bmpInfo2;
	FILE* inputFile2 = NULL;
	inputFile2 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
	fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

	// 영상1 크기 정보
	int width1 = bmpInfo1.biWidth;
	int height1 = bmpInfo1.biHeight;
	int size1 = bmpInfo1.biSizeImage;
	int bitCnt1 = bmpInfo1.biBitCount;
	int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

	// 인풋이미지1 선언
	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

	// 함수 y1 : Subsampling
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// Upsampling 크기
	int ratio = 1; // 크기를 키울 배율 (2의 ratio제곱만큼 키움)
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 result1 : Upsampling (크기 size2)
	unsigned char* result1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	//// Nearest Neighbor Interpolation
	//for (int j = 0; j < height2; j++)
	//	for (int i = 0; i < width2; i++) 
	//	{
	//		int origY = j / (1 << ratio);
	//		int origX = i / (1 << ratio);

	//		result1[j * width2 + i] = y1[origY * width1 + origX];
	//	}

	const int filter[4] = { -1, 9, 9, -1 };
	const int filtersum = 16;

	// n-tap interpolation
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			float origy = j / (float)(1 << ratio);
			float origx = i / (float)(1 << ratio);

			int value = 0;

			for (int k = 0; k < 4; k++)
				for (int l = 0; l < 4; l++)
				{
					int idxy = origy - 1 + k;
					if (idxy < 0) idxy = 0;
					if (idxy >= height1) idxy = height1 - 1;
					int idxx = origx - 1 + l;
					if (idxx < 0) idxx = 0;
					if (idxx >= width1) idxx = width1 - 1;

					value += y1[idxy * width1 + idxx] * filter[k] * filter[l];
				}

			value /= (filtersum * filtersum);
			if (value < 0) value = 0;
			if (value > 255) value = 255;

			result1[j * width2 + i] = value;
		}

	// 인풋이미지2 선언
	unsigned char* inputImg2 = NULL;
	inputImg2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	fread(inputImg2, sizeof(unsigned char), size2, inputFile2);

	// 함수 y2 : AICenterY
	unsigned char* y2 = NULL;
	y2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y2[j * width2 + i] = inputImg2[j * stride2 + 3 * i + 0];

	// PSNR (원본 - Upsampling)
	double mse = 0, psnr;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse += (double)((result1[j * width2 + i] - y2[j * width2 + i]) * (result1[j * width2 + i] - y2[j * width2 + i]));
	mse /= (width2 * height2);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 아웃풋이미지1 result1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result1[j * width2 + i];
		}
	// N-Tap(4) : 22.87
	// Nearest Neighbor : 22.54

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("Upsampling.bmp", "wb");
	bmpInfo1.biWidth = width2;
	bmpInfo1.biHeight = height2;
	bmpInfo1.biSizeImage = size2;
	bmpFile1.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + size2;
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size2, outputFile1);

	// 메모리 할당 해제
	free(inputImg1);
	fclose(inputFile1);
	free(inputImg2);
	fclose(inputFile2);

	free(y1);
	free(y2);
	free(result1);

	free(outputImg1);
	fclose(outputFile1);
}
