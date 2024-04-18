#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// 영상1 불러오기 (AICenterY.bmp)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    // 영상2 불러오기 (AICenterY_CombinedNoise.bmp)
	BITMAPFILEHEADER bmpFile2;
    BITMAPINFOHEADER bmpInfo2;
    FILE* inputFile2 = NULL;
    inputFile2 = fopen("AICenterY_CombinedNoise.bmp", "rb");
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
    
    // 인풋이미지2 선언
	unsigned char* inputImg2 = NULL;
	inputImg2 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	fread(inputImg2, sizeof(unsigned char), size1, inputFile2);

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// 함수 y1 : AICenterY
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// 함수 y2 : AICenterY_CombinedNoise
	unsigned char* y2 = NULL;
	y2 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y2[j * width1 + i] = inputImg2[j * stride1 + 3 * i + 0];

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
    unsigned char* result1 = NULL;
	result1 = (unsigned char*)calloc(psize, sizeof(unsigned char));

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

	// PSNR2 (원본 - 필터)
	double mse2 = 0, psnr2;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse2 += (double)((y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]) * (y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]));
	mse2 /= (width1 * height1);
	psnr2 = mse2 != 0.0 ? 10.0 * log10(255 * 255 / mse2) : 99.99;
	printf("[내 필터 PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse2, psnr2);

	// 아웃풋이미지1 result1 할당 (패딩 부분 제외)
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 1] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 2] = result1[(p + j) * pwidth1 + (p + i)];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("Output.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

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
