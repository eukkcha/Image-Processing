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
    inputFile1 = fopen("original.bmp", "rb");
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

    //RGB Images
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            //red
            outputImg1[j * stride + 3 * i + 0] = 0;
            outputImg1[j * stride + 3 * i + 1] = 0;
            outputImg1[j * stride + 3 * i + 2] = inputImg1[j * stride + 3 * i + 2];

            ////green
            //outputImg1[j * stride + 3 * i + 0] = 0;
            //outputImg1[j * stride + 3 * i + 1] = inputImg1[j * stride + 3 * i + 1];
            //outputImg1[j * stride + 3 * i + 2] = 0;

            ////blue
            //outputImg1[j * stride + 3 * i + 0] = inputImg1[j * stride + 3 * i + 0];
            //outputImg1[j * stride + 3 * i + 1] = 0;
            //outputImg1[j * stride + 3 * i + 2] = 0;
        }
    }

    //outputImg 파일 추출
    FILE* outputFile1 = fopen("output.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size, outputFile1);

    //사용한 메모리 할당 해제
    free(inputImg1);
    fclose(inputFile1);
    free(outputImg1);
    fclose(outputFile1);
}
