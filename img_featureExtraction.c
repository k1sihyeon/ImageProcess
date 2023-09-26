#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>

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



void edge_extraction() {
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

    
    ////////////////////////////////////////////////////////////////
    //sobel 1-(1)
    int window = 3;
    int halfW = window / 2;
    int sumX, sumY = 0;
    int min = (int)10e10;
    int max = (int)-10e10;

    BYTE *sobelImg = (BYTE *)malloc(imgSize * sizeof(char));

    int xmask[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int ymask[3][3] = {
        {-1, -2, -1}, 
        {0, 0, 0},
        {1, 2, 1}
    };

        //sobel mask
    for(int i = halfW; i < imgHeight - halfW; i++) {
        for(int j = halfW; j < imgWidth - halfW; j++) {

            sumX = 0;
            sumY = 0;

            for(int k = -halfW; k <= halfW; k++) {
                for(int l = -halfW; l <= halfW; l++) {
                    
                    sumX += (xmask[(k + halfW)][(l + halfW)] * inImg[(i+k) * imgWidth + (j+l)]);
                    sumY += (ymask[(k + halfW)][(l + halfW)] * inImg[(i+k) * imgWidth + (j+l)]);

                }
            }

            sobelImg[i * imgWidth + j] = abs(sumX) + abs(sumY);

        }
    }

        //find min, max
    for(int i = halfW; i < imgHeight - halfW; i++) {
        for(int j = halfW; j < imgWidth - halfW; j++) {

            if (min > sobelImg[i * imgWidth + j])
                min = sobelImg[i * imgWidth + j];

            if (max < sobelImg[i * imgWidth + j])
                max = sobelImg[i * imgWidth + j];
        }
    }

        //normalization
    for(int i = halfW; i < imgHeight - halfW; i++) {
        for(int j = halfW; j < imgWidth - halfW; j++) {
            sobelImg[i * imgWidth + j] = (int)(255. * sobelImg[i * imgWidth + j] - min) / ((max - min) + 0.5);
        }
    }

    writeImg(sobelImg, imgWidth, imgHeight, "1_1_Lena_sobel.pgm");


    ////////////////////////////////////////////////////////////////
    //nonlinear gradient 1-(2)
    window = 5;

    BYTE *ng1Img = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *ng2Img = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {
            ng1Img[i * imgWidth + j] = inImg[i * imgWidth + j];
            ng2Img[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {

            max = (int)-10e10;
            min = (int)10e10;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {

                    if(min > ng2Img[(i+k) * imgWidth + (j+l)]) {
                        min = ng2Img[(i+k) * imgWidth + (j+l)];
                    }

                    if(max < ng1Img[(i+k) * imgWidth + (j+l)]) {
                        max = ng1Img[(i+k) * imgWidth + (j+l)];
                    }
                }
            }

            ng1Img[i * imgWidth + j] = max - ng1Img[i * imgWidth + j];
            ng2Img[i * imgWidth + j] = ng2Img[i * imgWidth + j] - min ;
        }
    }

    writeImg(ng1Img, imgWidth, imgHeight, "1_2_Lena_nlg_max.pgm");
    writeImg(ng2Img, imgWidth, imgHeight, "1_2_Lena_nlg_min.pgm");


    ////////////////////////////////////////////////////////////////
    //nonlinear Laplacian 1-(3)
    window = 5;
    BYTE *laplImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            laplImg[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {

            max = (int)-10e10;
            min = (int)10e10;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {

                    if(min > laplImg[(i+k) * imgWidth + (j+l)])
                        min = laplImg[(i+k) * imgWidth + (j+l)];

                    if(max < laplImg[(i+k) * imgWidth + (j+l)])
                        max = laplImg[(i+k) * imgWidth + (j+l)];
                    
                }
            }

            laplImg[i * imgWidth + j] = max + min - 2 * laplImg[i * imgWidth + j];
        }
    }

     writeImg(laplImg, imgWidth, imgHeight, "1_3_Lena_nllapl.pgm");


    ////////////////////////////////////////////////////////////////
    // Laplacian thresholding 1-(4)
    window = 5;
    int sum = 0;
    int avg = 0;
    int v = 0;
    BYTE *laplthImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            sum = 0;
            v = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    sum += laplImg[(i+k) * imgWidth + (j+l)];
                }
            }

            avg = (int)((float)sum / (window * window) + 0.5);

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    v += (laplImg[i * imgWidth + j] - avg) * (laplImg[i * imgWidth + j] - avg);
                }
            }

            v /= window * window;

            if (v < avg)
                laplthImg[i * imgWidth + j] = 255;
            else
                laplthImg[i * imgWidth + j] = v;

        }
    }

    writeImg(laplthImg, imgWidth, imgHeight, "1_4_Lena_lapl_thres.pgm");

    ////////////////////////////////////////////////////////////////
    // Entropy sketch operator 1-(5)
    window = 5;
    sum = 0;
    int ent = 0;
    int tmp = 0;
    BYTE *entImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = 0; i < imgHeight; i++) {
        for(int j = 0; j < imgWidth; j++) {
            entImg[i * imgWidth + j] = inImg[i * imgWidth +j];
        }
    }

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            sum = 0;
            ent = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    sum += inImg[(i+k) * imgWidth + (j+l)];
                }
            }

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    tmp = (int)((float)inImg[(i+k) * imgWidth + (j+l)] / sum);
                    ent += tmp * (log10(tmp) / log10(2));
                }
            }

            entImg[i * imgWidth + j] = ent;

        }
    }

    max = (int)-10e10;
    min = (int)10e10;

    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {

            if (min > entImg[i * imgWidth + j])
                min = entImg[i * imgWidth + j];

            if (max < entImg[i * imgWidth + j])
                max = entImg[i * imgWidth + j];
        }
    }

        //normalization
    for(int i = halfW; i < imgHeight - halfW; i++) {
        for(int j = halfW; j < imgWidth - halfW; j++) {
            entImg[i * imgWidth + j] = (int)(255. * entImg[i * imgWidth + j] - min) / ((max - min) + 0.5);
        }
    }

    writeImg(entImg, imgWidth, imgHeight, "1_5_Lena_ent.pgm");


    ////////////////////////////////////////////////////////////////
    //DP, DIP 1-(6),(7)
    window = 5;
    sum = avg = 0;
    BYTE *dpImg = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *dipImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            dpImg[i * imgWidth + j] = inImg[i * imgWidth + j];
        }
    }


    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            
            sum = 0;
            max = (int)-10e10;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {

                    if(max < inImg[(i+k) * imgWidth + (j+l)])
                        max = inImg[(i+k) * imgWidth + (j+l)];
                    
                    sum += inImg[(i+k) * imgWidth + (j+l)];
                }
            }

            avg = (int)((float)sum / (window * window) + 0.5);

            dpImg[i * imgWidth + j] = (int)((float)max / avg - (float)dpImg[i * imgWidth + j] / avg + 0.5);
            dipImg[i * imgWidth + j] = dpImg[i * imgWidth + j] * (avg / max) * (avg / inImg[i * imgWidth + j]);
        }
    }

    writeImg(dpImg, imgWidth, imgHeight, "1_6_Lena_dp.pgm");
    writeImg(dipImg, imgWidth, imgHeight, "1_7_Lena_dip.pgm");

    return;
}

void harris_coner_dectector() {
    //get img size by bmp
    struct Size size;
    size = getSize("Ctest.bmp");
    int imgWidth = size.width;
    int imgHeight = size.height;
    int imgSize = size.size;

    //get raw img
    FILE *infile = fopen("Ctest.raw", "rb");
    BYTE *inImg = (BYTE *)malloc(imgSize * sizeof(char));
    fread(inImg, sizeof(char), imgSize, infile);
    fclose(infile);

    BYTE *x2Img = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *y2Img = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *xyImg = (BYTE *)malloc(imgSize * sizeof(char));

    BYTE *x2eqImg = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *y2eqImg = (BYTE *)malloc(imgSize * sizeof(char));
    BYTE *xyeqImg = (BYTE *)malloc(imgSize * sizeof(char));

    int window = 3;
    int sumX = 0, sumY = 0, sumXY = 0;

    int minX = (int)10e10, maxX = (int)-10e10;
    int minY = (int)10e10, maxY = (int)-10e10;
    int minXY = (int)10e10, maxXY = (int)-10e10;

    int xmask[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    int ymask[3][3] = {
        {-1, -2, -1}, 
        {0, 0, 0},
        {1, 2, 1}
    };

    ////////////////////////////////////////////////////////////////
    //sobel 2-(1)
    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {

            sumX = 0;
            sumY = 0;
            sumXY = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    
                    sumX += (xmask[(k + window/2)][(l + window/2)] * inImg[(i+k) * imgWidth + (j+l)]);
                    sumY += (ymask[(k + window/2)][(l + window/2)] * inImg[(i+k) * imgWidth + (j+l)]);

                }
            }

            x2Img[i * imgWidth + j] = sumX * sumX;
            y2Img[i * imgWidth + j] = sumY * sumY;
            xyImg[i * imgWidth + j] = abs(sumX) * abs(sumY);

        }
    }

    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {

            if (minX > x2Img[i * imgWidth + j])
                minX = x2Img[i * imgWidth + j];

            if (maxX < x2Img[i * imgWidth + j])
                maxX = x2Img[i * imgWidth + j];

            if (minY > y2Img[i * imgWidth + j])
                minY = y2Img[i * imgWidth + j];

            if (maxY < y2Img[i * imgWidth + j])
                maxY = y2Img[i * imgWidth + j];

            if (minXY > xyImg[i * imgWidth + j])
                minXY = xyImg[i * imgWidth + j];

            if (maxXY < xyImg[i * imgWidth + j])
                maxXY = xyImg[i * imgWidth + j];
        }
    }

        //normalization
    for(int i = window/2; i < imgHeight - window/2; i++) {
        for(int j = window/2; j < imgWidth - window/2; j++) {
            x2Img[i * imgWidth + j] = (int)(255. * x2Img[i * imgWidth + j] - minX) / ((maxX - minX) + 0.5);
            y2Img[i * imgWidth + j] = (int)(255. * y2Img[i * imgWidth + j] - minY) / ((maxY - minY) + 0.5);
            xyImg[i * imgWidth + j] = (int)(255. * xyImg[i * imgWidth + j] - minX) / ((maxXY - minXY) + 0.5);
        }
    }

    writeImg(x2Img, imgWidth, imgHeight, "2_1_Ctest_x2.pgm");
    writeImg(y2Img, imgWidth, imgHeight, "2_1_Ctest_y2.pgm");
    writeImg(xyImg, imgWidth, imgHeight, "2_1_Ctest_xy.pgm");


    ////////////////////////////////////////////////////////////////
    //equalization 2-(2)
    window = 5;
    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            sumX = 0;
            sumY = 0;
            sumXY = 0;

            for(int k = -window/2; k <= window/2; k++) {
                for(int l = -window/2; l <= window/2; l++) {
                    sumX += x2Img[(i+k) * imgWidth + (j+l)];
                    sumY += y2Img[(i+k) * imgWidth + (j+l)];
                    sumXY += xyImg[(i+k) * imgWidth + (j+l)];
                }
            }

            x2eqImg[i * imgWidth + j] = (int)((float)sumX / (window * window) + 0.5);
            y2eqImg[i * imgWidth + j] = (int)((float)sumY / (window * window) + 0.5);
            xyeqImg[i * imgWidth + j] = (int)((float)sumXY / (window * window) + 0.5);
        }
    }

    writeImg(x2eqImg, imgWidth, imgHeight, "2_2_Ctest_x2_eq.pgm");
    writeImg(y2eqImg, imgWidth, imgHeight, "2_2_Ctest_y2_eq.pgm");
    writeImg(xyeqImg, imgWidth, imgHeight, "2_2_Ctest_xy_eq.pgm");


    ////////////////////////////////////////////////////////////////
    //Harris Corner Response Function 2-(3)
    int idx = 0;
    float lamda = 0.05;
    float val = 0.0;
    BYTE *crfImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {
            idx = i * imgWidth + j;
            val = (x2eqImg[idx] * y2eqImg[idx]) - xyeqImg[idx] - (lamda * (x2eqImg[idx] + y2eqImg[idx]) * (x2eqImg[idx] + y2eqImg[idx]));
            crfImg[idx] = (int)(val + 0.5);
        }
    }

    writeImg(crfImg, imgWidth, imgHeight, "2_3_Ctest_CRF.pgm");


    ////////////////////////////////////////////////////////////////
    //CRF thresholding 2-(4)
    BYTE *crfthImg = (BYTE *)malloc(imgSize * sizeof(char));

    for(int i = window/2; i < imgHeight - window/2; i++){
        for(int j = window/2; j < imgWidth - window/2; j++) {

            idx = i * imgWidth + j;

            if (crfImg[idx] > 66)
                crfthImg[idx] = 255;
            else
                crfthImg[idx] = 0;
        }
    }

    writeImg(crfthImg, imgWidth, imgHeight, "2_4_Ctest_CRF_thres.pgm");

    return;
}

int main(void) {

    edge_extraction();
    harris_coner_dectector();

    return 0;
}