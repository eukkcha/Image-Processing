#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	//영상1 헤더파일 선언
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("originalY.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	//영상 크기 정보 (영상1 기준)
	int width = bmpInfo1.biWidth;
	int height = bmpInfo1.biHeight;
	int size = bmpInfo1.biSizeImage;
	int bitCnt = bmpInfo1.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);

	//영상1 인/아웃풋이미지 선언
	unsigned char* inputImg1 = NULL, * outputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size, inputFile1);

	//함수 y1 선언
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size, sizeof(unsigned char));

	//y1에 영상1 할당
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++) {
			y1[j * width + i] = inputImg1[j * stride + 3 * i + 0]; //원본 이미지
		}

	//픽셀값0~256 분포도 저장
	int histogram[256] = { 0 };
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			histogram[y1[j * width + i]]++;

	//분포값 10으로 나누기
	for (int k = 0; k < 256; k++)
		histogram[k] /= 15;

	//결과물 저장할 함수 result 선언
	unsigned char* result = NULL;
	result = (unsigned char*)calloc(size, sizeof(unsigned char));

	//흰색바탕 넣기
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			result[j * width + i] = 255;

	//분포값만큼 쌓아올리기
	int k = 0;
	for (int i = 0; i < width; i+=2)
	{
		for (int j = 0; j < histogram[k]; j++)
		{
			result[j * width + i] = 0;
			result[j * width + i + 1] = 0;
		}
		k++;
	}

	//outputImg에 result 입력
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++) {
			outputImg1[j * stride + 3 * i + 0] = result[j * width + i];
			outputImg1[j * stride + 3 * i + 1] = result[j * width + i];
			outputImg1[j * stride + 3 * i + 2] = result[j * width + i];
		}

	//outputImg 파일 추출
	FILE* outputFile1 = fopen("output.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size, outputFile1);

	//사용한 메모리 할당 해제
	free(inputImg1);
	fclose(inputFile1);
	free(y1);

	free(outputImg1);
	fclose(outputFile1);
	free(result);
}
