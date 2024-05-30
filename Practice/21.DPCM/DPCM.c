#define _CRT_SECURE_NO_WARINGS
#pragma warning(disable : 4996);
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	// I/O Code //
	// inputFile1: AICenterY.bmp
	BITMAPFILEHEADER bmpFile1;
	BITMAPINFOHEADER bmpInfo1;
	FILE *inputFile1 = NULL;
	inputFile1 = fopen("AICenterY.bmp", "rb");
	fread(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, inputFile1);
	fread(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, inputFile1);

	// File1 Size Info
	int width1 = bmpInfo1.biWidth;
	int height1 = bmpInfo1.biHeight;
	int size1 = bmpInfo1.biSizeImage;
	int bitCnt1 = bmpInfo1.biBitCount;
	int stride1 = (((bitCnt1 / 8) * width1) + 3 / 4 * 4);

	// InputImg1
	unsigned char *inputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));
	fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

	// Function o: original
	unsigned char *o = (unsigned char *)calloc(size1, sizeof(unsigned char));
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			o[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

	// DPCM Based Encoder code //
	unsigned char *e = (unsigned char *)calloc(size1, sizeof(unsigned char));    // Function e: Prediction Error
	unsigned char *qtz = (unsigned char *)calloc(size1, sizeof(unsigned char));  // Function qtz: Quantization
	unsigned char *iqtz = (unsigned char *)calloc(size1, sizeof(unsigned char)); // Function iqtz: Inverse Quantization
	unsigned char *r = (unsigned char *)calloc(size1, sizeof(unsigned char));    // Function r: Reconstruction

	// Open bitstream.txt for writing
	FILE *bitstream = fopen("bitstream.txt", "w");

	// outputImg1: r(enc)
	unsigned char *outputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));

	int p;     // predicted
	int q = 5; // Quantization parameter
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (i == 0)
				p = 128; // Prediction with a predfined value for the first pixel of each row

			e[j * width1 + i] = o[j * width1 + i] - p;   // Prediction Error
			qtz[j * width1 + i] = e[j * width1 + i] / q; // Quantization

			// Binarization code
			char binStr[7];
			for (int b = 5; b >= 0; b--)
				binStr[5 - b] = (qtz[j * width1 + i] & (1 << b)) ? '1' : '0';
			binStr[6] = '\0';
			fprintf(bitstream, "%s\n", binStr);

			iqtz[j * width1 + i] = qtz[j * width1 + i] * q; // Inverse Quantization
			r[j * width1 + i] = iqtz[j * width1 + i] + p;   // Reconstruction
			p = r[j * width1 + i];                          // Update prediction

			// outputImg1: r(enc)
			outputImg1[j * stride1 + 3 * i + 0] = r[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 1] = r[j * width1 + i];
			outputImg1[j * stride1 + 3 * i + 2] = r[j * width1 + i];
		}

	// outputFile1: reconEncY.bmp
	FILE *outputFile1 = fopen("reconEncY.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
	fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

	// close bitstream.txt
	fclose(bitstream);

	// DPCM Based Decoder code //
	unsigned char *qtz2 = (unsigned char *)calloc(size1, sizeof(unsigned char));  // Function qtz: Quantization
	unsigned char *iqtz2 = (unsigned char *)calloc(size1, sizeof(unsigned char)); // Function iqtz: Inverse Quantization
	unsigned char *r2 = (unsigned char *)calloc(size1, sizeof(unsigned char));    // Function r: Reconstruction

	// Open bitstream.txt for reading
	bitstream = fopen("bitstream.txt", "r");

	// outputImg2: r(dec)
	unsigned char *outputImg2 = (unsigned char *)calloc(size1, sizeof(unsigned char));

	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (i == 0)
				p = 128; // Prediction with a predfined value for the first pixel of each row

			// Inverse Binarization
			char binStr[7];
			fscanf(bitstream, "%s", binStr);
			for (int b = 0; b < 6; b++)
				if (binStr[b] == '1')
					qtz2[j * width1 + i] |= (1 << (5 - b));

			iqtz2[j * width1 + i] = qtz2[j * width1 + i] * q; // Inverse Quantization
			r2[j * width1 + i] = iqtz2[j * width1 + i] + p;   // Reconstruction
			p = r2[j * width1 + i];                           // Update prediction

			// outputImg2: r(dec)
			outputImg2[j * stride1 + 3 * i + 0] = r2[j * width1 + i];
			outputImg2[j * stride1 + 3 * i + 1] = r2[j * width1 + i];
			outputImg2[j * stride1 + 3 * i + 2] = r2[j * width1 + i];
		}

	// outputFile2: reconDecY.bmp
	FILE *outputFile2 = fopen("reconDecY.bmp", "wb");
	fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile2);
	fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile2);
	fwrite(outputImg2, sizeof(unsigned char), size1, outputFile2);

	// close bitstream.txt
	fclose(bitstream);

	// PSNR Code //
	// Encoder - Decoder PSNR
	double mse = 0, psnr;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse += (double)((r[j * width1 + i] - r2[j * width1 + i]) * (r[j * width1 + i] - r2[j * width1 + i]));
	mse /= (width1 * height1);
	psnr = mse != 0.0 ? 10.0 * log10(255 * 255 / mse) : 99.99;
	printf("%.2lfdB(%.2lf)\n", psnr, mse);

	// Original - Decoder PSNR
	double mse2 = 0, psnr2;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
			mse2 += (double)((o[j * width1 + i] - r[j * width1 + i]) * (o[j * width1 + i] - r[j * width1 + i]));
	mse2 /= (width1 * height1);
	psnr2 = mse2 != 0.0 ? 10.0 * log10(255 * 255 / mse2) : 99.99;
	printf("%.2lfdB(%.2lf)\n", psnr2, mse2);

	// free memory
	free(inputImg1);
	fclose(inputFile1);

	free(o);
	free(e);
	free(qtz);
	free(iqtz);
	free(r);
	free(qtz2);
	free(iqtz2);
	free(r2);

	free(outputImg1);
	fclose(outputFile1);
	free(outputImg2);
	fclose(outputFile2);
}

// e값이 적어야 좋은 압축임 (예측이 잘 되야 함)
// 기말고사는 필기시험 (좀 어렵게 나올 수 있음)
// 과제5 인코더 파일, 디코더 파일... 자세한건 다음주 화요일
