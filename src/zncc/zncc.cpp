#include "zncc.hpp"

double calculateMean(int x, int y, int d, const vector<unsigned char> &img, int width, int height)
{
    double sum = 0.0;
    int count = 0;

    for (int j = -WIN_SIZE / 2; j <= WIN_SIZE / 2; j++)
    {
        for (int i = -WIN_SIZE / 2; i <= WIN_SIZE / 2; i++)
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

    return sum / (double)count;
}

double calculateZncc(int x, int y, int d, double mean1, double mean2, const vector<unsigned char> &img1, const vector<unsigned char> &img2, int width, int height)
{
    double num = 0.0;
    double denom1 = 0.0;
    double denom2 = 0.0;

    for (int j = -WIN_SIZE / 2; j <= WIN_SIZE / 2; j++)
    {
        for (int i = -WIN_SIZE / 2; i <= WIN_SIZE / 2; i++)
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

    if (denom == 0.0)
    {
        return 0.0;
    }

    return num / denom;
}

void zncc(const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, vector<unsigned char> &disparityImg, int width, int height)
{
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            double maxZncc = -1.0;
            int bestDisp = 0;

            double mean1 = calculateMean(i, j, 0, leftImg, width, height);

            for (int d = 0; d < MAX_DISP; d++)
            {
                double mean2 = calculateMean(i, j, d, rightImg, width, height);

                double znccVal = calculateZncc(i, j, d, mean1, mean2, leftImg, rightImg, width, height);

                if (znccVal > maxZncc)
                {
                    maxZncc = znccVal;
                    bestDisp = d;
                }
            }

            auto idx = j * width + i;

            disparityImg[idx] = (unsigned char)bestDisp;

            if (idx > 0 && idx % (width * 10) == 0)
            {
                cout << "Progress " << fixed << setprecision(2) << (j + 1) / (double)height * 100 << " %, " << j + 1 << "/" << height << " rows done!" << endl;
            }
        }
    }

    // cout << "ZNCC min,max = " << (int)*min_element(disparityImg.begin(), disparityImg.end()) << "," << (int)*max_element(disparityImg.begin(), disparityImg.end()) << endl;
}

vector<unsigned char> cross_check(const vector<unsigned char>& dispMapLeft, const vector<unsigned char>& dispMapRight, int width, int height)
{
    vector<unsigned char> result(width * height);
    
    // Loop over all pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            
            // Get the disparities from both depth maps
            int dispLeft = dispMapLeft[idx];
            int dispRight = dispMapRight[idx];
            
            // Check if the disparities match
            // if (x - dispLeft >= 0 && x - dispLeft < width && abs(dispRight - dispMapLeft[idx - dispLeft]) <= CC_THRESHOLD)
            if(abs(dispRight - dispLeft) <= CC_THRESHOLD)
            {
                result[idx] = dispLeft;
            }
            else
            {
                result[idx] = 0;
            }
        }
    }
    
    return result;
}

vector<unsigned char> occlusion_fill(const vector<unsigned char>& depthMap, int width, int height)
{
    vector<unsigned char> result(width * height);
    
    // Loop over all pixels
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            
            // Get the disparity value for this pixel
            int disp = depthMap[idx];
            
            // If the disparity value is zero, this pixel is occluded
            if (disp == 0)
            {
                // Search for the nearest non-occluded pixel in the same row
                int left = x;
                int right = x;
                while (left >= 0 && depthMap[y * width + left] == 0)
                {
                    left--;
                }
                while (right < width && depthMap[y * width + right] == 0)
                {
                    right++;
                }
                
                // Calculate a new disparity value as the average of the two nearest non-occluded pixels
                auto idx_left = y * width + left;
                auto idx_right = y * width + right;

                int new_disp = (left >= 0 ? depthMap[idx_left] : right >= width ? 0 : depthMap[idx_right]);
                if (right < width && left >= 0)
                {
                    new_disp = (depthMap[idx_left] + depthMap[idx_right]) / 2;
                }
                
                // Clamp the new disparity value to the valid range
                new_disp = max(0, min(MAX_DISP, new_disp));
                
                result[idx] = new_disp;
            }
            else
            {
                result[idx] = disp;
            }
        }
    }
    
    return result;
}

void normalize(vector<unsigned char>& dispMap)
{
    for (auto &val : dispMap)
    {
        val = (unsigned char)((double)val / (double)MAX_DISP * 255.0);
    }
}

void zncc_pipeline(const vector<unsigned char> &imgLeft, const vector<unsigned char> &imgRight, vector<unsigned char> &dispMap, int width, int height, bool crossCheck, bool occlusionFill)
{
    vector<unsigned char> dispMapLeft(width * height);
    vector<unsigned char> dispMapRight(width * height);
    vector<unsigned char> dispConsolidated(width * height);
    vector<unsigned char> dispOC(width * height);

    // Compute the disparity map using ZNCC
    cout << "# ZNCC Left\n";
    zncc(imgLeft, imgRight, dispMapLeft, width, height);

    if (crossCheck)
    {
        cout << "# ZNCC right\n";
        zncc(imgRight, imgLeft, dispMapRight, width, height);

        // Cross-check the disparity map
        cout << "# Cross-checkingt\n";
        dispConsolidated = cross_check(dispMapLeft, dispMapRight, width, height);
    }
    else
    {
        dispConsolidated = dispMapLeft;
    }
    
    if (occlusionFill)
    {
        // Fill occlusions
        cout << "# Occlusion filling\n";
        dispOC = occlusion_fill(dispConsolidated, width, height);
    }
    else
    {
        dispOC = dispConsolidated;
    }
    
    // Normalize the disparity map
    cout << "# Normalization\n";
    normalize(dispOC);
    
    // Copy the result to the output
    dispMap = dispOC;
}