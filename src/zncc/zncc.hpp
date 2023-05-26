#pragma once

#ifdef USE_CUDA
#include "zncc_cuda/zncc_cuda.cuh"
#endif

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
#include "../utils/clchecks.hpp"
#include "../utils/scope_based_timer.hpp"
#include "zncc_common.hpp"

using namespace std;

extern mutex cout_mutex;

struct ZnccResult
{
    vector<unsigned char> dispMapLeft;
    vector<unsigned char> dispMapRight;
    vector<unsigned char> dispMapCC;
    vector<unsigned char> dispMapOC;
    vector<unsigned char> dispMap;
    double znccTime;
    double crosscheckTime;
    double occlusionTime;
    double totalTime;
};

void zncc_single(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_multi(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_openmp(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_simd(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_opencl_1(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_opencl_2(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_opencl_3(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
void zncc_cuda(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);

void zncc(vector<unsigned char>& dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
ZnccResult zncc_pipeline(const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
