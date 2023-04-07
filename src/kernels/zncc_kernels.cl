__kernel void calculateMean(__global const uchar *img, const int x, const int y, const int d, const int width, const int height, const int win_size, __global double *mean)
{
    const int halfWinSize = win_size / 2;
    double sum = 0.0;
    int count = 0;

    for (int j = -halfWinSize; j <= halfWinSize; j++)
    {
        for (int i = -halfWinSize; i <= halfWinSize; i++)
        {
            int xj = x + i;
            int yj = y + j;

            if (xj >= 0 && xj < width && yj >= 0 && yj < height)
            {
                int xjd = xj - d;
                if (xjd >= 0 && xjd < width)
                {
                    sum += img[yj * width + xjd];
                    count++;
                }
            }
        }
    }

    *mean = sum / (double)count;
}

__kernel void calculateZncc(int x, int y, int d, double mean1, double mean2, __global const unsigned char *img1, __global const unsigned char *img2, int width, int height, const int win_size, __global double *zncc)
{
    const int halfWinSize = win_size / 2;
    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int j = -halfWinSize; j <= halfWinSize; j++)
    {
        for (int i = -halfWinSize; i <= halfWinSize; i++)
        {
            int xj1 = x + i;
            int xj2 = x + i - d;
            int yj = y + j;
            if (xj1 >= 0 && xj1 < width && xj2 >= 0 && xj2 < width && yj >= 0 && yj < height)
            {
                double val1 = img1[yj * width + xj1] - mean1;
                double val2 = img2[yj * width + xj2] - mean2;
                num += val1 * val2;
                denom1 += val1 * val1;
                denom2 += val2 * val2;
            }
        }
    }

    double denom = sqrt(denom1 * denom2);

    *zncc = denom == 0.0 ? 0.0 : num / denom;
}

__kernel void zncc_kernel(__global const unsigned char *leftImg, __global const unsigned char *rightImg, __global unsigned char *disparityImg, const int width, const int height, const int win_size)
{
    const int MAX_DISP = 64;
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    double maxZncc = -1.0;
    int bestDisp = 0;

    __global double *mean1;
    calculateMean(leftImg, x, y, 0, width, height, win_size, mean1);

    for (int d = 0; d < MAX_DISP; d++)
    {
        __global double *mean2;
        calculateMean(rightImg, x, y, d, width, height, win_size, mean2);

        __global double *znccVal;
        calculateZncc(x, y, d, *mean1, *mean2, leftImg, rightImg, width, height, win_size, znccVal);

        if (*znccVal > maxZncc)
        {
            maxZncc = *znccVal;
            bestDisp = d;
        }
    }

    const int idx = y * width + x;
    disparityImg[idx] = (uchar)bestDisp;
}