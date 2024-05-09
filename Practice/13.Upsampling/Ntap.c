#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
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

	// Nearest Neighbor Interpolation
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++) {
			int origY = j / (1 << ratio);
			int origX = i / (1 << ratio);

			result1[j * width2 + i] = y1[origY * width1 + origX];
		}

    
    // Apply N-tap Interpolation
    for (int j = 0; j < height2; j++)
        for (int i = 0; i < width2; i++) 
        {
            float origY = j / (float)(1 << ratio);
            float origX = i / (float)(1 << ratio);

            int Value = 0;

            for (int k = 0; k < 4; k++) 
                for (int l = 0; l < 4; l++) 
                {
                    int idxY = origY - 1 + k;
                    if (idxY < 0) idxY = 0;
                    if (idxY >= height1) idxY = height1 - 1;
                    int idxX = origX - 1 + l;
                    if (idxX < 0) idxX = 0;
                    if (idxX >= width1) idxX = width1 - 1;

                    Value += y1[idxY * width1 + idxX] * filter[k] * filter[l];
                }

            Value /= (filterSum * filterSum);
            if (Value < 0) Value = 0;
            if (Value > 255) Value = 255;

            result1[j * width2 + i] = Value;
        }

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

	free(y1);
	free(result1);

	free(outputImg1);
	fclose(outputFile1);
}
