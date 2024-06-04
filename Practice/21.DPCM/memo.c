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
	int q = 20; // Quantization parameter
	int flag = 0;
	for (int j = 0; j < height1; j++)
		for (int i = 0; i < width1; i++)
		{
			if (i == 0)
				p = 128; // Prediction with a predfined value for the first pixel of each row

			e[j * width1 + i] = o[j * width1 + i] - p;   // Prediction Error
			qtz[j * width1 + i] = e[j * width1 + i] / q; // Quantization

			if (flag < 50) printf("%d - %d =  %d\n", o[j * width1 + i], p, e[j * width1 + i]); flag++;

			// Binarization code
			if (qtz[j * width1 + i] == 0) fprintf(bitstream, "0000");
			else if (qtz[j * width1 + i] == 1) fprintf(bitstream, "0001");
			else if (qtz[j * width1 + i] == 2) fprintf(bitstream, "0010");
			else if (qtz[j * width1 + i] == 3) fprintf(bitstream, "0011");
			else if (qtz[j * width1 + i] == 4) fprintf(bitstream, "0100");
			else if (qtz[j * width1 + i] == 5) fprintf(bitstream, "0101");
			else if (qtz[j * width1 + i] == 6) fprintf(bitstream, "0110");
			else if (qtz[j * width1 + i] == 7) fprintf(bitstream, "0111");
			else if (qtz[j * width1 + i] == 8) fprintf(bitstream, "1000");
			else if (qtz[j * width1 + i] == 9) fprintf(bitstream, "1001");
			else if (qtz[j * width1 + i] == 10) fprintf(bitstream, "1010");
			else if (qtz[j * width1 + i] == 11) fprintf(bitstream, "1011");
			else if (qtz[j * width1 + i] == 12) fprintf(bitstream, "1100");

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

	// free memory
	free(inputImg1);
	fclose(inputFile1);

	free(o);
	free(e);
	free(qtz);
	free(iqtz);
	free(r);

	free(outputImg1);
	fclose(outputFile1);
}

// e값이 적어야 좋은 압축임 (예측이 잘 되야 함)
