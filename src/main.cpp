#ifdef USE_OCL
// import FIRST! https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#include "utils/clchecks.hpp"
#endif
#include <filesystem>
#include <iostream>
#include <fstream>
#include "utils/datatools.hpp"
#include "zncc/zncc.hpp"

namespace fs = filesystem;

void printHelp(int argc, char **argv)
{
     cout << "Usage: mpp_project.exe <path_to_data_dir>\n";

     auto cwd = fs::current_path();
     cout << "current working dir " << cwd << "\n";
     cout << "program path " << argv[0] << "\n";

     cout << "You have entered " << argc - 1 << " arguments:"
          << "\n";

     for (int i = 1; i < argc; ++i)
          cout << argv[i] << "\n";
}

ZnccResult run_zncc(const Image &leftImg, const Image &rightImg, ZnccParams znccParams)
{
     Timer timer;
     auto leftImg_ = znccParams.resizeFactor != 1 ? downsample(leftImg.dataGray, leftImg.width, leftImg.height, znccParams.resizeFactor) : leftImg.dataGray;
     auto rightImg_ = znccParams.resizeFactor != 1 ? downsample(rightImg.dataGray, rightImg.width, rightImg.height, znccParams.resizeFactor) : rightImg.dataGray;

     cout << "Running ZNCC with method " << ZnccMethodToString(znccParams.method) << "\n";
     auto result = zncc_pipeline(leftImg_, rightImg_, znccParams);
     return result;
}

void run_post_proc(ZnccResult &result, ZnccParams &params)
{
     post_proc_pipeline(result, params);
}

void run_logger(ZnccResult &result, ZnccParams &params, ofstream &csv_log)
{
     auto methodStr = ZnccMethodToString(params.method);
     auto filename_suffix = to_string(params.resizeFactor) + "_" + to_string(params.winSize) + "_" + to_string(params.maxDisp) + "_" + to_string(params.ccThresh) + "_" + to_string(params.platformId) + ".png";

     auto filename = "./data/" + methodStr + "_disp_" + filename_suffix;
     saveImage(filename, result.dispMap, params.width, params.height);

     filename = "./data/" + methodStr + "_left_" + filename_suffix;
     saveImage(filename, result.dispMapLeft, params.width, params.height);

     filename = "./data/" + methodStr + "_right_" + filename_suffix;
     saveImage(filename, result.dispMapRight, params.width, params.height);

     csv_log << methodStr << "," << params.platformId << "," << params.resizeFactor << "," << params.winSize << "," << params.maxDisp << "," << params.ccThresh << "," << params.occThresh << "," << to_string(result.znccTime) << "," << to_string(result.postProcTime) << "\n";
     csv_log.flush();
}



int main(int argc, char **argv)
{
     // tcheck cwd and arguments
     printHelp(argc, argv);

     // Check OpenCL
#ifdef USE_OCL
     clHelloWorld();
     clVecAdd();
#endif

     // Load images
     auto [img_left, img_right] = loadImages(argc, argv);
     cout << "Left image stats:\n"
          << "\tDims: " << img_left.width << "x" << img_left.height << "\n"
          << "\tRGB size: " << img_left.dataRgb.size() << "\n"
          << "\tGray size: " << img_left.dataGray.size() << "\n";
     cout << "Right image stats:\n"
          << "\tDims: " << img_right.width << "x" << img_right.height << "\n"
          << "\tRGB size: " << img_right.dataRgb.size() << "\n"
          << "\tGray size: " << img_right.dataGray.size() << "\n";

     ofstream csv_log;
     csv_log.open("./data/log.csv", ios::out | ios::app);

     if(filesystem::is_empty("./data/log.csv"))
          csv_log << "method,platformId,resizeFactor,winSize,maxDisp,ccThresh,occThresh,znccTime,postprocTime\n";

     // Run Grid Search for ZNCC Params
     // for (auto method : {ZnccMethod::MULTI_THREADED, ZnccMethod::OPENMP, ZnccMethod::SIMD, ZnccMethod::OPENCL, ZnccMethod::CUDA})
     for (auto method : {ZnccMethod::OPENCL})//, ZnccMethod::OPENCL, ZnccMethod::SIMD, ZnccMethod::MULTI_THREADED})
     {
          for (auto platformId : {1})
          {
               for (auto resizeFactor : {2})
               {
                    for (auto winSize : {15, 25, 35})
                    {
                         for (auto maxDisp : {32, 64, 128})
                         {
                              auto znccParams = ZnccParams{static_cast<int>(img_left.width) / resizeFactor, static_cast<int>(img_left.height) / resizeFactor, maxDisp, winSize, 0, 0, resizeFactor, true, true, true, true, method, platformId};
                              auto result = run_zncc(img_left, img_right, znccParams);
                              for (auto ccThresh : {maxDisp / 4})
                              {
                                   for (auto occThresh : {ccThresh / 2})
                                   {
                                        znccParams.ccThresh = ccThresh;
                                        znccParams.occThresh = occThresh;
                                        run_post_proc(result, znccParams);
                                        run_logger(result, znccParams, csv_log);
                                   }
                              }
                         }
                    }
               }
          }
     }

     csv_log.close();

     // ZNCC best params
     // auto resizeFactor = 1;
     // auto winSize = 9;
     // auto maxDisp = 32;
     // auto ccThresh = 4;
     // auto occThresh = 24;

     // Run ZNCC for different OpenCL devices
     // for (auto platformId : {0, 1, 2}) // APU, GPU, CPU
     // {
     //      for (auto method : {ZnccMethod::OPENCL_1, ZnccMethod::OPENCL_2})
     //      {
     //           auto znccParams = ZnccParams{static_cast<int>(img_left.width) / resizeFactor, static_cast<int>(img_left.height) / resizeFactor, maxDisp, winSize, ccThresh, occThresh, resizeFactor, true, true, true, true, method, platformId};
     //           auto result = run_zncc(img_left, img_right, znccParams);
     //      }
     // }

     // Run ZNCC with best params
     // auto method = ZnccMethod::OPENCL_1;
     // auto platformId = 1;
     // auto znccParams = ZnccParams{static_cast<int>(img_left.width) / resizeFactor, static_cast<int>(img_left.height) / resizeFactor, maxDisp, winSize, ccThresh, occThresh, resizeFactor, true, true, false, true, method, platformId};
     // auto result = run_zncc(img_left, img_right, znccParams);

     return 0;
}