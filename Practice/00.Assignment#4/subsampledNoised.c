#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	// 영상1 불러오기 (testY.bmp)
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("testY1.bmp", "rb");
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

	// 함수 y1 : testY
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// Subsampling 크기
	int ratio = 2; // 크기를 줄일 배율
	int width2 = bmpInfo1.biWidth >> ratio;
	int height2 = bmpInfo1.biHeight >> ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// 함수 y2 : testYSub (크기 width2*height2)
	unsigned char* y2 = (unsigned char*)calloc(width2 * height2, sizeof(unsigned char));
	
	// subsampling
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			y2[j * width2 + i] = y1[(j << ratio) * width1 + (i << ratio)];

	// 함수 y3 : testYSubNoised (크기 width2*height2)
	unsigned char* y3 = (unsigned char*)calloc(width2 * height2, sizeof(unsigned char));

	// noise
	int Err = 55;
	double error_Y;
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
		{
			error_Y = y2[j * width2 + i];
			error_Y += rand() % Err - (Err >> 1);
			y3[j * width2 + i] = (unsigned char)(error_Y > 255 ? 255 : (error_Y < 0 ? 0 : error_Y));
		}

	// 아웃풋이미지1 선언 (size2)
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size2, sizeof(unsigned char));

	// 아웃풋이미지1 result1 할당
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			outputImg1[j * stride2 + 3 * i + 0] = y3[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 1] = y3[j * width2 + i];
			outputImg1[j * stride2 + 3 * i + 2] = y3[j * width2 + i];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("testYSubNoised1.bmp", "wb");
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

	free(y1);
	free(y2);
	free(y3);

	free(outputImg1);
	fclose(outputFile1);
}
