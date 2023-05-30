__kernel void calculateMean(int x, int y, int d, int width, int height, int halfWinSize, __global const unsigned char* img, __local double* mean)
{
    int yy_0 = max(0, y - halfWinSize);
    int yy_1 = min(height, y + halfWinSize + 1);
    int xx_0 = max(0, x - halfWinSize);
    int xx_1 = min(width, x + halfWinSize + 1);

    __local unsigned char workImg[64 * 64];

    int count = 0;

    // barrier(CLK_LOCAL_MEM_FENCE);
    for (int yy = yy_0; yy < yy_1; yy++)
    {
        for (int xx = xx_0; xx < xx_1; xx++)
        {
            int xxd = xx - d;
            if (xxd >= 0 && xxd < width)
            {
                workImg[count++] = img[yy * width + xxd];
                // if (x == 10 && y == 10 && d == 15)
                // {
                //     printf("%d, (%d, %d, %d): %d\n", count, xxd, yy, d, img[yy * width + xxd]);
                // }
            }
        }
    }
    // barrier(CLK_LOCAL_MEM_FENCE);

    int sum = 0;

    for (int idx = 0; idx < count; idx++)
    {
        sum += workImg[idx];
    }
    double result = (double)sum / (double)count;
    *mean = result;

//    if (x == 10 && y == 10 && d == 15)
//         printf("Mean: %d / %d = %f (%f)\n", sum, count, result, *mean);
}

// Define kernel for calculating ZNCC
// double calculateZncc(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, __global const unsigned char* img1, __global const unsigned char* img2)
// {
//     int yy_0 = max(max(0, y - halfWinSize), d);
//     int yy_1 = min(min(height, y + halfWinSize + 1), height + d);
//     int xx_0 = max(max(0, x - halfWinSize), d);
//     int xx_1 = min(min(width, x + halfWinSize + 1), width + d);

//     double num = 0.0;
//     double denom1 = 0.0;
//     double denom2 = 0.0;

//     for (int yy = yy_0; yy < yy_1; yy++)
//     {
//         for (int xx = xx_0; xx < xx_1; xx++)
//         {
//             double val1 = img1[yy * width + xx] - mean1;
//             double val2 = img2[yy * width + xx - d] - mean2;
//             num += val1 * val2;
//             denom1 += val1 * val1;
//             denom2 += val2 * val2;
//         }
//     }

//     double denom = sqrt(denom1 * denom2);
//     return denom == 0.0 ? 0.0 : num / denom;
// }

__kernel void calculateZncc2(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, __global const unsigned char* img1, __global const unsigned char* img2, __local double* zncc)
{
    int yy_0 = max(0, y - halfWinSize);
    int yy_1 = min(height, y + halfWinSize + 1);
    int xx_0 = max(0, x - halfWinSize);
    int xx_1 = min(width, x + halfWinSize + 1);

    __local unsigned char workImg1[64 * 64];
    __local unsigned char workImg2[64 * 64];

    int count = 0;
    for (int yy = yy_0; yy < yy_1; yy++)
    {
        for (int xx = xx_0; xx < xx_1; xx++)
        {
            int xxd = xx - d;
            if (xxd >= 0 && xxd < width)
            {
                workImg1[count] = img1[yy * width + xx];
                workImg2[count++] = img2[yy * width + xxd];
            }
        }
    }

    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int idx = 0; idx < count; idx++)
    {
            double val1 = workImg1[idx] - mean1;
            double val2 = workImg2[idx] - mean2;
            // if (x == 10 && y == 10 && d == 15)
            // {
            //     printf("val1: %d - %f = %f\n", workImg1[idx], mean1, val1);
            //     printf("val2: %d - %f = %f\n", workImg2[idx], mean2, val2);
            // }
            num += val1 * val2;
            denom1 += val1 * val1;
            denom2 += val2 * val2;
    }

    double denom = sqrt(denom1 * denom2);
    *zncc = denom == 0.0 ? 0.0 : num / denom;

    // if (x == 10 && y == 10 && d == 15)
    //     printf("ZNCC: %f / %f = %f\n", num, denom, *zncc);
}

// Kernel for ZNCC disparity calculation
__kernel void zncc_kernel(global const unsigned char* leftImg,
                        global const unsigned char* rightImg,
                        global unsigned char* leftDispImg,
                        global unsigned char* rightDispImg,
                        int width, int height, int winSize, int maxDisp)
{
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;
    int halfWinSize = winSize / 2;

    __local double leftMeanVals[128];
    __local double rightMeanVals[128];
    __local double leftZnccVals[128];
    __local double rightZnccVals[128];

    for (int d = 0; d < maxDisp; d++)
    {
        // if (x == 10 && y == 10 && d == 15)
        //     printf("Mean: %f\n", leftMeanVals[d]);
        calculateMean(x, y, d, width, height, halfWinSize, leftImg, &leftMeanVals[d]);
        // if (x == 10 && y == 10 && d == 15)
        //     printf("Mean: %f\n", leftMeanVals[d]);
    }

    for (int d = 0; d < maxDisp; d++)
    {
        // if (x == 10 && y == 10 && d == 15)
        //     printf("Mean: %f\n", rightMeanVals[d]);
        calculateMean(x, y, d, width, height, halfWinSize, rightImg, &rightMeanVals[d]);
        // if (x == 10 && y == 10 && d == 15)
        //     printf("Mean: %f\n", rightMeanVals[d]);
    }

    double leftMaxZncc = -1.0;
    double rightMaxZncc = -1.0;
    int leftBestDisp = 0;
    int rightBestDisp = 0;

    for (int d = 0; d < maxDisp; d++)
    {
        // if (x == 10 && y == 10 && d == 15)
        //     printf("ZNCC: %f\n", leftZnccVals[d]);

        calculateZncc2(x, y, d, leftMeanVals[0], rightMeanVals[d], width, height, winSize / 2, leftImg, rightImg, &leftZnccVals[d]);
        calculateZncc2(x, y, -d, rightMeanVals[0], leftMeanVals[d], width, height, winSize / 2, leftImg, rightImg, &rightZnccVals[d]);
        
        if (leftZnccVals[d] > leftMaxZncc)
        {
            leftMaxZncc = leftZnccVals[d];
            leftBestDisp = d;
        }

        if (rightZnccVals[d] > rightMaxZncc)
        {
            rightMaxZncc = rightZnccVals[d];
            rightBestDisp = d;
        }

        // if (x == 10 && y == 10 && d == 15)
        //     printf("ZNCC: %f\n", leftZnccVals[d]);
    }
    leftDispImg[idx] = (unsigned char)leftBestDisp;
    rightDispImg[idx] = (unsigned char)rightBestDisp;
}