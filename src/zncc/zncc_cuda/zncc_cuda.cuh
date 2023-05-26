#ifndef ZNCC_CUDA__
#define ZNCC_CUDA__

#include <stdio.h>

void wrap_test_print();
void wrap_add_cuda(int *c, const int *a, const int *b, unsigned int size);
void zncc_cuda_wrapper(unsigned char* dispMap, const unsigned char* leftImg, const unsigned char* rightImg, int width, int height, int winSize, int maxDisp);

#endif
