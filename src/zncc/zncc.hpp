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

#define MAX_DISP 32
#define WIN_SIZE 11
#define CC_THRESHOLD MAX_DISP / 8
#define OCC_THRESHOLD CC_THRESHOLD / 2

using namespace std;

// ZNCC methods enum and its helpers
enum class ZnccMethod
{
    SINGLE_THREADED,
    MULTI_THREADED,
    OPENMP,
    OPENCL,
    CUDA
};

const map<ZnccMethod, string> ZnccMethodToString = {
    {ZnccMethod::SINGLE_THREADED, "SINGLE_THREADED"},
    {ZnccMethod::MULTI_THREADED, "MULTI_THREADED"},
    {ZnccMethod::OPENMP, "OPENMP"},
    {ZnccMethod::OPENCL, "OPENCL"},
    {ZnccMethod::CUDA, "CUDA"},
};

// double calculateMean(int x, int y, int d, vector<unsigned char>& img, int width, int height);
// double calculateZncc(int x, int y, int d, double mean1, double mean2, vector<unsigned char>& img1, vector<unsigned char>& img2, int width, int height);
extern mutex cout_mutex;
string ZnccMethodToStringHelper(ZnccMethod method);
void zncc(const vector<unsigned char>& leftImg, const vector<unsigned char>& rightImg, vector<unsigned char>& disparityImg, int width, int height, ZnccMethod method);
void zncc_pipeline(const vector<unsigned char> &imgLeft, const vector<unsigned char> &imgRight, vector<unsigned char> &dispMap, int width, int height, ZnccMethod method = ZnccMethod::SINGLE_THREADED, bool crossCheck = true, bool occlusionFill = true);

// double compute_zncc(const vector<unsigned char>& left_image, const vector<unsigned char>& right_image, int i, int j, int d, int image_width);
// void zncc2(const vector<unsigned char> &left_image, const vector<unsigned char> &right_image, vector<unsigned char> &disparity_map, int width, int height);
