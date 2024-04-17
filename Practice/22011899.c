#pragma warning(disable:4996);
#define _CRT_SECURE_NO_WARINGS
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	/////////////////////////////////////////////////////////////////
	// AICenter Format


	// ����1 �ҷ�����
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE* inputFile1 = NULL;
	inputFile1 = fopen("original.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// ����1 ũ�� ����
	int width1 = bmpInfo1.biWidth;
	int height1 = bmpInfo1.biHeight;
	int size1 = bmpInfo1.biSizeImage;
	int bitCnt1 = bmpInfo1.biBitCount;
	int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

	// ��ǲ�̹���1 ����
	unsigned char* inputImg1 = NULL;
	inputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

	// �ƿ�ǲ�̹���1 ����
	unsigned char* outputImg1 = NULL;
	outputImg1 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// �Լ� y1 : original
	unsigned char* y1 = NULL;
	y1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			y1[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// �ƿ�ǲ�̹���1 y1 �Ҵ�
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = y1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 1] = y1[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 2] = y1[j * width1 + i];
		}

	// �ƿ�ǲ�̹��� ����1
	FILE* outputFile1 = fopen("output.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

	// �޸� �Ҵ� ����
	free(inputImg1);
	fclose(inputFile1);

	free(y1);

	free(outputImg1);
	fclose(outputFile1);


	/////////////////////////////////////////////////////////////////
	// AICenter Format


	// ����1 �ҷ����� (AICenter.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1 ����
	// �ƿ�ǲ�̹���1 ����

	// �˰���
	int Y;
	for (int j = 0; j < height1; j++)
	{
		for (int i = 0; i < width1; i++)
		{
			// 0. Image Format
			outputImg1[j * stride1 + 3 * i + 0] = inputImg1[j * stride1 + 3 * i + 0];
			outputImg1[j * stride1 + 3 * i + 1] = inputImg1[j * stride1 + 3 * i + 1];
			outputImg1[j * stride1 + 3 * i + 2] = inputImg1[j * stride1 + 3 * i + 2];

			// 1. Experiment
			if (j < 100 && i < 100)
			{
				outputImg1[j * stride1 + 3 * i + 0] = 0;
				outputImg1[j * stride1 + 3 * i + 1] = 0;
				outputImg1[j * stride1 + 3 * i + 2] = 0;
			}
			if (j > 400 && i > 400)
			{
				outputImg1[j * stride1 + 3 * i + 0] = 255;
				outputImg1[j * stride1 + 3 * i + 1] = 255;
				outputImg1[j * stride1 + 3 * i + 2] = 255;
			}

			// 2. RGB Filter
			outputImg1[j * stride1 + 3 * i + 0] = 0;
			outputImg1[j * stride1 + 3 * i + 1] = 0;
			outputImg1[j * stride1 + 3 * i + 2] = inputImg1[j * stride1 + 3 * i + 2];

			// 3. YYY Format
			Y = 0.299 * outputImg1[j * stride1 + 3 * i + 2] + 0.587 * outputImg1[j * stride1 + 3 * i + 1] + 0.114 * outputImg1[j * stride1 + 3 * i + 0];
			outputImg1[j * stride1 + 3 * i + 0] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg1[j * stride1 + 3 * i + 1] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
			outputImg1[j * stride1 + 3 * i + 2] = (unsigned char)(Y > 255 ? 255 : (Y < 0 ? 0 : Y));
		}
	}

	// �ƿ�ǲ�̹��� ����1
	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// AICenterY Masking


	// ����1 �ҷ����� (AICenterY.bmp)
	// ����2 �ҷ����� (Mask.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1, ��ǲ�̹���2 ����
	// �ƿ�ǲ�̹���1, �ƿ�ǲ�̹���2 ����

	// �Լ� y1 : AICenterY
	// �Լ� y2 : Mask 

	// �Լ� result : AICenter * Mask
	unsigned char* result1 = NULL;
	result1 = (unsigned char*)calloc(size1, sizeof(unsigned char));
	unsigned char* result2 = NULL;
	result2 = (unsigned char*)calloc(size1, sizeof(unsigned char));

	// �˰���
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y2[j * width1 + i] == 0) // 0�� �κ�
				y2[j * width1 + i] = y2[j * width1 + i] + 1; // y1���� �״�� �ֱ� ���� 1�� �����
			else
				y2[j * width1 + i] = y2[j * width1 + i] - 255; // 255�� �κ��� 0���� �����
			// result1���� y2�� 1�� �κ��� y1����, y2�� 0�� �κ��� 0���� �Էµ�
			result1[j * width1 + i] = y1[j * width1 + i] * y2[j * width1 + i];
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y2[j * width1 + i] == 1) // 1�� ������� �κ�
				y2[j * width1 + i] = y2[j * width1 + i] - 1; // 0���� �����
			else // 0���� ������� �κ�
				y2[j * width1 + i] = y2[j * width1 + i] + 1; // y1���� �״�� �ֱ� ���� 1�� �����
			// result2���� y2�� 0�� �κ��� 0����, y2�� 1�� �κ��� y1���� �Էµ�
			result2[j * width1 + i] = y1[j * width1 + i] * y2[j * width1 + i];
		}

	// �ƿ�ǲ�̹���1/2 result1/2 �Ҵ�
	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// PSNR


	// ����1 �ҷ����� (AICenterY.bmp)
	// ����2 �ҷ����� (AICenterY_Noise.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1, ��ǲ�̹���2 ����
	// �ƿ�ǲ�̹���1, �ƿ�ǲ�̹���2 ����

	// �Լ� y1 : AICenterY
	// �Լ� y2 : AICenterY_Noise

	// PSNR1 (���� - ������)
	double mse1 = 0, psnr1;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse1 += (double)((y1[j * width1 + i] - y2[j * width1 + i]) * (y1[j * width1 + i] - y2[j * width1 + i]));
	mse1 /= (width1 * height1);
	psnr1 = mse1 != 0.0 ? 10.0 * log10(255 * 255 / mse1) : 99.99;
	printf("MSE = %.2lf\nPSNR = %.2lfdB\n\n", mse1, psnr1);


	/////////////////////////////////////////////////////////////////
	// Filters


	// ����1 �ҷ����� (AICenterY.bmp)
	// ����2 �ҷ����� (AICenterY_CombinedNoise.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1, ��ǲ�̹���2 ����
	// �ƿ�ǲ�̹���1 ����

	// �Լ� y1 : AICenterY
	// �Լ� y2 : AICenterY_CombinedNoise  

	// Filter Design ����
	// Gaussian Filter 5x5
	double Gauss5F[5][5] = { {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273},
							 {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
							 {(double)7 / 273, (double)26 / 273, (double)41 / 273, (double)26 / 273, (double)7 / 273},
							 {(double)4 / 273, (double)16 / 273, (double)26 / 273, (double)16 / 273, (double)4 / 273},
							 {(double)1 / 273, (double)4 / 273, (double)7 / 273, (double)4 / 273, (double)1 / 273} };
	// Gaussian Filter 3x3
	double Gauss3F[3][3] = { {(double)1 / 16, (double)2 / 16, (double)1 / 16},
							 {(double)2 / 16, (double)4 / 16, (double)2 / 16},
							 {(double)1 / 16, (double)2 / 16, (double)1 / 16} };
	// Mean Filter 3x3
	double Mean3F[3][3] = { {(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9},
							{(double)1 / 9, (double)1 / 9, (double)1 / 9} };

	// Same Padding
	int p = (3 - 1) / 2; // Same Padding�� �� = ((Filter�� ��) - 1) / 2
	int pwidth1 = width1 + 2 * p; // Same Padding�� ������ �������� ũ�� : (height + 2p, width + 2p)
	int pheight1 = height1 + 2 * p;
	int psize1 = pwidth1 * pheight1 * 3;

	// �Լ� result1 : ���� ����� (ũ�� psize)

	// 1. Zero Padding
	for (int j = 0; j < pheight1; j++)
		for (int i = 0; i < pwidth1; i++)
			result1[j * pwidth1 + i] = 0;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			result1[(p + j) * pwidth1 + (p + i)] = y2[j * width1 + i];

	// 2. Replicate Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width1; i++) {
			result1[j * pwidth1 + p + i] = y2[i]; // ���
			result1[(pheight1 - 1 - j) * pwidth1 + p + i] = y2[(height1 - 1) * width1 + i]; // �ϴ�
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < p; i++) {
			result1[(p + j) * pwidth1 + i] = y2[j * width1]; // ����
			result1[(p + j) * pwidth1 + (pwidth1 - 1 - i)] = y2[j * width1 + width1 - 1]; // ����
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			result1[j * pwidth1 + i] = y2[0]; // �»��
			result1[j * pwidth1 + pwidth1 - 1 - i] = y2[width1 - 1]; // ����
			result1[(pheight1 - 1 - j) * pwidth1 + i] = y2[(height1 - 1) * width1]; // ���ϴ�
			result1[(pheight1 - 1 - j) * pwidth1 + pwidth1 - 1 - i] = y2[(height1 - 1) * width1 + width1 - 1]; // ���ϴ�
		}

	// 3. Mirror Padding
	for (int j = 0; j < p; j++)
		for (int i = 0; i < width1; i++) {
			result1[(p - 1 - j) * pwidth1 + p + i] = y2[(j + 1) * width1 + i]; // ���
			result1[(pheight1 - p + j) * pwidth1 + p + i] = y2[(height1 - 2 - j) * width1 + i]; // �ϴ�
		}
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < p; i++) {
			result1[(p + j) * pwidth1 + p - 1 - i] = y2[j * width1 + 1 + i]; // ����
			result1[(p + j) * pwidth1 + (pwidth1 - p + i)] = y2[j * width1 + width1 - 2 - i]; // ����
		}
	for (int j = 0; j < p; j++)
		for (int i = 0; i < p; i++) {
			result1[(p - 1 - j) * pwidth1 + (p - 1 - i)] = y2[(1 + j) * width1 + (1 + i)]; // �»��
			result1[(p - 1 - j) * pwidth1 + pwidth1 - p + i] = y2[(1 + j) * width1 + width1 - 2 - i]; // ����
			result1[(pheight1 - p + j) * pwidth1 + p - 1 - i] = y2[(height1 - 2 - j) * width1 + 1 + i]; // ���ϴ�
			result1[(pheight1 - p + j) * pwidth1 + pwidth1 - p + i] = y2[(height1 - 2 - j) * width1 + width1 - 2 - i]; // ���ϴ�
		}

	// Median Filter
	int len = 9;
	int value[9];
	for (int j = p; j < p + height1; j++)
		for (int i = p; i < p + width1; i++)
		{
			int F, k = 0;
			for (int m = 0; m < 3; m++) // 3x3 Filter (m,n) = (3,3)
				for (int n = 0; n < 3; n++)
				{
					value[k] = result1[(j - p + m) * pwidth1 + (i - p + n)];
					k++;
				}

			// �߾Ӱ� ���ϱ�
			int m, n, tmp = 0;
			for (m = 0; m < len; m++)
				for (n = 0; n < (len - 1) - m; n++)
					if (value[n] > value[n + 1])
					{
						tmp = value[n];
						value[n] = value[n + 1];
						value[n + 1] = tmp;
					}
			F = value[len / 2];

			result1[j * pwidth1 + i] = F;
		}

	// Convolution
	for (int j = p; j < p + height1; j++)
		for (int i = p; i < p + width1; i++) {
			int F = 0;
			for (int m = 0; m < 3; m++) // 3x3 Filter (m,n) = (3,3)
				for (int n = 0; n < 3; n++)
					F += result1[(j - p + m) * pwidth1 + (i - p + n)] * Gauss3F[m][n]; // Gauss3F
			result1[j * pwidth1 + i] = F;
		}

	// PSNR1 (���� - ������)
	double mse1 = 0, psnr1;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse1 += (double)((y1[j * width1 + i] - y2[j * width1 + i]) * (y1[j * width1 + i] - y2[j * width1 + i]));
	mse1 /= (width1 * height1);
	psnr1 = mse1 != 0.0 ? 10.0 * log10(255 * 255 / mse1) : 99.99;
	printf("[���� ������ PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse1, psnr1);
	//18.13dB

	// PSNR2 (���� - ����)
	double mse2 = 0, psnr2;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse2 += (double)((y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]) * (y1[j * width1 + i] - result1[(p + j) * pwidth1 + (p + i)]));
	mse2 /= (width1 * height1);
	psnr2 = mse2 != 0.0 ? 10.0 * log10(255 * 255 / mse2) : 99.99;
	printf("[�� ���� PSNR]\nMSE = %.2lf(%.2lfdB)\n\n", mse2, psnr2);
	//Gaussian 3x3 (replicate) : 23.27dB

	// �ƿ�ǲ�̹���1 result1 �Ҵ� (�е� �κ� ����)
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++) {
			outputImg1[j * stride1 + 3 * i + 0] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 1] = result1[(p + j) * pwidth1 + (p + i)];
			outputImg1[j * stride1 + 3 * i + 2] = result1[(p + j) * pwidth1 + (p + i)];
		}

	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// Histogram, Thresholding


	// ����1 �ҷ����� (AICenterY.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1 ����
	// �ƿ�ǲ�̹���1,2 ����

	// �Լ� y1 : AICenterY
	// �Լ� histo1 : AICenterY ������׷�
	// �Լ� result1 : AICenterY Thresholding

	// y1�� ȭ�Ұ� ���� (distribution)
	int distr1[256] = { 0 };
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			distr1[y1[j * width1 + i]]++;

	// ������ 10���� ������
	for (int k = 0; k < 256; k++)
		distr1[k] /= 15;

	// �������
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			histo1[j * width1 + i] = 255;

	// ��������ŭ �׾ƿø���
	int k = 0;
	for (int i = 0; i < width1; i += 2)
	{
		for (int j = 0; j < distr1[k]; j++)
		{
			histo1[j * width1 + i] = 0;
			histo1[j * width1 + i + 1] = 0;
		}
		k++;
	}

	// �ƿ�ǲ�̹���1 histo1 �Ҵ�

	// Binary Image (+ Multilevel Threshholding)
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (y1[j * width1 + i] > 250) result1[j * width1 + i] = 255;
			else if (y1[j * width1 + i] > 170) result1[j * width1 + i] = 0;
			else result1[j * width1 + i] = 170;
		}

	// �ƿ�ǲ�̹���2 result1 �Ҵ�
	// �ƿ�ǲ�̹��� ����1
	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// HE(Histogram Equalization)


	// ����1 �ҷ����� (AICenterY_Dark.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1 ����
	// �ƿ�ǲ�̹���1 ����

	// �Լ� y1 : AICenterY_Dark
	// �Լ� result1 : AICenterY_Dark HE

	// y1�� ȭ�Ұ� ����
	// Cumulative Histogram
	for (int i = 1; i < 256; i++)
		distr1[i] += distr1[i - 1];
	// Histogram Equalization
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			result1[j * width1 + i] = 255 * distr1[y1[j * width1 + i]] / (width1 * height1);

	// �ƿ�ǲ�̹���1 result1 �Ҵ�
	// �ƿ�ǲ�̹��� ����1
	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// Subsampling


	// ����1 �ҷ����� (AICenterY.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1 ����
	// �ƿ�ǲ�̹���1 ����

	// �Լ� y1 : AICenterY

	// Subsampling ũ��
	int ratio = 1; // ũ�⸦ ���� ����
	int width2 = bmpInfo1.biWidth >> ratio;
	int height2 = bmpInfo1.biHeight >> ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// �Լ� result1 : Subsampling (ũ�� width2*height2)
	unsigned char* result1 = (unsigned char*)calloc(width2 * height2, sizeof(unsigned char));
	for (int j = 0; j < height2; j++)
		for (int i = 0; i < width2; i++)
			result1[j * width2 + i] = y1[(j << ratio) * width1 + (i << ratio)];

	// �ƿ�ǲ�̹���1 result1 �Ҵ�
	// �ƿ�ǲ�̹��� ����1
	// �޸� �Ҵ� ����


	/////////////////////////////////////////////////////////////////
	// Upsampling


	// ����1 �ҷ����� (Subsampling.bmp)
	// ����1 ũ�� ����
	// ��ǲ�̹���1 ����
	// �ƿ�ǲ�̹���1 ����

	// �Լ� y1 : Subsampling

	// Upsampling ũ��
	int ratio = 1; // ũ�⸦ ���� ����
	int width2 = bmpInfo1.biWidth << ratio;
	int height2 = bmpInfo1.biHeight << ratio;
	int stride2 = ((bitCnt1 / 8) * width2 + 3) / 4 * 4;
	int size2 = stride2 * height2;

	// �Լ� result1 : Upsampling (ũ�� size2)
	unsigned char* result1 = (unsigned char*)calloc(size2, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			result1[(2 * j) * width2 + (2 * i)] = y1[j * width1 + i];
			result1[(2 * j) * width2 + (2 * i + 1)] = y1[j * width1 + i];
			result1[(2 * j + 1) * width2 + (2 * i)] = y1[j * width1 + i];
			result1[(2 * j + 1) * width2 + (2 * i + 1)] = y1[j * width1 + i];
		}

	// �ƿ�ǲ�̹���1 result1 �Ҵ�
	// �ƿ�ǲ�̹��� ����1
	// �޸� �Ҵ� ����
}

////����� (padding�� �� �Ǿ����� Ȯ��)
//for (int j = 0; j < 5; j++) //�»��
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //���ϴ�
//{
//    for (int i = 0; i < 5; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = 0; j < 5; j++) //����
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//for (int j = pheight - 5; j < pheight; j++) //���ϴ�
//{
//    for (int i = pwidth - 5; i < pwidth; i++)
//        printf("%d ", result[j * pwidth + i]);
//    printf("\n");
//}
//printf("\n");
//
//printf("----------\n\n");