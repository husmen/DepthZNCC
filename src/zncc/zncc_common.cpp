#include "zncc_common.hpp"

string ZnccMethodToString(ZnccMethod method)
{
    auto it = ZnccString.find(method);
    return it != ZnccString.end() ? it->second : "unknown";	
}

double calculateMean(int x, int y, int d, const vector<unsigned char> &img, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.winSize * znccParams.winSize;
    const int halfWinSize = znccParams.winSize / 2;
    double sum = 0.0;
    int count = 0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int i = idx % znccParams.winSize - halfWinSize;
        int j = idx / znccParams.winSize - halfWinSize;

        int xj = x + i;
        int yj = y + j;
        if (xj >= 0 && xj < znccParams.width && yj >= 0 && yj < znccParams.height)
        {
            int xjd = xj - d;
            if (xjd >= 0 && xjd < znccParams.width)
            {
                sum += img[yj * znccParams.width + xjd];
                count++;
            }
        }
    }

    return sum / static_cast<double>(count);
}

double calculateZncc(int x, int y, int d, double mean1, double mean2, const vector<unsigned char> &img1, const vector<unsigned char> &img2, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.winSize * znccParams.winSize;
    const int halfWinSize = znccParams.winSize / 2;
    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int i = idx % znccParams.winSize - halfWinSize;
        int j = idx / znccParams.winSize - halfWinSize;

        int xj1 = x + i;
        int xj2 = x + i - d;
        int yj = y + j;
        if (xj1 >= 0 && xj1 < znccParams.width && xj2 >= 0 && xj2 < znccParams.width && yj >= 0 && yj < znccParams.height)
        {
            double val1 = img1[yj * znccParams.width + xj1] - mean1;
            double val2 = img2[yj * znccParams.width + xj2] - mean2;
            num += val1 * val2;
            denom1 += val1 * val1;
            denom2 += val2 * val2;
        }
    }

    double denom = sqrt(denom1 * denom2);

    return denom == 0.0 ? 0.0 : num / denom;
}

// #pragma omp declare simd
double calculateMeanSimd(int x, int y, int d, int width, int height, int halfWinSize, const vector<unsigned char> &img)
{
    int yy_0 = max(d, y - halfWinSize);
    int yy_1 = min(height - d, y + halfWinSize);
    int xx_0 = max(d, x - halfWinSize);
    int xx_1 = min(width - d, x + halfWinSize);

    double sum = 0.0;
#ifdef USE_SIMD
#pragma omp simd reduction(+:sum) collapse(2)
#endif
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

// #pragma omp declare simd
double calculateZnccSimd(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, const vector<unsigned char> &img1, const vector<unsigned char> &img2)
{
    int yy_0 = max(d, y - halfWinSize);
    int yy_1 = min(height - d, y + halfWinSize);
    int xx_0 = max(d, x - halfWinSize);
    int xx_1 = min(width - d, x + halfWinSize);

    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

#ifdef USE_SIMD
#pragma omp simd reduction(+:num, denom1, denom2) collapse(2)
#endif
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

vector<unsigned char> crosscheck(const vector<unsigned char> &dispMapLeft, const vector<unsigned char> &dispMapRight, const ZnccParams &znccParams)
{
    cout << "## Cross checking\n";
    vector<unsigned char> result(dispMapLeft);

    // Loop over all pixels
    for (int idx = 0; idx < znccParams.width * znccParams.height; idx++)
    {
        int y = idx / znccParams.width;
        int x = idx % znccParams.width;

        // Get the disparities from both depth maps
        int dispLeft = dispMapLeft[idx];
        int dispRight = dispMapRight[idx];

        // Check if the disparities match
        // if (x - dispLeft >= 0 && x - dispLeft < width && abs(dispRight - dispMapLeft[idx - dispLeft]) <= CC_THRESHOLD)
        if (abs(dispRight - dispLeft) > znccParams.ccThresh)
        {
            result[idx] = 0;
        }
    }

    return result;
}

vector<unsigned char> fillOcclusion(const vector<unsigned char> &dispMap, const ZnccParams &znccParams)
{
    cout << "## Occlusion filling\n";
    vector<unsigned char> result(dispMap);

    // Loop over all pixels
    for (int idx = 0; idx < znccParams.width * znccParams.height; idx++)
    {
        int y = idx / znccParams.width;
        int x = idx % znccParams.width;

        // Get the disparity value for this pixel
        int disp = dispMap[idx];

        // If the disparity value is zero, this pixel is occluded
        if (disp == 0)
        {
            // Search for the nearest non-occluded pixel in the same row
            int left = x;
            int right = x;
            while (left >= 0 && dispMap[y * znccParams.width + left] == 0)
            {
                left--;
            }
            while (right < znccParams.width && dispMap[y * znccParams.width + right] == 0)
            {
                right++;
            }

            // Calculate a new disparity value as the average of the two nearest non-occluded pixels
            auto idx_left = max(0, y * znccParams.width + left);
            auto idx_right = min(znccParams.width - 1, y * znccParams.width + right);
            auto new_disp = (dispMap[idx_left] + dispMap[idx_right]) / 2;

            // Clamp the new disparity value to the valid range
            // new_disp = max(0, min(znccParams.maxDisp, new_disp));

            result[idx] = static_cast<unsigned char>(new_disp);
        }
    }

    return result;
}

vector<unsigned char> normalizeMap(const vector<unsigned char> &dispMap, const ZnccParams &znccParams)
{
    cout << "## Map Normalization\n";
    vector<unsigned char> normalizedMap = dispMap;

    for (auto &val : normalizedMap)
    {
        val = static_cast<unsigned char>(val * 255.0 / znccParams.maxDisp);
    }

    return normalizedMap;
}