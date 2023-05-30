#include "zncc_cuda.cuh"

// Define kernel for calculating mean
__device__ double calculateMean(int x, int y, int d, int width, int height, int halfWinSize, const unsigned char* img)
{
    int yy_0 = max(d, y - halfWinSize);
    int yy_1 = min(height, y + halfWinSize);
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
__device__ double calculateZncc(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, const unsigned char* img1, const unsigned char* img2)
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
    double result = denom == 0.0 ? 0.0 : num / denom;
    return result;
}

// Kernel for ZNCC disparity calculation
__global__ void zncc_kernel(const unsigned char* leftImg,
                        const unsigned char* rightImg,
                        unsigned char* disparityImg,
                        double* meanVals,
                        double* znccVals,
                        int width, int height, int winSize, int maxDisp)
{
    // Get global thread ID
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
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

void zncc_cuda(unsigned char* dispMap, const unsigned char* leftImg, const unsigned char* rightImg, int width, int height, int winSize, int maxDisp)
{
    // Allocate device memory
    unsigned char* d_leftImg;
    unsigned char* d_rightImg;
    unsigned char* d_disparityImg;
    double* d_meanVals;
    double* d_znccVals;

    cudaMalloc((void**)&d_leftImg, width * height * sizeof(unsigned char));
    cudaMalloc((void**)&d_rightImg, width * height * sizeof(unsigned char));
    cudaMalloc((void**)&d_disparityImg, width * height * sizeof(unsigned char));
    cudaMalloc((void**)&d_meanVals, maxDisp * sizeof(double));
    cudaMalloc((void**)&d_znccVals, maxDisp * sizeof(double));

    // Copy input data from host to device
    cudaMemcpy(d_leftImg, leftImg, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_rightImg, rightImg, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice);

    // Launch the CUDA kernel
    dim3 blockSize(256);  // Adjust the block size as needed
    dim3 gridSize((width * height + blockSize.x - 1) / blockSize.x);
    zncc_kernel<<<gridSize, blockSize>>>(d_leftImg, d_rightImg, d_disparityImg, d_meanVals, d_znccVals, width, height, winSize, maxDisp);

    // Copy output data from device to host
    cudaMemcpy(dispMap, d_disparityImg, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_leftImg);
    cudaFree(d_rightImg);
    cudaFree(d_disparityImg);
    cudaFree(d_meanVals);
    cudaFree(d_znccVals);
}
