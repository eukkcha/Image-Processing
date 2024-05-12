#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable:4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // 영상1 불러오기 (test.bmp)
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE* inputFile1 = NULL;
    inputFile1 = fopen("test1.bmp", "rb");
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

    // 아웃풋이미지1 선언
    unsigned char* outputImg1 = NULL;
    outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

    //RGB -> YYY
    int Y;
    for (int j = 0; j < height1; j++)
    {
        for (int i = 0; i < width1; i++)
        {
            Y = 0.299 * inputImg1[j * stride1 + 3 * i + 2] + 0.587 * inputImg1[j * stride1 + 3 * i + 1] + 0.114 * inputImg1[j * stride1 + 3 * i + 0];
            outputImg1[j * stride1 + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
            outputImg1[j * stride1 + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
            outputImg1[j * stride1 + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
        }
    }

    //outputImg 파일 추출
    FILE* outputFile1 = fopen("testY1.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

    //사용한 메모리 할당 해제
    free(inputImg1);
    fclose(inputFile1);

    free(outputImg1);
    fclose(outputFile1);
}
