#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	/////////////////////////////////////////////////////////////////
	// 기본 코드


	// 영상1 불러오기
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("original.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

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

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// 함수 y1 : original
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// 아웃풋이미지1 y1 할당
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = y1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 1] = y1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 2] = y1[j * width1 + i];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("output.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

	// 메모리 할당 해제
	free(inputImg1);
	fclose(inputFile1);

	free(y1);

	free(outputImg1);
	fclose(outputFile1);


	/////////////////////////////////////////////////////////////////
	// AICenter Format


	// 영상1 불러오기 (AICenter.bmp)
	// 영상1 크기 정보
	// 인풋이미지1 선언
	// 아웃풋이미지1 선언

	// 알고리즘
	int Y;
	for (int j = 0; j < height1; j++)
	{
		for (int i = 0; i < width1; i++)
		{
			// 0. Image Format
			outputImg1[j * stride1 + 3 * i + 0] = inputImg1[j * stride1 + 3 * i + 0];
			outputImg1[j * stride1 + 3 * i + 1] = inputImg1[j * stride1 + 3 * i + 1];
			outputImg1[j * stride1 + 3 * i + 2] = inputImg1[j * stride1 + 3 * i + 2];

			// 1. Experiment
			if (j < 100 && i < 100)
			{
				outputImg1[j * stride1 + 3 * i + 0] = 0;
				outputImg1[j * stride1 + 3 * i + 1] = 0;
				outputImg1[j * stride1 + 3 * i + 2] = 0;
			}
			if (j > 400 && i > 400)
			{
				outputImg1[j * stride1 + 3 * i + 0] = 255;
				outputImg1[j * stride1 + 3 * i + 1] = 255;
				outputImg1[j * stride1 + 3 * i + 2] = 255;
			}

			// 2. RGB Filter
			outputImg1[j * stride1 + 3 * i + 0] = 0;
			outputImg1[j * stride1 + 3 * i + 1] = 0;
			outputImg1[j * stride1 + 3 * i + 2] = inputImg1[j * stride1 + 3 * i + 2];

			// 3. YYY Format
			Y = 0.299 * outputImg1[j * stride1 + 3 * i + 2] + 0.587 * outputImg1[j * stride1 + 3 * i + 1] + 0.114 * outputImg1[j * stride1 + 3 * i + 0];
			outputImg1[j * stride1 + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg1[j * stride1 + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg1[j * stride1 + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
		}
	}

	// 아웃풋이미지 파일1
	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// AICenterY Masking


	// 영상1 불러오기 (AICenterY.bmp)
	// 영상2 불러오기 (Mask.bmp)
	// 영상1 크기 정보
	// 인풋이미지1, 인풋이미지2 선언
	// 아웃풋이미지1, 아웃풋이미지2 선언

	// 함수 y1 : AICenterY
	// 함수 y2 : Mask 

	// 함수 result : AICenter * Mask
	unsigned char* result1 = NULL;
	result1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	unsigned char* result2 = NULL;
	result2 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// 알고리즘
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y2[j * width1 + i] == 0) // 0인 부분
				y2[j * width1 + i] = y2[j * width1 + i] + 1; // y1값을 그대로 넣기 위해 1로 만들기
			else
				y2[j * width1 + i] = y2[j * width1 + i] - 255; // 255인 부분은 0으로 만들기
			// result1에는 y2가 1인 부분은 y1값이, y2가 0인 부분은 0값이 입력됨
			result1[j * width1 + i] = y1[j * width1 + i] * y2[j * width1 + i];
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y2[j * width1 + i] == 1) // 1로 만들었던 부분
				y2[j * width1 + i] = y2[j * width1 + i] - 1; // 0으로 만들기
			else // 0으로 만들었던 부분
				y2[j * width1 + i] = y2[j * width1 + i] + 1; // y1값을 그대로 넣기 위해 1로 만들기
			// result2에는 y2가 0인 부분은 0값이, y2가 1인 부분은 y1값이 입력됨
			result2[j * width1 + i] = y1[j * width1 + i] * y2[j * width1 + i];
		}

	// 아웃풋이미지1/2 result1/2 할당
	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// PSNR


	// 영상1 불러오기 (AICenterY.bmp)
	// 영상2 불러오기 (AICenterY_Noise.bmp)
	// 영상1 크기 정보
	// 인풋이미지1, 인풋이미지2 선언
	// 아웃풋이미지1, 아웃풋이미지2 선언

	// 함수 y1 : AICenterY
	// 함수 y2 : AICenterY_Noise

	// PSNR1 (원본 - 노이즈)
	double mse1 = 0, psnr1;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse1 += (double)((y1[j * width1 + i] - y2[j * width1 + i]) * (y1[j * width1 + i] - y2[j * width1 + i]));
	mse1 /= (width1 * height1);
	psnr1 = mse1 != 0.0 ? 10.0 * log10(255 * 255 / mse1) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse1, psnr1);


	/////////////////////////////////////////////////////////////////
	// Filters


	// 영상1 불러오기 (AICenterY.bmp)
	// 영상2 불러오기 (AICenterY_CombinedNoise.bmp)
	// 영상1 크기 정보
	// 인풋이미지1, 인풋이미지2 선언
	// 아웃풋이미지1 선언

	// 함수 y1 : AICenterY
	// 함수 y2 : AICenterY_CombinedNoise  

	// Filter Design 선언
	// Gaussian Filter 5x5
	double Gauss5F[5][5] = { {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273},
							 {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
							 {(double)7 / 273, (double)26 / 273, (double)41 / 273, (double)26 / 273, (double)7 / 273},
							 {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
							 {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273} };
	// Gaussian Filter 3x3
	double Gauss3F[3][3] = { {(double)1 / 16, (double)2 / 16, (double)1 / 16},
							 {(double)2 / 16, (double)4 / 16, (double)2 / 16},
							 {(double)1 / 16, (double)2 / 16, (double)1 / 16} };
	// Mean Filter 3x3
	double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9} };

	// Same Padding
	int p = (3 - 1) / 2; // Same Padding의 폭 = ((Filter의 폭) - 1) / 2
	int pwidth1 = width1 + 2 * p; // Same Padding을 적용한 데이터의 크기 : (height + 2p, width + 2p)
	int pheight1 = height1 + 2 * p;
	int psize1 = pwidth1 * pheight1 * 3;

	// 함수 result1 : 필터 결과값 (크기 psize)

	// 1. Zero Padding
	for (int j = 0; j < pheight1; j++)
		for (int i = 0; i < pwidth1; i++)
			result1[j * pwidth1 + i] = 0;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			result1[(p + j) * pwidth1 + (p + i)] = y2[j * width1 + i];

	// 2. Replicate Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width1; i++) {
			result1[j * pwidth1 + p + i] = y2[i]; // 상단
			result1[(pheight1 - 1 - j) * pwidth1 + p + i] = y2[(height1 - 1) * width1 + i]; // 하단
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < p; i++) {
			result1[(p + j) * pwidth1 + i] = y2[j * width1]; // 좌측
			result1[(p + j) * pwidth1 + (pwidth1 - 1 - i)] = y2[j * width1 + width1 - 1]; // 우측
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			result1[j * pwidth1 + i] = y2[0]; // 좌상단
			result1[j * pwidth1 + pwidth1 - 1 - i] = y2[width1 - 1]; // 우상단
			result1[(pheight1 - 1 - j) * pwidth1 + i] = y2[(height1 - 1) * width1]; // 좌하단
			result1[(pheight1 - 1 - j) * pwidth1 + pwidth1 - 1 - i] = y2[(height1 - 1) * width1 + width1 - 1]; // 우하단
		}

	// 3. Mirror Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width1; i++) {
			result1[(p - 1 - j) * pwidth1 + p + i] = y2[(j + 1) * width1 + i]; // 상단
			result1[(pheight1 - p + j) * pwidth1 + p + i] = y2[(height1 - 2 - j) * width1 + i]; // 하단
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < p; i++) {
			result1[(p + j) * pwidth1 + p - 1 - i] = y2[j * width1 + 1 + i]; // 좌측
			result1[(p + j) * pwidth1 + (pwidth1 - p + i)] = y2[j * width1 + width1 - 2 - i]; // 우측
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			result1[(p - 1 - j) * pwidth1 + (p - 1 - i)] = y2[(1 + j) * width1 + (1 + i)]; // 좌상단
			result1[(p - 1 - j) * pwidth1 + pwidth1 - p + i] = y2[(1 + j) * width1 + width1 - 2 - i]; // 우상단
			result1[(pheight1 - p + j) * pwidth1 + p - 1 - i] = y2[(height1 - 2 - j) * width1 + 1 + i]; // 좌하단
			result1[(pheight1 - p + j) * pwidth1 + pwidth1 - p + i] = y2[(height1 - 2 - j) * width1 + width1 - 2 - i]; // 우하단
		}

	// Median Filter
	int len = 9;
	int value[9];
	for (int j = p; j < p + height1; j++)
		for (int i = p; i < p + width1; i++)
		{
			int F, k = 0;
			for (int m = 0; m < 3; m++) // 3x3 Filter (m,n) = (3,3)
				for (int n = 0; n < 3; n++)
				{
					value[k] = result1[(j - p + m) * pwidth1 + (i - p + n)];
					k++;
				}

			// 중앙값 구하기
			int m, n, tmp = 0;
			for (m = 0; m < len; m++)
				for (n = 0; n < (len - 1) - m; n++)
					if (value[n] > value[n + 1])
					{
						tmp = value[n];
						value[n] = value[n + 1];
						value[n + 1] = tmp;
					}
			F = value[len / 2];

			result1[j * pwidth1 + i] = F;
		}

	// Convolution
	for (int j = p; j < p + height1; j++)
		for (int i = p; i < p + width1; i++) {
			int F = 0;
			for (int m = 0; m < 3; m++) // 3x3 Filter (m,n) = (3,3)
				for (int n = 0; n < 3; n++)
					F += result1[(j - p + m) * pwidth1 + (i - p + n)] * Gauss3F[m][n]; // Gauss3F
			result1[j * pwidth1 + i] = F;
		}

	// PSNR1 (원본 - 노이즈)
	double mse1 = 0, psnr1;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse1 += (double)((y1[j * width1 + i] - y2[j * width1 + i]) * (y1[j * width1 + i] - y2[j * width1 + i]));
	mse1 /= (width1 * height1);
	psnr1 = mse1 != 0.0 ? 10.0 * log10(255 * 255 / mse1) : 99.99;
	printf("[기존 노이즈 PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse1, psnr1);
	//18.13dB

	// PSNR2 (원본 - 필터)
	double mse2 = 0, psnr2;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse2 += (double)((y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]) * (y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]));
	mse2 /= (width1 * height1);
	psnr2 = mse2 != 0.0 ? 10.0 * log10(255 * 255 / mse2) : 99.99;
	printf("[내 필터 PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse2, psnr2);
	//Gaussian 3x3 (replicate) : 23.27dB

	// 아웃풋이미지1 result1 할당 (패딩 부분 제외)
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 1] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 2] = result1[(p + j) * pwidth1 + (p + i)];
		}

	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// Histogram, Thresholding


	// 영상1 불러오기 (AICenterY.bmp)
	// 영상1 크기 정보
	// 인풋이미지1 선언
	// 아웃풋이미지1,2 선언

	// 함수 y1 : AICenterY
	// 함수 histo1 : AICenterY 히스토그램
	// 함수 result1 : AICenterY Thresholding

	// y1의 화소값 분포 (distribution)
	int distr1[256] = { 0 };
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			distr1[y1[j * width1 + i]]++;

	// 분포값 10으로 나누기
	for (int k = 0; k < 256; k++)
		distr1[k] /= 15;

	// 흰색바탕
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			histo1[j * width1 + i] = 255;

	// 분포값만큼 쌓아올리기
	int k = 0;
	for (int i = 0; i < width1; i += 2)
	{
		for (int j = 0; j < distr1[k]; j++)
		{
			histo1[j * width1 + i] = 0;
			histo1[j * width1 + i + 1] = 0;
		}
		k++;
	}

	// 아웃풋이미지1 histo1 할당

	// Binary Image (+ Multilevel Threshholding)
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y1[j * width1 + i] > 250) result1[j * width1 + i] = 255;
			else if (y1[j * width1 + i] > 170) result1[j * width1 + i] = 0;
			else result1[j * width1 + i] = 170;
		}

	// 아웃풋이미지2 result1 할당
	// 아웃풋이미지 파일1
	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// HE(Histogram Equalization)


	// 영상1 불러오기 (AICenterY_Dark.bmp)
	// 영상1 크기 정보
	// 인풋이미지1 선언
	// 아웃풋이미지1 선언

	// 함수 y1 : AICenterY_Dark
	// 함수 result1 : AICenterY_Dark HE

	// y1의 화소값 분포
	// Cumulative Histogram
	for (int i = 1; i < 256; i++)
		distr1[i] += distr1[i - 1];
	// Histogram Equalization
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			result1[j * width1 + i] = 255 * distr1[y1[j * width1 + i]] / (width1 * height1);

	// 아웃풋이미지1 result1 할당
	// 아웃풋이미지 파일1
	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// Subsampling


	// 영상1 불러오기 (AICenterY.bmp)
	// 영상1 크기 정보
	// 인풋이미지1 선언

	// 함수 y1 : AICenterY

	// Subsampling 크기
	int ratio = 1; // 크기를 줄일 배율
	int width2 = bmpInfo1.biWidth >> ratio;
	int height2 = bmpInfo1.biHeight >> ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 result1 : Subsampling (크기 width2*height2)
	unsigned char* result1 = (unsigned char*)calloc(width2 * height2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			result1[j * width2 + i] = y1[(j << ratio) * width1 + (i << ratio)];

	// 아웃풋이미지1 선언 (size2)
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 아웃풋이미지1 y1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result1[j * width2 + i];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("Subsampling.bmp", "wb");
	bmpInfo1.biWidth = width2;
	bmpInfo1.biHeight = height2;
	bmpInfo1.biSizeImage = size2;
	bmpFile1.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPFILEHEADER) + size2;
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size2, outputFile1);

	// 메모리 할당 해제


	/////////////////////////////////////////////////////////////////
	// Upsampling


	// 영상1 불러오기 (Subsampling.bmp)
	// 영상1 크기 정보
	// 인풋이미지1 선언
	// 아웃풋이미지1 선언

	// 함수 y1 : Subsampling

	// Upsampling 크기
	int ratio = 1; // 크기를 줄일 배율
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 result1 : Upsampling (크기 size2)
	unsigned char* result1 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			result1[(2 * j) * width2 + (2 * i)] = y1[j * width1 + i];
			result1[(2 * j) * width2 + (2 * i + 1)] = y1[j * width1 + i];
			result1[(2 * j + 1) * width2 + (2 * i)] = y1[j * width1 + i];
			result1[(2 * j + 1) * width2 + (2 * i + 1)] = y1[j * width1 + i];
		}

	// 아웃풋이미지1 y1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = result1[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = result1[j * width2 + i];
		}

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
}

////디버깅 (padding이 잘 되었는지 확인)
//for (int j = 0; j < 5; j++) //좌상단
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //좌하단
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = 0; j < 5; j++) //우상단
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //우하단
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//
//printf("----------\n\n");

//// Noise Generation
// int Err = 55;
// Y += rand() % Err - (Err >> 1);

//// Salt-and-Pepper Noise
// int prob = 10;
// if ((rand() % prob) == 0) Y = 225;
// else if ((rand() % prob) == 1) Y == 0;
// else Y = Y;
