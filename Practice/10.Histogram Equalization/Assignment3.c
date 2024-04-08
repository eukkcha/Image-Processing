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
	inputFile1 = fopen("DarkY.bmp.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	//영상 크기 정보 (영상1 기준)
	int width = bmpInfo1.biWidth;
	int height = bmpInfo1.biHeight;
	int size = bmpInfo1.biSizeImage;
	int bitCnt = bmpInfo1.biBitCount;
	int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);

	//영상1 인풋이미지 선언 (Dark영상)
	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size, inputFile1);

	//아웃풋이미지 선언 : dark영상 Histogram, HE영상, HE영상의 Histogram
	unsigned char* outputImg1 = NULL, * outputImg2 = NULL, * outputImg3 = NULL;
	outputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
	outputImg3 = (unsigned char*)calloc(size, sizeof(unsigned char));

	//함수 dark 선언 및 영상1 할당 (Dark영상)
	unsigned char* dark = NULL;
	dark = (unsigned char*)calloc(size, sizeof(unsigned char));
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			dark[j * width + i] = inputImg1[j * stride + 3 * i + 0];

	// 1. Dark영상의 Histogrm
	//Dark영상 Histogram을 저장할 함수 histo 선언
	unsigned char* histo = NULL;
	histo = (unsigned char*)calloc(size, sizeof(unsigned char));

	//Dark영상의 화소값 분포 (distribution)
	int distr1[256] = { 0 };
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			distr1[dark[j * width + i]]++;

	//분포값 20으로 나누기
	for (int i = 0; i < 256; i++)
		distr1[i] /= 20;

	//흰색바탕
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			histo[j * width + i] = 255;

	//분포값만큼 쌓아올리기
	int k = 0;
	for (int i = 0; i < width; i += 2)
	{
		for (int j = 0; j < distr1[k]; j++)
		{
			histo[j * width + i] = 0;
			histo[j * width + i + 1] = 0;
		}
		k++;
	}

	//outputImg1에 histo 입력
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++) {
			outputImg1[j * stride + 3 * i + 0] = histo[j * width + i];
			outputImg1[j * stride + 3 * i + 1] = histo[j * width + i];
			outputImg1[j * stride + 3 * i + 2] = histo[j * width + i];
		}


	// 2. Histogrm Equalization영상
	//HE영상을 저장할 함수 HE 선언
	unsigned char* HE = NULL;
	HE = (unsigned char*)calloc(size, sizeof(unsigned char));

	//Dark영상의 화소값 분포(distribution)
	int distr2[256] = { 0 };
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			distr2[dark[j * width + i]]++;

	//Cumulative Histogram
	for (int i = 1; i < 256; i++)
		distr2[i] += distr2[i - 1];
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			HE[j * width + i] = 255 * distr2[dark[j * width + i]] / (width * height);

	//outputImg2에 HE 입력
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++) {
			outputImg2[j * stride + 3 * i + 0] = HE[j * width + i];
			outputImg2[j * stride + 3 * i + 1] = HE[j * width + i];
			outputImg2[j * stride + 3 * i + 2] = HE[j * width + i];
		}


	// 3. Histogrm Equalization영상의 Histogram
	//HE영상 Histogram을 저장할 함수 HEhisto 선언
	unsigned char* HEhisto = NULL;
	HEhisto = (unsigned char*)calloc(size, sizeof(unsigned char));

	//HE영상의 화소값 분포 (distribution)
	int HEdistr[256] = { 0 };
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			HEdistr[HE[j * width + i]]++;

	//분포값 20으로 나누기
	for (int i = 0; i < 256; i++)
		HEdistr[i] /= 20;

	//흰색바탕
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
			HEhisto[j * width + i] = 255;

	//분포값만큼 쌓아올리기
	k = 0;
	for (int i = 0; i < width; i += 2)
	{
		for (int j = 0; j < HEdistr[k]; j++)
		{
			HEhisto[j * width + i] = 0;
			HEhisto[j * width + i + 1] = 0;
		}
		k++;
	}

	//outputImg3에 HEhisto 입력
	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++) {
			outputImg3[j * stride + 3 * i + 0] = HEhisto[j * width + i];
			outputImg3[j * stride + 3 * i + 1] = HEhisto[j * width + i];
			outputImg3[j * stride + 3 * i + 2] = HEhisto[j * width + i];
		}


	//outputImg 파일 추출
	FILE* outputFile1 = fopen("histogram.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size, outputFile1);

	FILE* outputFile2 = fopen("HE.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile2);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile2);
	fwrite(outputImg2, sizeof(unsigned char), size, outputFile2);

	FILE* outputFile3 = fopen("HE_histogram.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile3);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile3);
	fwrite(outputImg3, sizeof(unsigned char), size, outputFile3);

	//사용한 메모리 할당 해제
	free(inputImg1);
	fclose(inputFile1);

	free(dark);
	free(histo);
	free(HE);
	free(HEhisto);

	free(outputImg1);
	free(outputImg2);
	free(outputImg3);
	fclose(outputFile1);
	fclose(outputFile2);
	fclose(outputFile3);
}
