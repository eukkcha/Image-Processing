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
    unsigned char *inputImg1 = NULL;
    inputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));
    fread(inputImg1, sizeof(unsigned char), size1, inputFile1);

    // Function o: original
    unsigned char *o = NULL;
    o = (unsigned char *)calloc(size1, sizeof(unsigned char));
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
            o[j * width1 + i] = inputImg1[j * stride1 + 3 * i + 0];

    // DPCM Code //
    // Function e: Prediction Error
    unsigned char *e = NULL;
    e = (unsigned char *)calloc(size1, sizeof(unsigned char));

    // Function qtz: Quantization
    unsigned char *qtz = NULL;
    qtz = (unsigned char *)calloc(size1, sizeof(unsigned char));

    // Function iqtz: Inverse Quantization
    unsigned char *iqtz = NULL;
    iqtz = (unsigned char *)calloc(size1, sizeof(unsigned char));
    
    // Function r: Reconstruction
    unsigned char *r = NULL;
    r = (unsigned char *)calloc(size1, sizeof(unsigned char));

    // bitstream.txt File
    FILE *bitstream = fopen("bitstream.txt", "w");

    // outputImg1: r(enc)
    unsigned char *outputImg1 = NULL;
    outputImg1 = (unsigned char *)calloc(size1, sizeof(unsigned char));

    int p;     // predicted
    int q = 5; // Quantization parameter

    // DPCM Based Encoder
    for (int j = 0; j < height1; j++)
        for (int i = 0; i < width1; i++)
        {
            if (i == 0)
                p = 128;                                 // Prediction with a predfined value for the first pixel of each row
            e[j * width1 + i] = o[j * width1 + i] - p;   // Prediction Error
            qtz[j * width1 + i] = e[j * width1 + i] / q; // Quantization

            // Binarization

            iqtz[j * width1 + i] = qtz[j * width1 + i] * q; // Inverse Quantization
            r[j * width1 + i] = iqtz[j * width1 + i] + p;   // Reconstruction
            p = r[j * width1 + i];                          // Update prediction

            // outputImg1: r(enc)
            outputImg1[j * stride1 + 3 * i + 0] = r[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 1] = r[j * width1 + i];
            outputImg1[j * stride1 + 3 * i + 2] = r[j * width1 + i];
        }

    // outputFile1: reconDecY.bmp
    FILE *outputFile1 = fopen("reconDecY.bmp", "wb");
    fwrite(&bmpFile1, sizeof(BITMAPFILEHEADER), 1, outputFile1);
    fwrite(&bmpInfo1, sizeof(BITMAPINFOHEADER), 1, outputFile1);
    fwrite(outputImg1, sizeof(unsigned char), size1, outputFile1);

    // // DPCM Based Decoder
    // for (int j = 0; j < height1; j++)
    //     for (int i = 0; i < width1; i++)
    //     {
    //         // Inverse Binarization

    //         iqtz[j * width1 + i] = qtz[j * width1 + i] * q; // Inverse Quantization
    //         r[j * width1 + i] = iqtz[j * width1 + i] + p; // Reconstruction
    //         p = r[j * width1 + i]; // Update prediction
    //     }

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
    fclose(bitstream);
}
