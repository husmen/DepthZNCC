#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable

__kernel 
void calc_mean(__global const unsigned char* img,
                                   const int width,
                                   const int height,
                                   const int x,
                                   const int y,
                                   const int d,
                                   __global double* sum,
                                   __global int* count)
{
    const int WIN_SIZE = 5;
    for (int j = -WIN_SIZE / 2; j <= WIN_SIZE / 2; j++)
    {
        for (int i = -WIN_SIZE / 2; i <= WIN_SIZE / 2; i++)
        {
            int xj = x + i;
            int yj = y + j;
            if (xj >= 0 && xj < width && yj >= 0 && yj < height)
            {
                int xjd = xj - d;
                if (xjd >= 0 && xjd < width)
                {
                    atomic_add(&sum[0], (double)img[yj * width + xjd]);
                    atomic_add(&count[0], 1);
                }
            }
        }
    }
}

__kernel 
void calc_zncc(__global const unsigned char* img1,
                                   __global const unsigned char* img2,
                                   const int width,
                                   const int height,
                                   const int x,
                                   const int y,
                                   const int d,
                                   const double mean1,
                                   const double mean2,
                                   __global double* num,
                                   __global double* denom1,
                                   __global double* denom2)
{
    const int WIN_SIZE = 5;
    for (int j = -WIN_SIZE / 2; j <= WIN_SIZE / 2; j++)
    {
        for (int i = -WIN_SIZE / 2; i <= WIN_SIZE / 2; i++)
        {
            int xj1 = x + i;
            int xj2 = x + i - d;
            int yj = y + j;
            if (xj1 >= 0 && xj1 < width && xj2 >= 0 && xj2 < width && yj >= 0 && yj < height)
            {
                double val1 = (double)img1[yj * width + xj1] - mean1;
                double val2 = (double)img2[yj * width + xj2] - mean2;
                atomic_add(&num[0], val1 * val2);
                atomic_add(&denom1[0], val1 * val1);
                atomic_add(&denom2[0], val2 * val2);
            }
        }
    }
}
