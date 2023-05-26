// Define kernel for calculating mean
double calculateMean(int x, int y, int d, int width, int height, int halfWinSize, __global const unsigned char* img)
{
    int yy_0 = max(d, y - halfWinSize);
    int yy_1 = min(height - d, y + halfWinSize);
    int xx_0 = max(d, x - halfWinSize);
    int xx_1 = min(width - d, x + halfWinSize);

    double sum = 0.0;
    for (int yy = yy_0; yy < yy_1; yy++)
    {
        for (int xx = xx_0; xx < xx_1; xx++)
        {
            sum += img[yy * width + xx - d];
        }
    }

    int count = (xx_1 - xx_0) * (yy_1 - yy_0);
    return sum / (double)count;
}

// Define kernel for calculating ZNCC
double calculateZncc(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, __global const unsigned char* img1, __global const unsigned char* img2)
{
    int yy_0 = max(d, y - halfWinSize);
    int yy_1 = min(height - d, y + halfWinSize);
    int xx_0 = max(d, x - halfWinSize);
    int xx_1 = min(width - d, x + halfWinSize);

    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int yy = yy_0; yy < yy_1; yy++)
    {
        for (int xx = xx_0; xx < xx_1; xx++)
        {
            double val1 = img1[yy * width + xx] - mean1;
            double val2 = img2[yy * width + xx - d] - mean2;
            num += val1 * val2;
            denom1 += val1 * val1;
            denom2 += val2 * val2;
        }
    }

    double denom = sqrt(denom1 * denom2);
    return denom == 0.0 ? 0.0 : num / denom;
}

// Kernel for ZNCC disparity calculation
__kernel void zncc_kernel(global const unsigned char* leftImg,
                        global const unsigned char* rightImg,
                        global unsigned char* disparityImg,
                        global double* meanVals,
                        global double* znccVals,
                        int width, int height, int winSize, int maxDisp)
{
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;

    double maxZncc = -1.0;
    int bestDisp = 0;

    double meanVals_0 = calculateMean(x, y, 0, width, height, winSize / 2, leftImg);

    for (int d = 0; d < maxDisp; d++)
    {
        meanVals[d] = calculateMean(x, y, d, width, height, winSize / 2, rightImg);
    }

    for (int d = 0; d < maxDisp; d++)
    {
        znccVals[d] = calculateZncc(x, y, d, meanVals_0, meanVals[d], width, height, winSize / 2, leftImg, rightImg);
    }

    for (int d = 0; d < maxDisp; d++)
    {
        if (znccVals[d] > maxZncc)
        {
            maxZncc = znccVals[d];
            bestDisp = d;
        }
    }

    disparityImg[idx] = (unsigned char)bestDisp;
}