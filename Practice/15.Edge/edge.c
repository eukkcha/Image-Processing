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

	// 함수 y1 : 원본
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// 함수 Gx : Gx 결과물
	unsigned char* Gx = NULL;
	Gx = (unsigned char*)calloc(size1, sizeof(unsigned char));

	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			Gx[j * width1 + i] = 0;
	for (int j = 0; j < height1; j++)
	{
		for (int i = 1; i < width1 - 1; i++)
		{
			double dev = (y1[j * width1 + i + 1] - y1[j * width1 + i - 1]) / 2;
			dev = sqrt(dev * dev);
			Gx[j * width1 + i] = dev;
		}
	}

	// 함수 Gy : Gy 결과물
	unsigned char* Gy = NULL;
	Gy = (unsigned char*)calloc(size1, sizeof(unsigned char));

	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			Gy[j * width1 + i] = 0;
	for (int j = 1; j < height1 - 1; j++)
	{
		for (int i = 0; i < width1; i++)
		{
			double dev = (y1[(j + 1) * width1 + i] - y1[(j - 1) * width1 + i]) / 2;
			dev = sqrt(dev * dev);


			Gy[j * width1 + i] = dev;
		}
	}

	// 함수 result1 : G 결과물
	unsigned char* result1 = NULL;
	result1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	for (int j = 0; j < height1; j++)
	{
		for (int i = 0; i < width1; i++)
		{
			double G = sqrt(Gx[j * width1 + i] * Gx[j * width1 + i] + Gy[j * width1 + i] * Gy[j * width1 + i]);
			result1[j * width1 + i] = (unsigned char)(G > 255 ? 255 : (G < 0 ? 0 : G));
		}
	}

	// 아웃풋이미지1 선언
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// 아웃풋이미지1 result1 할당
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = result1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 1] = result1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 2] = result1[j * width1 + i];
		}

	// 아웃풋이미지 파일1
	FILE* outputFile1 = fopen("edge.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

	// 메모리 할당 해제
	free(inputImg1);
	fclose(inputFile1);

	free(y1);
	free(Gx);
	//free(Gy);
	//free(result1);

	free(outputImg1);
	fclose(outputFile1);
}
