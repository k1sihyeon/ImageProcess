#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <stdbool.h>

#define BYTE unsigned char

struct Size {
    int width;
    int height;
    int size;
};

struct Size getSize(char* filename) {
    FILE *infile;
    struct Size size;
    BITMAPFILEHEADER hf;
    BITMAPINFOHEADER hInfo;

    if((infile = fopen(filename, "rb")) == NULL) {
        printf("NO IMAGE FILE");
        exit(0);
    }

    fread(&hf, sizeof(BITMAPFILEHEADER), 1, infile);
    fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, infile);
    size.width = hInfo.biWidth;
    size.height = hInfo.biHeight;
    size.size = size.width * size.height;
    fclose(infile);

    return size;
}

void writeImg(BYTE* outImg, int W, int H, char* filename) {

    FILE *out = fopen(filename, "wb");
	fprintf(out, "P5\n");
	fprintf(out, "%d %d\n", W, H);
	fprintf(out, "255\n");
	fwrite(outImg, sizeof(char), W * H, out);
	fclose(out);

    return;
}

bool isSameRegion(BYTE cur, BYTE nxt) {
    bool res = (cur != 0) && (nxt != 0) && (cur != nxt);
    return res;
}


int main(void) {
    //get img size by bmp
    struct Size size;
    size = getSize("hand.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("hand.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    int hist[256];
    float between[256];
    int thres;
    float avg;
    float p1, mm1, mm2, ptmp, mtmp, max;
    ptmp = 0.;
    mtmp = 0.;
    max = 0.;

    BYTE *otsuImg = (BYTE *)malloc(imgSize * sizeof(char));
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            otsuImg[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }

    //hist init
    for(int i = 0; i < 256; i++) {
        hist[i] = 0;
    }

    //get hist
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            int idx = inImg[i * imgWidth + j];
            hist[idx] += 1;
        }
    }

    //get global avg
    for(int i = 0; i < 256; i++) {
        avg += (float)(i * hist[i]);
    }
    avg = (float)avg / (float)imgSize;

    //between
    for(int T = 0; T < 256; T++) {
        p1 = ptmp + (float)hist[T] / (float)imgSize;

        if(p1 == 0.) {
            mm1 = 0.;
        }
        else {
            mm1 = (ptmp * mtmp + (T * ((float)hist[T] / (float)imgSize))) / p1;
        }

        if(p1 == 1.) {
            mm2 = 0.;
        }
        else {
            mm2 = (avg - p1 * mm1) / (1. - p1);
        }

        between[T] = p1 * (mm1 - avg) * (mm1 - avg) + (1. - p1) * (mm2 - avg) * (mm2 - avg);
        //p1 * (1 - p1) * (mm1 - mm2) * (mm1 - mm2);
        ptmp = p1;
        mtmp = mm1;
    }

    //get threshold
    for(int i = 0; i < 256; i++) {
        if(max < between[i]) {
            max = between[i];
            thres = i;
        }
    }

    //thresholding
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {

            if(otsuImg[i * imgWidth + j] < thres) {
                otsuImg[i * imgWidth + j] = 0;
            }
            else {
                otsuImg[i * imgWidth + j] = 255;
            }
        }
    }

    printf("max: %f, thres: %d", max, thres);

    writeImg(otsuImg, imgWidth, imgHeight, "1_otsu_hand.pgm");


    ////////////////////////////////////////////////////////////////
    //efficient 2-pass
    BYTE *labelImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            labelImg[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }

    int count = 1;
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {

            if(labelImg[i * imgWidth + j] != 0) {
                labelImg[i * imgWidth + j] = count;
                count += 1;
            }
        }
    }

    //top down pass
    for(int i = 0; i < imgHeight - 1; i++) {
        for(int j = 1; j < imgWidth - 1; j++) {

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[i * imgWidth + (j+1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[i * imgWidth + (j+1)])
                    labelImg[i * imgWidth + (j+1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[i * imgWidth + (j+1)];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i+1) * imgWidth + (j+1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i+1) * imgWidth + (j+1)])
                    labelImg[(i+1) * imgWidth + (j+1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i+1) * imgWidth + (j+1)];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i+1) * imgWidth + j])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i+1) * imgWidth + j])
                    labelImg[(i+1) * imgWidth + j] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i+1) * imgWidth + j];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i+1) * imgWidth + (j-1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i+1) * imgWidth + (j-1)])
                    labelImg[(i+1) * imgWidth + (j-1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i+1) * imgWidth + (j-1)];
            }
        }
    }

    //bottom up pass
    for(int i = 255; i > 0; i--) {
        for(int j = 255; j > 0; j--) {

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[i * imgWidth + (j-1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[i * imgWidth + (j-1)])
                    labelImg[i * imgWidth + (j-1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[i * imgWidth + (j-1)];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i-1) * imgWidth + (j-1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i-1) * imgWidth + (j-1)])
                    labelImg[(i-1) * imgWidth + (j-1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i-1) * imgWidth + (j-1)];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i-1) * imgWidth + j])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i-1) * imgWidth + j])
                    labelImg[(i-1) * imgWidth + j] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i-1) * imgWidth + j];
            }

            if(isSameRegion(labelImg[i * imgWidth + j], labelImg[(i-1) * imgWidth + (j+1)])) {
                if(labelImg[i * imgWidth + j] < labelImg[(i-1) * imgWidth + (j+1)])
                    labelImg[(i-1) * imgWidth + (j+1)] = labelImg[i * imgWidth + j];
                else
                    labelImg[i * imgWidth + j] = labelImg[(i-1) * imgWidth + (j+1)];
            }
        }
    }

    //get region hist
    int *tmpHist = malloc(count * sizeof(int));
    int hcount = 0;

    for(int i = 0; i < count; i++) {
        tmpHist[i] = 0;
    }

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            tmpHist[labelImg[i * imgWidth + j]] += 1;
            hcount += 1;
        }
    }

    int *rHist = malloc(hcount * sizeof(int));

    for(int i = 0; i < hcount; i++) {
        rHist[i] = 0;
    }

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            rHist[labelImg[i * imgWidth + j]] += 1;
        }
    }

    free(tmpHist);

    //remove small region
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            if(rHist[labelImg[i * imgWidth + j]] < 30) {
                labelImg[i * imgWidth + j] = 0;
            }

            else {
                labelImg[i * imgWidth + j] = 255;
            }
        }
    }

    // printf("hcount: %d, count: %d", hcount, count);

    writeImg(labelImg, imgWidth, imgHeight, "2_efficient_2_pass.pgm");

    return 0;
}