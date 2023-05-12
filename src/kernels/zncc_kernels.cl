// Define constants
#define MAX_DISP 32
#define WIN_SIZE 11
// #define CC_THRESHOLD MAX_DISP / 8
// #define OCC_THRESHOLD CC_THRESHOLD / 2

// Define kernel for calculating mean
double calculateMean(int x, int y, int d, __global const unsigned char* img, int width, int height)
{
    const int numPixels = WIN_SIZE * WIN_SIZE;
    const int halfWinSize = WIN_SIZE / 2;
    double sum = 0.0;
    int count = 0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int i = idx % WIN_SIZE - halfWinSize;
        int j = idx / WIN_SIZE - halfWinSize;

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

    return sum / (double)count;
}

// Define kernel for calculating ZNCC
double calculateZncc(int x, int y, int d, double mean1, double mean2, __global const unsigned char* img1, __global const unsigned char* img2, int width, int height)
{
    const int numPixels = WIN_SIZE * WIN_SIZE;
    const int halfWinSize = WIN_SIZE / 2;
    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int i = idx % WIN_SIZE - halfWinSize;
        int j = idx / WIN_SIZE - halfWinSize;

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

    double denom = sqrt(denom1 * denom2);

    if (denom == 0.0)
    {
        return 0.0;
    }

    return num / denom;
}

// Kernel for ZNCC disparity calculation
__kernel void zncc_kernel(global const unsigned char* leftImg,
                        global const unsigned char* rightImg,
                        global unsigned char* disparityImg,
                        int width, int height)
{
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;

    double maxZncc = -1.0;
    int bestDisp = 0;

    double mean1 = calculateMean(x, y, 0, leftImg, width, height);

    for (int d = 0; d < MAX_DISP; d++)
    {
        double mean2 = calculateMean(x, y, d, rightImg, width, height);

        double znccVal = calculateZncc(x, y, d, mean1, mean2, leftImg, rightImg, width, height);

        if (znccVal > maxZncc)
        {
            maxZncc = znccVal;
            bestDisp = d;
        }
    }

    disparityImg[idx] = (unsigned char)bestDisp;
    }
