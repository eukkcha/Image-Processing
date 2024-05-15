#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// 영상1 불러오기 (testYnoise.bmp)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("testYnoise5.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// 영상2 불러오기 (testY.bmp)
	BITMAPFILEHEADER bmpFile2;
	BITMAPINFOHEADER bmpInfo2;
	FILE* inputFile2 = NULL;
	inputFile2 = fopen("testY5.bmp", "rb");
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

	// 함수 y1 : testYnoise
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Upsample Code //

	// Upsample 크기
	int ratio = 2; // 크기 키울 배율 (2의 ratio제곱)
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 y2 : Upsample
	unsigned char* y2 = (unsigned char*)calloc(size2, sizeof(unsigned char));

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

			y2[j * width2 + i] = (unsigned char)value;
		}

	// Upsample Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Denoise Code //

	// Padding 크기
	int p = 1; // Padding의 폭
	int pwidth = width2 + 2 * p; 
	int pheight = height2 + 2 * p;
	int psize = pwidth * pheight * 3;

	// 함수 y3 : Padding & Median
	unsigned char* y3 = (unsigned char*)calloc(psize, sizeof(unsigned char));

	// Paste y2
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y3[(p + j) * pwidth + (p + i)] = y2[j * width2 + i];

	// Mirror Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width2; i++) {
			y3[(p - 1 - j) * pwidth + p + i] = y2[(j + 1) * width2 + i]; // 상단
			y3[(pheight - p + j) * pwidth + p + i] = y2[(height2 - 2 - j) * width2 + i]; // 하단
		}
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < p; i++) {
			y3[(p + j) * pwidth + p - 1 - i] = y2[j * width2 + 1 + i]; // 좌측
			y3[(p + j) * pwidth + (pwidth - p + i)] = y2[j * width2 + width2 - 2 - i]; // 우측
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			y3[(p - 1 - j) * pwidth + (p - 1 - i)] = y2[(1 + j) * width2 + (1 + i)]; // 좌상단
			y3[(p - 1 - j) * pwidth + pwidth - p + i] = y2[(1 + j) * width2 + width2 - 2 - i]; // 우상단
			y3[(pheight - p + j) * pwidth + p - 1 - i] = y2[(height2 - 2 - j) * width2 + 1 + i]; // 좌하단
			y3[(pheight - p + j) * pwidth + pwidth - p + i] = y2[(height2 - 2 - j) * width2 + width2 - 2 - i]; // 우하단
		}

	// Median Filter
	int len = 9;
	int value[9];
	for (int j = p; j < p + height2; j++)
		for (int i = p; i < p + width2; i++)
		{
			int F, k = 0;
			for (int m = 0; m < 3; m++) 
				for (int n = 0; n < 3; n++) 
				{
					value[k] = y3[(j - p + m) * pwidth + (i - p + n)];
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

			y3[j * pwidth + i] = F;
		}

	// Mean Filter 3x3
	double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9} };

	// 함수 y4 : Mean 
	unsigned char* y4 = (unsigned char*)calloc(psize, sizeof(unsigned char));

	// Convolution (Mean3F)
	for (int j = p; j < p + height2; j++)
		for (int i = p; i < p + width2; i++) 
		{
			int F = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					F += y3[(j - p + m) * pwidth + (i - p + n)] * Mean3F[m][n];
			y4[j * pwidth + i] = F;
		}

	// Denoise Code //
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

	// PSNR1 (원본 - upsample)
	double mse = 0, psnr;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse += (double)((y[j * width2 + i] - y2[j * width2 + i]) * (y[j * width2 + i] - y2[j * width2 + i]));
	mse /= (width2 * height2);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);

	// PSNR2 (원본 - denoise)
	double mse2 = 0, psnr2;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse2 += (double)((y[j * width2 + i] - y4[(p + j) * pwidth + (p + i)]) * (y[j * width2 + i] - y4[(p + j) * pwidth + (p + i)]));
	mse2 /= (width2 * height2);
	psnr2 = mse2 != 0.0 ? 10.0 * log10(255 * 255 / mse2) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse2, psnr2);

	// PSNR Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	//// 아웃풋이미지1 y2 할당 
	//for (int j = 0; j < height2; j++)
	//	for (int i = 0; i < width2; i++) {
	//		outputImg1[j * stride2 + 3 * i + 0] = y2[j * width2 + i];
	//		outputImg1[j * stride2 + 3 * i + 1] = y2[j * width2 + i];
	//		outputImg1[j * stride2 + 3 * i + 2] = y2[j * width2 + i];
	//	}

	// 아웃풋이미지1 y4 할당 (패딩 부분 제외)
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = y4[(p + j) * pwidth + (p + i)];
			outputImg1[j * stride2 + 3 * i + 1] = y4[(p + j) * pwidth + (p + i)];
			outputImg1[j * stride2 + 3 * i + 2] = y4[(p + j) * pwidth + (p + i)];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("testY5_2.bmp", "wb");
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

	free(y);  // 원본
	free(y1); // Subsample & Noise
	free(y2); // Bilinear Upsample
	free(y3); // Mirror Padding & Median Filter (Best?)
	free(y4); // Mean Filter

	free(outputImg1);
	fclose(outputFile1);
}
