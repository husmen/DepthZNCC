#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <omp.h>

using namespace std;

// ZNCC methods enum and its helpers
enum class ZnccMethod
{
    SINGLE_THREADED,
    MULTI_THREADED,
    OPENMP,
    SIMD,
    OPENCL_1,
    OPENCL_2,
    OPENCL_3,
    CUDA
};

struct ZnccParams
{
    int width;
    int height;
    int maxDisp;
    int winSize;
    int ccThresh;
    int occThresh;
    int resizeFactor;
    bool withRight;
    bool withCrossChecking;
    bool withOcclusionFilling;
    bool withNormalization;
    ZnccMethod method;
    int platformId;
};

const map<ZnccMethod, string> ZnccString = {
    {ZnccMethod::SINGLE_THREADED, "SINGLE_THREADED"},
    {ZnccMethod::MULTI_THREADED, "MULTI_THREADED"},
    {ZnccMethod::OPENMP, "OPENMP"},
    {ZnccMethod::SIMD, "SIMD"},
    {ZnccMethod::OPENCL_1, "OPENCL_1"},
    {ZnccMethod::OPENCL_2, "OPENCL_2"},
    {ZnccMethod::OPENCL_3, "OPENCL_3"},
    {ZnccMethod::CUDA, "CUDA"},
};

string ZnccMethodToString(ZnccMethod method);

double calculateMean(int x, int y, int d, const vector<unsigned char> &img, const ZnccParams &znccParams);
double calculateZncc(int x, int y, int d, double mean1, double mean2, const vector<unsigned char> &img1, const vector<unsigned char> &img2, const ZnccParams &znccParams);

double calculateMeanSimd(int x, int y, int d, int width, int height, int halfWinSize, const vector<unsigned char> &img);
double calculateZnccSimd(int x, int y, int d, double mean1, double mean2, int width, int height, int halfWinSize, const vector<unsigned char> &img1, const vector<unsigned char> &img2);

vector<unsigned char> crosscheck(const vector<unsigned char> &dispMapLeft, const vector<unsigned char> &dispMapRight, const ZnccParams &znccParams);
vector<unsigned char> fillOcclusion(const vector<unsigned char> &dispMap, const ZnccParams &znccParams);
vector<unsigned char> normalizeMap(const vector<unsigned char> &dispMap, const ZnccParams &znccParams);