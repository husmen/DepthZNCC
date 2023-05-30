// Define kernel for calculating mean
double calculateMean(int x, int y, int d, int width, int height, int winSize, __global const unsigned char* img)
{
    const int numPixels = winSize * winSize;
    const int halfWinSize = winSize / 2;
    int sum = 0;
    int count = 0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        // offset from center pixel
        int i = idx % winSize - halfWinSize;
        int j = idx / winSize - halfWinSize;

        // coordinates of neighbor pixel in reference image
        int xj = x + i;
        int yj = y + j;

        bool started = false;

        if (xj >= 0 && xj < width && yj >= 0 && yj < height)
        {
            // coordinates of neighbor pixel in target image
            int xjd = xj - d;
            if (xjd >= 0 && xjd < width)
            {
                sum += img[yj * width + xjd];
                count++;

                // if (x == 10 && y == 10 && d == 15)
                // {
                //     printf("%d, (%d, %d, %d): %d\n", count, xj - d, yj, d, img[yj * width + xjd]);
                // }
            }
        }
    }
    double result = (double)sum / (double)count;

    // if (x == 10 && y == 10 && d == 15)
    //     printf("Mean: %d / %d = %f\n", sum, count, result);

    return result;
}

// Define kernel for calculating ZNCC
double calculateZncc(int x, int y, int d, double mean1, double mean2, int width, int height, int winSize, __global const unsigned char* img1, __global const unsigned char* img2)
{
    const int numPixels = winSize * winSize;
    const int halfWinSize = winSize / 2;
    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        // offset from center pixel
        int i = idx % winSize - halfWinSize;
        int j = idx / winSize - halfWinSize;

        // coordinates of neighbor pixel in reference image
        int xj1 = x + i;
        int yj = y + j;

        // coordinates of neighbor pixel in target image
        int xj2 = x + i - d;

        if (xj1 >= 0 && xj1 < width && xj2 >= 0 && xj2 < width && yj >= 0 && yj < height)
        {
            double val1 = img1[yj * width + xj1] - mean1;
            double val2 = img2[yj * width + xj2] - mean2;
            // if (x == 10 && y == 10 && d == 15)
            // {
            //     printf("val1: %d - %f = %f\n", img1[yj * width + xj1], mean1, val1);
            //     printf("val2: %d - %f = %f\n", img2[yj * width + xj2], mean2, val2);
            // }
            num += val1 * val2;
            denom1 += val1 * val1;
            denom2 += val2 * val2;
        }
    }

    double denom = sqrt(denom1 * denom2);
    double result = denom == 0.0 ? 0.0 : num / denom;

    // if (x == 10 && y == 10 && d == 15)
    //     printf("ZNCC: %f / %f = %f\n", num, denom, result);

    return result;
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

    double maxZncc = -1.0;
    int bestDisp = 0;

    double mean1 = calculateMean(x, y, 0, width, height, winSize, leftImg);

    if (maxDisp > 0)
        for (int d = 0; d < maxDisp; d++)
        {
            double mean2 = calculateMean(x, y, d, width, height, winSize, rightImg);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, width, height, winSize, leftImg, rightImg);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = d;
            }
        }
    else
        for (int d = 0; d > maxDisp; d--)
        {
            double mean2 = calculateMean(x, y, d, width, height, winSize, rightImg);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, width, height, winSize, leftImg, rightImg);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = -d;
            }
        }

    disparityImg[idx] = (unsigned char)bestDisp;
    }
