//Perform an element–wise addition of A and B and store in C
//N work–items will be created to execute this kernel.
__kernel
void vecadd(__global int *C, __global int *A, __global int *B)
{
    int tid = get_global_id(0); // OpenCLintrinsicfunction
    C[tid] = A[tid] + B[tid];
}