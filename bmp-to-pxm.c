#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

#define WIDTHBYTES(bits) (((bits)+31)/32*4)
#define BYTE    unsigned char

void bmp2pgm_BW() {
    FILE *infile;

    if((infile = fopen("ImgBW1.bmp", "rb")) == NULL) {
        printf("NO IMAGE FILE");
        return;
    }

    BITMAPFILEHEADER hf;
    BITMAPINFOHEADER hInfo;
    RGBQUAD hRGB[256];

    fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);
    fread(hRGB, sizeof(RGBQUAD), 256, infile);

    int imgWidth = hInfo.biWidth;
    int imgHeight = hInfo.biHeight;
    int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

    BYTE *lpImg_bmp = malloc(hInfo.biSizeImage);
    BYTE *bwImgRev_pgm = malloc(imgHeight * imgWidth);
    BYTE *bwImg_pgm = malloc(imgHeight * imgWidth);

    fread(lpImg_bmp, sizeof(char), hInfo.biSizeImage, infile);
    fclose(infile);

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            bwImgRev_pgm[i * imgWidth + j] = lpImg_bmp[i * rwsize + j];
            bwImg_pgm[i * imgWidth + j] = lpImg_bmp[(imgHeight - 1 - i) * rwsize + j];
        }
    }

    FILE *out = fopen("bwImg_rev.pgm", "wb");
    fprintf(out, "P5\n");
    fprintf(out, "%d %d\n", imgWidth, imgHeight);
    fprintf(out, "255\n");
    fwrite(bwImgRev_pgm, sizeof(char), imgHeight * imgWidth, out);
    fclose(out);

    out = fopen("bwImg.pgm", "wb");
    fprintf(out, "P5\n");
    fprintf(out, "%d %d\n", imgWidth, imgHeight);
    fprintf(out, "255\n");
    fwrite(bwImg_pgm, sizeof(char), imgHeight * imgWidth, out);
    fclose(out);
}


void bmp2pgm_Color() {
    FILE *infile;

    if((infile = fopen("ImgColor.bmp", "rb")) == NULL) {
        printf("NO IMAGE FILE");
        return;
    }

    BITMAPFILEHEADER hf;
    BITMAPINFOHEADER hInfo;
    fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);

    if(hf.bfType != 0x4D42)
        exit(1);
    if(hInfo.biBitCount != 24) {
        printf("BAD FILE FORMAT !!");
        return;
    }

    int imgWidth = hInfo.biWidth;
    int imgHeight = hInfo.biHeight;
    int rwsize = WIDTHBYTES(hInfo.biBitCount * hInfo.biWidth);

    BYTE *lpImg_bmp = malloc(hInfo.biSizeImage);
    BYTE *BgrRevImg_pgm = malloc(imgHeight * imgWidth * 3);
    BYTE *RgbRevImg_pgm = malloc(imgHeight * imgWidth * 3);
    BYTE *RgbImg_pgm = malloc(imgHeight * imgWidth * 3);

    fread(lpImg_bmp, sizeof(char), hInfo.biSizeImage, infile);
    fclose(infile);

    int color_k = 0;
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            for(int k = 0; k < 3; k++) { 

                if (k == 0) {
                    color_k = 2;
                }
                else if(k == 1) {
                    color_k = k;
                }
                else {
                    color_k = 0;
                }
                
                BgrRevImg_pgm[i * imgWidth * 3 + j * 3 + k] = lpImg_bmp[i * rwsize + j * 3 + k];
                RgbRevImg_pgm[i * imgWidth * 3 + j * 3 + k] = lpImg_bmp[i * rwsize + j * 3 + color_k];
                RgbImg_pgm[i * imgWidth * 3 + j * 3 + k] = lpImg_bmp[(imgHeight - 1 - i) * rwsize + j * 3 + color_k];
            }
        }
    }

    FILE *outfile = fopen("BGR-REV Img.pgm", "wb");
    fprintf(outfile, "P6\n");
    fprintf(outfile, "%d %d\n", imgWidth, imgHeight);
    fprintf(outfile, "255\n");
    fwrite(BgrRevImg_pgm, sizeof(char), imgHeight * imgWidth * 3, outfile);
    fclose(outfile);

    outfile = fopen("RGB-REV Img.pgm", "wb");
    fprintf(outfile, "P6\n");
    fprintf(outfile, "%d %d\n", imgWidth, imgHeight);
    fprintf(outfile, "255\n");
    fwrite(RgbRevImg_pgm, sizeof(char), imgHeight * imgWidth * 3, outfile);
    fclose(outfile);

    outfile = fopen("RGB Img.pgm", "wb");
    fprintf(outfile, "P6\n");
    fprintf(outfile, "%d %d\n", imgWidth, imgHeight);
    fprintf(outfile, "255\n");
    fwrite(RgbImg_pgm, sizeof(char), imgHeight * imgWidth * 3, outfile);
    fclose(outfile);
}


void main() {
    bmp2pgm_BW();
    bmp2pgm_Color();
}