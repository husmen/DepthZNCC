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


__kernel void calculateZncc(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, __global const unsigned char* img1, __global const unsigned char* img2, __local double* zncc)
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
                        global unsigned char* disparityImg,
                        int width, int height, int winSize, int maxDisp)
{
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;
    int halfWinSize = winSize / 2;

    __local double mean1;
    __local double meanVals[128];
    __local double znccVals[128];

    double maxZncc = -1.0;
    int bestDisp = 0;

    calculateMean(x, y, 0, width, height, halfWinSize, leftImg, &mean1);

    if (maxDisp > 0)
    {
        for (int d = 0; d < maxDisp; d++)
        {
            calculateMean(x, y, d, width, height, halfWinSize, rightImg, &meanVals[d]);
            calculateZncc(x, y, d, mean1, meanVals[d], width, height, halfWinSize, leftImg, rightImg, &znccVals[d]);

            if (znccVals[d] > maxZncc)
            {
                maxZncc = znccVals[d];
                bestDisp = d;
            }
        }
    }
    else
    {
        for (int d = 0; d > maxDisp; d--)
        {
            calculateMean(x, y, d, width, height, halfWinSize, rightImg, &meanVals[d]);
            calculateZncc(x, y, d, mean1, meanVals[-d], width, height, halfWinSize, leftImg, rightImg, &znccVals[-d]);

            if (znccVals[-d] > maxDisp)
            {
                maxDisp = znccVals[-d];
                bestDisp = -d;
            }
        }
    }

    disparityImg[idx] = (unsigned char)bestDisp;
}