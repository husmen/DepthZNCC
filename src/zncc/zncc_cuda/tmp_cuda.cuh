#ifndef ZNCC_CUDA__
#define ZNCC_CUDA__

#include <stdio.h>

void wrap_test_print();
void wrap_add_cuda(int *c, const int *a, const int *b, unsigned int size);

#endif
