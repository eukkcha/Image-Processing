#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Upsample(y2) Code //

	// Upsample 크기
	int ratio = 2; // 크기를 키울 배율 (2의 ratio제곱)
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

	// Upsampling Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ED(y3) Code //

	double sobelfilterx[3][3] = { {-1, 0, 1},
							      {-2, 0, 2},
							      {-1, 0, 1} };
	double sobelfiltery[3][3] = { {1, 2, 1},
								  {0, 0, 0},
								  {-1, -2, -1} };

	// 함수 Gx : Gx 결과물
	unsigned char* Gx = NULL;
	Gx = (unsigned char*)calloc(size2, sizeof(unsigned char));

	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			Gx[j * width2 + i] = 0;
	for (int j = 1; j < height2 - 1; j++)
		for (int i = 1; i < width2 - 1; i++)
		{
			double F = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					F += y2[(j - 1 + m) * width2 + (i - 1 + n)] * sobelfilterx[m][n];

			F = sqrt(F * F);

			if (F < 0)
				F = 0;
			else if (F > 255)
				F = 255;

			Gx[j * width2 + i] = F;
		}

	// 함수 Gy : Gy 결과물
	unsigned char* Gy = NULL;
	Gy = (unsigned char*)calloc(size2, sizeof(unsigned char));

	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			Gy[j * width2 + i] = 0;
	for (int j = 1; j < height2 - 1; j++)
		for (int i = 1; i < width2 - 1; i++)
		{
			double F = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					F += y2[(j - 1 + m) * width2 + (i - 1 + n)] * sobelfiltery[m][n];

			F = sqrt(F * F);

			if (F < 0)
				F = 0;
			else if (F > 255)
				F = 255;

			Gy[j * width2 + i] = F;
		}

	// 함수 y3 : ED & Threshold
	unsigned char* y3 = NULL;
	y3 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			double G = sqrt(Gx[j * width2 + i] * Gx[j * width2 + i] + Gy[j * width2 + i] * Gy[j * width2 + i]);

			// Thresholding
			if (G >= 200)
				y3[j * width2 + i] = 255;
			else 
				y3[j * width2 + i] = 0;
		}

	// ED(y3) Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Denoise(y4) Code //

	// Filter
	double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9} };

	// 함수 y4 : Denoise
	unsigned char* y4 = NULL;
	y4 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 엣지 부분 mask
	// y2에서, 0인 부분은 1로 만들기
	// y3에서 255인 좌표는 y3에서 0으로 mask 하기

	// Convolution (mask, 즉 0인 부분은 제외)
	for (int j = 1; j < height2 - 1; j++)
		for (int i = 1; i < width2 - 1; i++)
		{
			int F = 0;
			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					F += y2[(j - 1 + m) * width2 + (i - 1 + n)] * Mean3F[m][n];
			y4[j * width2 + i] = F;
		}

	// Denoise(y4) Code //
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

	// PSNR (original - result)
	double mse = 0, psnr;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			mse += (double)((y[j * width2 + i] - y4[j * width2 + i]) * (y[j * width2 + i] - y4[j * width2 + i]));
	mse /= (width2 * height2);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);
	// test1 Nni : 21.76 // Bi : 24.94 // Ntap2 : 24.16 // Ntap4 : 24.56 // Ntap6 : 21.43 // Ntap8 : 20.32
	// test2 Nni : 19.82 // Bi : 22.82 // Ntap2 : 21.13 // Ntap4 : 21.99 // Ntap6 : 18.48
	// test3 Nni : 14.73 // Bi : 17.48 // Ntap4 : 16.49 // Ntap6 : 13.63
	// test4 Nni : 18.66 // Ntap4 : 20.57 // Ntap6 : 17.58
	// Bi > Ntap4 > Ntap2

	// PSNR Code //
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 아웃풋이미지1 result1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = y4[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = y4[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = y4[j * width2 + i];
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

	free(y); // 원본
	free(y1); // Subsample & Noise
	free(y2); // Upsample
	free(y3); // ED & Threshold
	free(y4); // Denoise

	free(outputImg1);
	fclose(outputFile1);
}
