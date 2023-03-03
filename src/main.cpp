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

int main(int argc, char **argv)
{
    // tcheck cwd and arguments
    printHelp(argc, argv);

    // Check OpenCL
    clHelloWorld();
    clVecAdd();

    // Load images
    auto [img_left, img_right] = loadImages();
    cout << "Left image stats:\n"
         << "\tDims: " << img_left.width << "x" << img_left.height << "\n"
         << "\tRGB size: " << img_left.dataRgb.size() << "\n"
         << "\tGray size: " << img_left.dataGray.size() << "\n";
    cout << "Right image stats:\n" 
         << "\tDims: " << img_right.width << "x" << img_right.height << "\n"
         << "\tRGB size: " << img_right.dataRgb.size() << "\n"
         << "\tGray size: " << img_right.dataGray.size() << "\n";

    // Run ZNCC
    vector<unsigned char> dispImgSmall(img_left.widthSmall * img_left.heightSmall);
    zncc_pipeline(img_left.dataGraySmall, img_right.dataGraySmall, dispImgSmall, img_left.widthSmall, img_left.heightSmall);
    auto dispImg = upsample(dispImgSmall, img_left.widthSmall, img_left.heightSmall);
    saveImage("./data/disp_zncc.png", dispImg, img_left.width, img_left.height);

    return 0;
}