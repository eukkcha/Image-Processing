#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>

int main(int argc, char* argv[])
{
    //파일1 헤더파일 선언
    BITMAPFILEHEADER bmpFile1;
    BITMAPINFOHEADER bmpInfo1;
    FILE* inputFile1 = NULL;
    inputFile1 = fopen("AICenterY.bmp", "rb");
    fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
    fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

    //파일2 헤더파일 선언
    BITMAPFILEHEADER bmpFile2;
    BITMAPINFOHEADER bmpInfo2;
    FILE* inputFile2 = NULL;
    inputFile2 = fopen("mask.bmp", "rb");
    fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
    fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

    //이미지 크기 정보 (파일1의 크기)
    int width = bmpInfo1.biWidth;
    int height = bmpInfo1.biHeight;
    int size = bmpInfo1.biSizeImage;
    int bitCnt = bmpInfo1.biBitCount;
    int stride = (((bitCnt / 8) * width) + 3 / 4 * 4);
    printf("W: %d(%d)\nH: %d\nS: %d\nD: %d\n\n", width, stride, height, size, bitCnt);

    //파일1 인/아웃풋 이미지 선언
    unsigned char* inputImg1 = NULL, * outputImg1 = NULL;
    inputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
    outputImg1 = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size, inputFile1);

    //파일2 인/아웃풋 이미지 선언
    unsigned char* inputImg2 = NULL, * outputImg2 = NULL;
    inputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
    outputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
    fread(inputImg2, sizeof(unsigned char), size, inputFile2);

    //파일1, 파일2에 대응하는 함수 y1 y2 선언
    unsigned char* y1 = NULL, * y2 = NULL;
    y1 = (unsigned char*)calloc(size, sizeof(unsigned char));
    y2 = (unsigned char*)calloc(size, sizeof(unsigned char));
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++) {
            y1[j * width + i] = inputImg1[j * stride + 3 * i + 0];
            y2[j * width + i] = inputImg2[j * stride + 3 * i + 0];
        }

    //알고리즘
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            if (y2[j * width + i] == 0) //검은색 부분
                y2[j * width + i] = y2[j * width + i] + 1; //곱했을때 변하지 않도록 1만 더해주기
            else
                y2[j * width + i] = y2[j * width + i] - 255; //흰색이었던 부분은 검은색으로 만들어주기

            outputImg1[j * stride + 3 * i + 0] = y1[j * width + i] * y2[j * width + i];
            outputImg1[j * stride + 3 * i + 1] = y1[j * width + i] * y2[j * width + i];
            outputImg1[j * stride + 3 * i + 2] = y1[j * width + i] * y2[j * width + i];
        }
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            if (y2[j * width + i] == 1) //검은색 부분 (이전 알고리즘에서 0일 때 +1했던 부분)
                y2[j * width + i] = y2[j * width + i] - 1; //다시 0으로 만들어 검은색으로 만들기
            else //검은색 부분 (이전 알고리즘에서 흰색일 때 -255해서 검은색으로 만든 부분)
                y2[j * width + i] = y2[j * width + i] + 1; //다시 곱했을때 변하지않도록 1만 더해주기

            outputImg2[j * stride + 3 * i + 0] = y1[j * width + i] * y2[j * width + i];
            outputImg2[j * stride + 3 * i + 1] = y1[j * width + i] * y2[j * width + i];
            outputImg2[j * stride + 3 * i + 2] = y1[j * width + i] * y2[j * width + i];
        }

    //영상 파일 추출
    FILE* outputFile1 = fopen("Output1.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size, outputFile1);

    FILE* outputFile2 = fopen("Output2.bmp", "wb");
    fwrite(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, outputFile2);
    fwrite(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, outputFile2);
    fwrite(outputImg2, sizeof(unsigned char), size, outputFile2);

    //메모리 할당 해제
    free(outputImg1);
    free(inputImg1);
    fclose(inputFile1);
    fclose(outputFile1);
    free(y1);

    free(outputImg2);
    free(inputImg2);
    fclose(inputFile2);
    fclose(outputFile2);
    free(y2);
}
