#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// 영상1 불러오기 (testYSubNoised.bmp)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("testYSubNoised1.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// 영상2 불러오기 (testY.bmp)
	BITMAPFILEHEADER bmpFile2;
	BITMAPINFOHEADER bmpInfo2;
	FILE* inputFile2 = NULL;
	inputFile2 = fopen("testY1.bmp", "rb");
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

	// 함수 y1 : testYSubNoised
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// Upsampling 크기
	int ratio = 2; // 크기를 키울 배율 (2의 ratio제곱)
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 result1 : Upsampling (크기 size2)
	unsigned char* result1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// Bilinear Interpolation
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			// 원본 이미지에 대응하는 좌표
			float origY = j / (float)(1 << ratio);
			float origX = i / (float)(1 << ratio);

			// 좌표 정수 부분
			int x01 = (int)origX;
			int y01 = (int)origY;
			int x02 = x01 + 1;
			int y02 = y01 + 1;
			if (x02 >= width1) x02 = width1 - 1;
			if (y02 >= height1) y02 = height1 - 1;

			// 좌표 소수 부분
			float dx = origX - x01;
			float dy = origY - y01;

			// 4개 픽셀의 가중평균
			float value = (1 - dx) * (1 - dy) * y1[y01 * width1 + x01] +
				dx * (1 - dy) * y1[y01 * width1 + x02] +
				(1 - dx) * dy * y1[y02 * width1 + x01] +
				dx * dy * y1[y02 * width1 + x02];

			result1[j * width2 + i] = (unsigned char)value;
		}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Denoising Code //

	// Filter 3x3
	double Gauss3F[3][3] = { {(double)1 / 16, (double)2 / 16, (double)1 / 16},
							  {(double)2 / 16, (double)4 / 16, (double)2 / 16},
							  {(double)1 / 16, (double)2 / 16, (double)1 / 16} };
	double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9} };

	// 함수 result : 최종 결과치
	unsigned char* result = NULL;
	result = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// Convolution
	for (int j = 1; j < height2 - 1; j++)
		for (int i = 1; i < width2 - 1; i++)
		{
			int F = 0;
			for (int m = 0; m < 3; m++) // 3x3 Filter (m,n) = (3,3)
				for (int n = 0; n < 3; n++)
					F += result1[(j - 1 + m) * width2 + (i - 1 + n)] * Mean3F[m][n]; // 3F
			result[j * width2 + i] = F;
		}

	// Denoising Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PSNR Code //

	// 인풋이미지2 선언
	unsigned char* inputImg2 = NULL;
	inputImg2 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	fread(inputImg2, sizeof(unsigned char), size2, inputFile2);

	// 함수 y : testY (Original)
	unsigned char* y = NULL;
	y = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y[j * width2 + i] = inputImg2[j * stride2 + 3 * i + 0];

	// PSNR (Original - Upsampling)
	double mse = 0, psnr;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse += (double)((result[j * width2 + i] - y[j * width2 + i]) * (result[j * width2 + i] - y[j * width2 + i]));
	mse /= (width2 * height2);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);
	// test1 Nni : 21.76 // Bi : 24.94 // Ntap2 : 24.16 // Ntap4 : 24.56 // Ntap6 : 21.43 // Ntap8 : 20.32
	// test2 Nni : 19.82 // Bi : 22.82 // Ntap2 : 21.13 // Ntap4 : 21.99 // Ntap6 : 18.48
	// test3 Nni : 14.73 // Bi : 17.48 // Ntap4 : 16.49 // Ntap6 : 13.63
	// test4 Nni : 18.66 // Ntap4 : 20.57 // Ntap6 : 17.58

	// PSNR Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 아웃풋이미지1 result1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result[j * width2 + i];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("testYBi1.bmp", "wb");
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
	free(y); // 원본
	free(result1);
	free(result);

	free(outputImg1);
	fclose(outputFile1);
}
