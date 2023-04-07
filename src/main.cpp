#include <filesystem>

#include "utils\clchecks.hpp"
#include "utils\datatools.hpp"
#include "utils\ScopeBasedTimer.hpp"
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
     for (auto method : {ZnccMethod::OPENCL, ZnccMethod::MULTI_THREADED, ZnccMethod::OPENMP, ZnccMethod::SINGLE_THREADED})
     {
          Timer timer;
          cout << "Running ZNCC with method " << static_cast<int>(method) << ": " << ZnccMethodToStringHelper(method) << "\n";
          vector<unsigned char> dispImg(img_left.width * img_left.height);
          zncc_pipeline(img_left.dataGray, img_right.dataGray, dispImg, img_left.width, img_left.height, method, false, false);
          string filename = "./data/disp_zncc_" + ZnccMethodToStringHelper(method) + ".png";
          saveImage(filename, dispImg, img_left.width, img_left.height);
     }

     return 0;
}