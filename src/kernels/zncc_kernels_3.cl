pipe double mean_pipe __attribute__((xcl_reqd_pipe_depth(256)));
// pipe double mean_pipe_right __attribute__((xcl_reqd_pipe_depth(256)));

pipe double zncc_pipe __attribute__((xcl_reqd_pipe_depth(256)));
// pipe double zncc_pipe_right __attribute__((xcl_reqd_pipe_depth(256)));

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void calculate_mean(global const unsigned char* img, int width, int height, int winSize, int maxDisp)
{   
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;

    int halfWinSize = winSize / 2;

    for (int d = 0; d < maxDisp; d++)
    {  
        int yy_0 = max(d, y - halfWinSize);
        int yy_1 = min(height - d, y + halfWinSize);
        int xx_0 = max(d, x - halfWinSize);
        int xx_1 = min(width - d, x + halfWinSize);

        double sum_left = 0.0;
        double sum_right = 0.0;
        for (int yy = yy_0; yy < yy_1; yy++)
        {
            for (int xx = xx_0; xx < xx_1; xx++)
            {
                sum_left += img[yy * width + xx - d];
            }
        }

        int count = (xx_1 - xx_0) * (yy_1 - yy_0);
        
        write_pipe(mean_pipe, sum_left / (double)count); 
    }
}

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
__kernel void calculate_zncc(global const unsigned char* leftImg,
                        global const unsigned char* rightImg,
                        int width, int height, int winSize, int maxDisp)
{   
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;

    int halfWinSize = winSize / 2;

    double mean_0, mean_d;
    read_pipe(mean_pipe, &mean_0);

    for (int d = 0; d < maxDisp; d++)
    {
        int yy_0 = max(d, y - halfWinSize);
        int yy_1 = min(height - d, y + halfWinSize);
        int xx_0 = max(d, x - halfWinSize);
        int xx_1 = min(width - d, x + halfWinSize);

        double num = 0.0;
        double denom1 = 0.0;
        double denom2 = 0.0;

        
        if(d==0)
        {
            mean_d = mean_0;
        }
        else
        {
            read_pipe(mean_pipe, &mean_d);
        }

        for (int yy = yy_0; yy < yy_1; yy++)
        {
            for (int xx = xx_0; xx < xx_1; xx++)
            {
                double val1 = leftImg[yy * width + xx] - mean_0;
                double val2 = rightImg[yy * width + xx - d] - mean_d;
                num += val1 * val2;
                denom1 += val1 * val1;
                denom2 += val2 * val2;
            }
        }

        double denom = sqrt(denom1 * denom2);
        write_pipe(zncc_pip, denom == 0.0 ? 0.0 : num / denom); 
    }
}

// Kernel for ZNCC disparity calculation
__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
__kernel void zncc_kernel(global unsigned char* disparityImg, int width, int maxDisp)
{
    // Get global thread ID
    int idx = get_global_id(0);
    int x = idx % width;
    int y = idx / width;

    double zncc;
    double maxZncc = -1.0;
    int bestDisp = 0;

    for (int d = 0; d < maxDisp; d++)
    {
        read_pipe(zncc_pipe, &zncc);
        if (zncc > maxZncc)
        {
            maxZncc = zncc;
            bestDisp = d;
        }
    }

    disparityImg[idx] = (unsigned char)bestDisp;
}
