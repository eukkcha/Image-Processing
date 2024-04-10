#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

//알고리즘 내용
//PSNR

int main(int argc, char* argv[])
{
    //영상1 헤더파일 선언
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE* inputFile1 = NULL;
    inputFile1 = fopen("AICenterY.bmp", "rb");
    fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
    fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    //영상2 헤더파일 선언
    BITMAPFILEHEADER bmpFile2;
    BITMAPINFOHEADER bmpInfo2;
    FILE* inputFile2 = NULL;
    inputFile2 = fopen("AICenterY_Noise.bmp", "rb");
    fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
    fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

    //영상 크기 정보 (영상1 기준)
    int width = bmpInfo1.biWidth;
    int height = bmpInfo1.biHeight;
    int size = bmpInfo1.biSizeImage;
    int bitCnt = bmpInfo1.biBitCount;
    int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);

    //영상1 인풋이미지 선언
    unsigned char* inputImg1 = NULL;
    inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size, inputFile1);

    //영상2 인풋이미지 선언
    unsigned char* inputImg2 = NULL;
    inputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(inputImg2, sizeof(unsigned char), size, inputFile2);

    //함수 y1 y2 선언
    unsigned char* y1 = NULL, * y2 = NULL;
    y1 = (unsigned char*)calloc(size, sizeof(unsigned char));
    y2 = (unsigned char*)calloc(size, sizeof(unsigned char));

    //y1 y2에 영상1 영상2 할당
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++) {
            y1[j * width + i] = inputImg1[j * stride + 3 * i + 0]; //원본 이미지
            y2[j * width + i] = inputImg2[j * stride + 3 * i + 0]; //노이즈 이미지
        }

    //원본 - 노이즈 PSNR (y1 - y2)
    double mse = 0, psnr;
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
            mse += (double)((y1[j * width + i] - y2[j * width + i]) * (y1[j * width + i] - y2[j * width + i]));
    mse /= (width * height);
    psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
    printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse, psnr);

    //사용한 메모리 할당 해제
    free(inputImg1);
    fclose(inputFile1);
    free(y1);

    free(inputImg2);
    fclose(inputFile2);
    free(y2);
}
