#include "zncc.hpp"

mutex cout_mutex;

// Single threaded ZNCC
void zncc_single(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        double mean1 = calculateMean(x, y, leftImg, znccParams);

        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            double mean2 = calculateMean(x - d, y, rightImg, znccParams);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, leftImg, rightImg, znccParams);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);

        if (idx > 0 && idx % (znccParams.width * 100) == 0)
        {
            cout << "Progress " << fixed << setprecision(2) << (idx + 1) / static_cast<double>(numPixels) * 100 << " %, " << idx + 1 << "/" << numPixels << " pixels done!" << endl;
        }
    }

    // cout << "ZNCC min,max = " << (int)*min_element(disparityImg.begin(), disparityImg.end()) << "," << (int)*max_element(disparityImg.begin(), disparityImg.end()) << endl;
}

// Multi threaded ZNCC
void zncc_multi(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int num_threads = thread::hardware_concurrency();
    const int chunk_size = znccParams.height / num_threads;

    vector<thread> threads(num_threads);

    for (int t = 0; t < num_threads; t++)
    {
        const int start_row = t * chunk_size;
        const int end_row = (t == num_threads - 1) ? znccParams.height : (t + 1) * chunk_size;

        threads[t] = thread([=, &leftImg, &rightImg, &dispMap]()
                            {
            for (int idx = start_row * znccParams.width; idx < end_row * znccParams.width; idx++) 
            {
                int j = idx / znccParams.width;
                int i = idx % znccParams.width;

                double maxZncc = -1.0;
                int bestDisp = 0;

                double mean1 = calculateMean(i, j, leftImg, znccParams);

                for (int d = 0; d < znccParams.maxDisp; d++)
                {
                    double mean2 = calculateMean(i - d, j, rightImg, znccParams);

                    double znccVal = calculateZncc(i, j, d, mean1, mean2, leftImg, rightImg, znccParams);

                    if (znccVal > maxZncc)
                    {
                        maxZncc = znccVal;
                        bestDisp = d;
                    }
                }

                dispMap[idx] = static_cast<unsigned char>(bestDisp);
            } });
    }

    for (int t = 0; t < num_threads; t++)
    {
        threads[t].join();
    }
}


void zncc_openmp(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

#pragma omp parallel for schedule(dynamic)
    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        double mean1 = calculateMean(x, y, leftImg, znccParams);

        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            double mean2 = calculateMean(x - d, y, rightImg, znccParams);

            double znccVal = calculateZncc(x, y, d, mean1, mean2, leftImg, rightImg, znccParams);

            if (znccVal > maxZncc)
            {
                maxZncc = znccVal;
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);
    }
}

#ifdef USE_SIMD
void zncc_simd(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    const int numPixels = znccParams.width * znccParams.height;

#pragma omp parallel for
    for (int idx = 0; idx < numPixels; idx++)
    {
        int x = idx % znccParams.width;
        int y = idx / znccParams.width;

        double maxZncc = -1.0;
        int bestDisp = 0;

        auto meanVals = vector<double>(znccParams.maxDisp);
        auto znccVals = vector<double>(znccParams.maxDisp);

        double mean1 = calculateMeanSimd(x, y, znccParams.width, znccParams.height, znccParams.winSize / 2, leftImg);

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            meanVals[d] = calculateMeanSimd(x - d, y, znccParams.width, znccParams.height, znccParams.winSize / 2, rightImg);
        }

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            znccVals[d] = calculateZnccSimd(x, y, d, mean1, meanVals[d], znccParams.width, znccParams.height, znccParams.winSize / 2, leftImg, rightImg);
        }

// #pragma omp parallel for simd
        for (int d = 0; d < znccParams.maxDisp; d++)
        {
            if (znccVals[d] > maxZncc)
            {
                maxZncc = znccVals[d];
                bestDisp = d;
            }
        }

        dispMap[idx] = static_cast<unsigned char>(bestDisp);
    }
}

#else
void zncc_simd(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    cout << "SIMD not enabled" << endl;
}
#endif


void zncc_cuda(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    #ifdef USE_CUDA
    zncc_cuda(&dispMap[0], &leftImg[0], &rightImg[0], znccParams.width, znccParams.height, znccParams.winSize, znccParams.maxDisp);
    #else
    cout << "# CUDA not enabled" << endl;
    #endif
}

// ZNCC wrapper function
void zncc(vector<unsigned char> &leftDispMap, vector<unsigned char> &rightDispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    #ifndef USE_OCL
    if (znccParams.method == ZnccMethod::OPENCL || znccParams.method == ZnccMethod::OPENCL_OPT)
    {
        cout << "OpenCL not enabled" << endl;
        return;
    }
    #endif

    switch (znccParams.method)
    {
    case ZnccMethod::SINGLE_THREADED:
        zncc_single(leftDispMap, leftImg, rightImg, znccParams);
        zncc_single(rightDispMap, rightImg, leftImg, znccParams);
        break;
    case ZnccMethod::MULTI_THREADED:
        zncc_multi(leftDispMap, leftImg, rightImg, znccParams);
        zncc_multi(rightDispMap, rightImg, leftImg, znccParams);
        break;
    case ZnccMethod::OPENMP:
        zncc_openmp(leftDispMap, leftImg, rightImg, znccParams);
        zncc_openmp(rightDispMap, rightImg, leftImg, znccParams);
        break;
    case ZnccMethod::SIMD:
        zncc_simd(leftDispMap, leftImg, rightImg, znccParams);
        zncc_simd(rightDispMap, rightImg, leftImg, znccParams);
        break;
    case ZnccMethod::OPENCL:
        zncc_opencl(leftDispMap, leftImg, rightImg, znccParams, false);
        zncc_opencl(rightDispMap, rightImg, leftImg, znccParams, true);
        break;
    case ZnccMethod::OPENCL_OPT1:
        zncc_opencl_opt1(leftDispMap, leftImg, rightImg, znccParams);
        zncc_opencl_opt1(rightDispMap, rightImg, leftImg, znccParams);
        break;
    case ZnccMethod::OPENCL_OPT:
        zncc_opencl_opt(leftDispMap, leftImg, rightImg, znccParams, false);
        zncc_opencl_opt(rightDispMap, rightImg, leftImg, znccParams, true);
        break;
    case ZnccMethod::OPENCL_OPT3:
        zncc_opencl_opt3(leftDispMap, rightDispMap, leftImg, rightImg, znccParams);
        break;
    // case ZnccMethod::OPENCL_PIPE:
    //     zncc_opencl_pipe(leftDispMap, leftImg, rightImg, znccParams);
    //     break;
    case ZnccMethod::CUDA:
        zncc_cuda(leftDispMap, leftImg, rightImg, znccParams);
        zncc_cuda(rightDispMap, rightImg, leftImg, znccParams);
        break;
    }
}

// ZNCC pipeline
ZnccResult zncc_pipeline(const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams)
{
    int numPixels = znccParams.width * znccParams.height;
    ZnccResult znccResult;
    znccResult.dispMap = vector<unsigned char>(numPixels);
    znccResult.dispMapLeft = vector<unsigned char>(numPixels);
    znccResult.dispMapRight = vector<unsigned char>(numPixels);


    // Compute the disparity map using ZNCC
    cout << "## ZNCC ...\n";
    {
        Timer timer;
        zncc(znccResult.dispMapLeft, znccResult.dispMapRight, leftImg, rightImg, znccParams);
        znccResult.znccTime = timer.getDuration();
    }

    return znccResult;
}

void post_proc_pipeline(ZnccResult &result, ZnccParams &params)
{
    cout << "## Postprocessing ...\n";
    {
        Timer timer;
        
        // Apply cross checking if enabled
        result.dispMapCC = params.withCrossChecking ? crosscheck(result.dispMapLeft, result.dispMapRight, params) : result.dispMapLeft;

        // Apply occlusion filling if enabled
        result.dispMapOC = params.withOcclusionFilling ? fillOcclusion(result.dispMapCC, params) : result.dispMapCC;

        // Normalize the disparity map if enabled
        if(params.withNormalization)
        {
            result.dispMap = normalizeMap(result.dispMapCC, params);
            result.dispMapLeft = normalizeMap(result.dispMapLeft, params);
            result.dispMapRight = normalizeMap(result.dispMapRight, params);
        }
        else
        {
            result.dispMap = result.dispMapOC;
        }

        result.postProcTime = timer.getDuration();
    }
}