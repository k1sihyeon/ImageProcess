#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include <time.h>

#define BYTE    unsigned char

int hist[256];
int cdf[256];

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


void hist_spec() {
    double count, ncount = 0;
    int ns = 0;
    
    //get size by bmp
    struct Size size;
    size = getSize("publicSquare.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //raw img load
    FILE *infile = fopen("publicSquare.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    //get hist
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            int index = inImg[i * imgWidth + j];
            hist[index] += 1;
        }
    }

    //get desire cdf
    for(int i = 0; i < 256; i++) {
        count += (double)hist[i]/imgSize;

        for(int j = ns; j < 256; j++) {
            ncount = ((double)(j*j)/(255. * 255.));

            if(count <= ncount) { 
                cdf[i] = j;
                ns = i;
                break;
            }
        }
    }

    //specify hist
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            inImg[i * imgWidth + j] = cdf[inImg[i * imgWidth + j]];
        }
    }

    //save file
    writeImg(inImg, imgWidth, imgHeight, "publicSquare_hist.pgm"); 

    return;
}

void avg_filter() {

    //get img size by bmp
    struct Size size;
    size = getSize("Snow.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("Snow.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    //Execution time set
    clock_t start, end;
    double exectime2d, exectime1d;

    //21*21 2D filter processing
    int window = 21;
    int sum;
    BYTE *outImg1 = (BYTE *)malloc(imgSize * sizeof(char));

    start = clock();

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            sum = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    sum += inImg[(i+k) * imgWidth + (j+l)];
                }
            }

            outImg1[i * imgWidth + j] = (int)((float)sum / (window * window) + 0.5);
        }
    }

    end = clock();
    exectime2d = (double)(end - start) / CLOCKS_PER_SEC;

    //1*21 1D filter processing
    BYTE *tmpImg = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *outImg2 = (BYTE *)malloc(imgSize * sizeof(char));
    window = 21;
    int hsum, vsum;

    start = clock();

        //1D horizon
    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {
            hsum = 0;

            for(int k = -window/2; k <= window/2; k++) {
                hsum += inImg[(i+k) * imgWidth + j];
            }

            tmpImg[i * imgWidth + j] = (int)((float)hsum / window + 0.5);
        }
    }

        //1D vertical
    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {
            vsum = 0;

            for(int k = -window/2; k <= window/2; k++) {
                vsum += tmpImg[i * imgWidth + (j + k)];
            }

            outImg2[i * imgWidth + j] = (int)((float)vsum / window + 0.5);
        }
    }

    end = clock();
    exectime1d = (double)(end - start) / CLOCKS_PER_SEC;

    //save img 1, 2
    writeImg(outImg1, imgWidth, imgHeight, "Snow_blur_2D.pgm"); 
    writeImg(outImg2, imgWidth, imgHeight, "Snow_blur_1D.pgm"); 

    printf("\n2D Average Filter Execution Time: %lf\n", exectime2d);
    printf("1D Average Filter Execution Time: %lf\n", exectime1d);

    return;
}

void integ_avg() {

    //get img size by bmp
    struct Size size;
    size = getSize("Snow.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("Snow.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    //integral image
    int *integImg = (int *)malloc(imgSize * sizeof(int));
    BYTE *outImg = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *normintegImg = (BYTE *)malloc(imgSize * sizeof(char));

        //horizon cumulation
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {

            if(j == 0) {
                integImg[i * imgWidth + j] = inImg[i * imgWidth + j];
            }
            else {
                integImg[i * imgWidth + j] = integImg[i * imgWidth + (j-1)] + inImg[i * imgWidth + j];
            }
        }
    }

        //vertical cumluation
    for(int i = 0; i < imgWidth; i++) {
        for(int j = 1; j < imgHeight; j++) {
            integImg[j * imgWidth+ i] += integImg[(j-1) * imgWidth+ i];
        }
    }

        //normaliztion
    int max = integImg[(imgHeight - 1) * imgWidth + (imgWidth - 1)];
    
    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            normintegImg[i * imgWidth + j] = (int)(255. * ((float)integImg[i * imgWidth + j] / max));
        }
    }

    //execution time set
    clock_t start, end;

    //avg filter processing
    int window = 21;
    int k = window / 2;
    int sum;

    start = clock();

    for(int i = k; i < imgHeight - k; i++) {
        for(int j = k; j < imgWidth - k; j++) {
            sum = integImg[(i-k) * imgWidth + (j-k) - 1] + integImg[(i+k) * imgWidth + (j+k)] 
            - (integImg[(i-k) * imgWidth + (j+k) - 1] + integImg[(i+k) * imgWidth + (j-k) - 1]);

            outImg[i * imgWidth + j] = sum = (int)((float)sum / (window * window) + 0.5);
        }
    }

    end = clock();
    printf("Integration Average Execution Time: %f\n", (double)(end - start)/ CLOCKS_PER_SEC);

    //save img
    writeImg(normintegImg, imgWidth, imgHeight, "Snow_integ.pgm");
    writeImg(outImg, imgWidth, imgHeight, "Snow_integ_blur.pgm"); 

    return;
}

void unsharp_mask() {

    //get img size by bmp
    struct Size size;
    size = getSize("Pentagon.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("Pentagon.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    //get LPF and HPF
    int window = 5;
    int sum, v;
    BYTE *outImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            outImg[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            sum = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    sum += inImg[(i+k) * imgWidth + (j+l)];
                }
            }

            //sum = lpfImg[i * imgWidth + j] 
            //lpf
            sum = (int)((float)sum / (window * window) + 0.5);
            //hpf
            v = (int)(inImg[i * imgWidth + j] + (0.3 * (inImg[i * imgWidth + j] - sum)) + 0.5);

            //clipping
            if (v > 255) {
                v = 255;
            }
            else if(v < 0) {
                v = 0;
            }

            outImg[i * imgWidth + j] = v;
        }
    }

    //save img
    writeImg(outImg, imgWidth, imgHeight, "Pentagon_unshrap.pgm"); 

    return;
}

void interpolation() {

    //get img size by bmp
    struct Size size;
    size = getSize("Lena.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("Lena.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    //set octave (2배 = 1octave)
    int octave = 2;
    int newWidth = imgWidth * octave - 1;
    int newHeight = imgHeight * octave - 1;

    //zero-order zooming
    BYTE *zeroorderImg = (BYTE *)malloc(newWidth * newHeight * sizeof(char));
    for(int i = 0; i < newHeight; i++) {
        for(int j = 0; j < newWidth; j++) {
            zeroorderImg[i * newWidth + j] = inImg[(i/2) * imgWidth + (j/2)];
        }
    }

    //first-order zooming
    BYTE *firstorderImg = (BYTE *)malloc(newHeight * newWidth * sizeof(char));
    int avg;

    for(int i = 0; i < newHeight; i++) {
        for(int j = 0; j < newWidth; j++) {
            if((i % 2 == 0) && (j % 2 == 0)) {
                firstorderImg[i * newWidth + j] = inImg[(i/2) * imgWidth + (j/2)];
            }

            else if((i % 2 == 0) && (j % 2 != 0)) {
                avg = (int)((float)(inImg[(i/2) * imgWidth + (j/2)] + inImg[(i/2) * imgWidth + (j/2 + 1)]) / 2 + 0.5);
                firstorderImg[i * newWidth + j] = avg;
            }

            else if((i % 2 != 0) && (j % 2 == 0)) {
                avg = (int)((float)(inImg[(i/2) * imgWidth + (j/2)] + inImg[(i/2 + 1) * imgWidth + (j/2)]) / 2 + 0.5);
                firstorderImg[i * newWidth + j] = avg;
            }

            else if((i % 2 != 0) && (j % 2 != 0)) {
                avg = (int)((float)(inImg[(i/2) * imgWidth + (j/2)] + inImg[(i/2 + 1) * imgWidth + (j/2 + 1)]) / 2 + 0.5);
                firstorderImg[i * newWidth + j] = avg;
            }

            else {
                printf("\nerr!\n");
            }
        }
    }

    //save img
    writeImg(zeroorderImg, newWidth, newHeight, "Lena_zero-order.pgm"); 
    writeImg(firstorderImg, newWidth, newHeight, "Lena_first-order.pgm"); 

    return;
}

int main(void) {

    hist_spec(); //public square
    avg_filter(); //snow - 2D, 1D
    integ_avg(); //snow - integ
    unsharp_mask(); //pentagon
    interpolation(); //lena - zero, first order

    printf("\n==program exited==\n");

    return 0;
}