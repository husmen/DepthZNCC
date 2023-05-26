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
     auto methodStr = ZnccMethodToString(znccParams.method);
     auto leftImg_ = znccParams.resizeFactor != 1 ? downsample(leftImg.dataGray, leftImg.width, leftImg.height, znccParams.resizeFactor) : leftImg.dataGray;
     auto rightImg_ = znccParams.resizeFactor != 1 ? downsample(rightImg.dataGray, rightImg.width, rightImg.height, znccParams.resizeFactor) : rightImg.dataGray;

     cout << "Running ZNCC with method " << methodStr << "\n";
     auto result = zncc_pipeline(leftImg_, rightImg_, znccParams);

     string filename_suffix = to_string(znccParams.resizeFactor) + "_" + to_string(znccParams.winSize) + "_" + to_string(znccParams.maxDisp) + "_" + to_string(znccParams.ccThresh) + "_" + to_string(znccParams.platformId) + ".png";
     string filename = "./data/" + methodStr + "_disp_" + filename_suffix;
     saveImage(filename, result.dispMap, znccParams.width, znccParams.height);

     filename = "./data/" + methodStr + "_left_" + filename_suffix;
     saveImage(filename, result.dispMapLeft, znccParams.width, znccParams.height);

     filename = "./data/" + methodStr + "_right_" + filename_suffix;
     saveImage(filename, result.dispMapRight, znccParams.width, znccParams.height);

     return result;
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
     csv_log << "method,platformId,resizeFactor,winSize,maxDisp,ccThresh,occThresh,znccTime,postprocTime\n";

     // Run Grid Search for ZNCC Params
     // for (auto method : {ZnccMethod::MULTI_THREADED, ZnccMethod::OPENMP, ZnccMethod::SIMD, ZnccMethod::OPENCL_NAIVE, ZnccMethod::CUDA})
     for (auto method : {ZnccMethod::OPENCL_NAIVE})
     {
          for (auto platformId : {1})
          {
               for (auto resizeFactor : {1, 2, 4})
               {
                    for (auto winSize : {15, 31, 63})
                    {
                         for (auto maxDisp : {16, 32, 64, 128})
                         {
                              for (auto ccThresh : {maxDisp / 8, maxDisp / 4, maxDisp / 2})
                              {
                                   for (auto occThresh : {ccThresh / 2})
                                   {
                                        auto znccParams = ZnccParams{static_cast<int>(img_left.width) / resizeFactor, static_cast<int>(img_left.height) / resizeFactor, maxDisp, winSize, ccThresh, occThresh, resizeFactor, true, true, true, true, method, platformId};
                                        auto result = run_zncc(img_left, img_right, znccParams);
                                        csv_log << ZnccMethodToString(method) << "," << platformId << "," << resizeFactor << "," << winSize << "," << maxDisp << "," << ccThresh << "," << occThresh << "," << to_string(result.znccTime) << "," << to_string(result.postProcTime) << "\n";
                                        csv_log.flush();
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