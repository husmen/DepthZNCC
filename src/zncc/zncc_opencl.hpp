#pragma once

#include <iostream>
#include <vector>
#include <tuple>
#include "../utils/clchecks.hpp"
#include "zncc_common.hpp"

using namespace std;

void zncc_opencl(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams, bool reverse);

void zncc_opencl_opt1(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);

void zncc_opencl_opt(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams, bool reverse);

void zncc_opencl_opt3(vector<unsigned char> &leftDispMap, vector<unsigned char> &rightDispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);

void zncc_opencl_pipe(vector<unsigned char> &dispMap, const vector<unsigned char> &leftImg, const vector<unsigned char> &rightImg, const ZnccParams &znccParams);
