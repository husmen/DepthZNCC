#pragma once

#define __CL_ENABLE_EXCEPTIONS

#include <CL/cl.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string get_cl_err(cl_int errCode);
int clHelloWorld();
int clVecAdd();