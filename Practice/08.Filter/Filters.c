#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

//알고리즘 내용
//5x5 gaussian filter
//3x3 gaussian filter
//3x3 mean filter
//same padding
//zero padding
//replicate padding
//convolution
//MSE(PSNR)

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
    inputFile2 = fopen("AICenterY_CombinedNoise.bmp", "rb");
    fread(&bmpFile2, sizeof(BITMAPFILEHEADER), 1, inputFile2);
    fread(&bmpInfo2, sizeof(BITMAPINFOHEADER), 1, inputFile2);

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

    //영상2 인/아웃풋이미지 선언
    unsigned char* inputImg2 = NULL, * outputImg2 = NULL;
    inputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
    outputImg2 = (unsigned char*)calloc(size, sizeof(unsigned char));
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

    //Filter Design 선언
    //Gaussian Filter 5x5
    double Gauss5F[5][5] = { {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273},
                              {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
                              {(double)7 / 273, (double)26 / 273, (double)41 / 273, (double)26 / 273, (double)7 / 273},
                              {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
                              {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273} };
    //Gaussian Filter 3x3
    double Gauss3F[3][3] = { {(double)1 / 16, (double)2 / 16, (double)1 / 16},
                              {(double)2 / 16, (double)4 / 16, (double)2 / 16},
                              {(double)1 / 16, (double)2 / 16, (double)1 / 16} };
    //Mean Filter 3x3
    double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
                            {(double)1 / 9, (double)1 / 9, (double)1 / 9},
                            {(double)1 / 9, (double)1 / 9, (double)1 / 9} };

    //Same Padding
    int p = (3 - 1) / 2; //Same Padding의 폭 = ((Filter의 폭) - 1) / 2
    int pwidth = width + 2 * p; //Same Padding을 적용한 데이터의 크기 : (height + 2p, width + 2p)
    int pheight = height + 2 * p;
    int psize = pwidth * pheight * 3;

    //결과물을 저장할 함수 result 선언
    unsigned char* result = NULL;
    result = (unsigned char*)calloc(psize, sizeof(unsigned char));

    //Zero Padding
    for (int j = 0; j < pheight; j++)
        for (int i = 0; i < pwidth; i++)
            result[j * pwidth + i] = 0;
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
            result[(p + j) * pwidth + (p + i)] = y2[j * width + i];

    //Replicate Padding
    for (int j = 0; j < p; j++)
        for (int i = 0; i < width; i++) {
            result[j * pwidth + p + i] = y2[i]; //상단
            result[(pheight - 1 - j) * pwidth + p + i] = y2[(height - 1) * width + i]; //하단
        }
    for (int j = 0; j < height; j++)
        for (int i = 0; i < p; i++) {
            result[(p + j) * pwidth + i] = y2[j * width]; // 좌측
            result[(p + j) * pwidth + (pwidth - 1 - i)] = y2[j * width + width - 1]; //우측
        }
    for (int j = 0; j < p; j++)
        for (int i = 0; i < p; i++) {
            result[j * pwidth + i] = y2[0]; //좌상단
            result[j * pwidth + pwidth - 1 - i] = y2[width - 1]; //우상단
            result[(pheight - 1 - j) * pwidth + i] = y2[(height - 1) * width]; //좌하단
            result[(pheight - 1 - j) * pwidth + pwidth - 1 - i] = y2[(height - 1) * width + width - 1]; //우하단
        }

    //Convolution
    for (int j = p; j < p + height; j++)
        for (int i = p; i < p + width; i++) {
            int F = 0;
            for (int m = 0; m < 3; m++) //3x3 Filter (m,n) = (3,3)
                for (int n = 0; n < 3; n++)
                    F += result[(j - p + m) * pwidth + (i - p + n)] * Gauss3F[m][n];
            result[j * pwidth + i] = F;
        }

    //원본 - 노이즈 PSNR (y1 - y2)
    double mse = 0, psnr;
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
            mse += (double)((y1[j * width + i] - y2[j * width + i]) * (y1[j * width + i] - y2[j * width + i]));
    mse /= (width * height);
    psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
    printf("[기존 노이즈 PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse, psnr);
    //18.13dB

    //원본 - 결과 PSNR (y1 - result)
    double mse_result = 0, psnr_result;
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
            mse_result += (double)((y1[j * width + i] - result[(p + j) * pwidth + (p + i)]) * (y1[j * width + i] - result[(p + j) * pwidth + (p + i)]));
    mse_result /= (width * height);
    psnr_result = mse_result != 0.0 ? 10.0 * log10(255 * 255 / mse_result) : 99.99;
    printf("[내 필터 PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse_result, psnr_result);
    //Gaussian 5x5 (zero) : 19.32dB
    //Gaussian 5x5 (replicate) : 19.54dB
    //Mean 3x3 (zero) : 22.52dB
    //Mean 3x3 (replicate) : 22.90
    //Gaussian 3x3 (zero) : 23.02dB
    //Gaussian 3x3 (replicate) : 23.27dB

    //outputImg에 result 입력 (패딩 부분 제외)
    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++) {
            outputImg1[j * stride + 3 * i + 0] = result[(p + j) * pwidth + (p + i)];
            outputImg1[j * stride + 3 * i + 1] = result[(p + j) * pwidth + (p + i)];
            outputImg1[j * stride + 3 * i + 2] = result[(p + j) * pwidth + (p + i)];
        }

    //outputImg 파일 추출
    FILE* outputFile1 = fopen("Filter.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size, outputFile1);

    //사용한 메모리 할당 해제
    free(inputImg1);
    fclose(inputFile1);
    free(y1);

    free(inputImg2);
    fclose(inputFile2);
    free(y2);

    free(outputImg1);
    fclose(outputFile1);
    free(result);
}

////디버깅 (padding이 잘 되었는지 확인)
//for (int j = 0; j < 5; j++) //좌상단
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //우상단
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = 0; j < 5; j++) //좌하단
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //우하단
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//
//printf("----------\n\n");
