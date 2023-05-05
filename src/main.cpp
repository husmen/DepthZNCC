#include <filesystem>

#include "utils\clchecks.hpp"
#include "utils\datatools.hpp"
#include "zncc\zncc.hpp"

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

     string filename = "./data/disp_zncc_" + methodStr + "_" + to_string(znccParams.resizeFactor) + "_" + to_string(znccParams.winSize) + "_" + to_string(znccParams.maxDisp) + "_" + to_string(znccParams.ccThresh) + ".png";
     saveImage(filename, result.dispMap, znccParams.width, znccParams.height);

     return result;
}

int main(int argc, char **argv)
{
     // tcheck cwd and arguments
     printHelp(argc, argv);

     // Check OpenCL
     clHelloWorld();
     clVecAdd();

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

     // Run ZNCC
     for (auto method : {ZnccMethod::OPENCL}) //, ZnccMethod::OPENCL, ZnccMethod::MULTI_THREADED, ZnccMethod::OPENMP, ZnccMethod::SINGLE_THREADED
     {
          for (auto resizeFactor : {1, 2, 4})
          {
               for (auto winSize : {9, 17, 33})
               {
                    for (auto maxDisp : {16, 32, 64})
                    {
                         for (auto ccThresh : {maxDisp / 2, maxDisp, maxDisp * 2})
                         {
                              ZnccParams znccParams = {img_left.width / resizeFactor, img_left.height / resizeFactor, maxDisp, winSize, ccThresh, ccThresh / 2, resizeFactor, true, true, true, true, method};
                              auto result = run_zncc(img_left, img_right, znccParams);
                         }
                    }
               }
          }
     }

     return 0;
}