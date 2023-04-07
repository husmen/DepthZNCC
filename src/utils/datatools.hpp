#pragma once

#include <lodepng.h>

using namespace std;

// typedef vector<unsigned char> pixels;

#define RESIZE_FACTOR 4

struct Image
{
    unsigned int width;
    unsigned int height;
    unsigned int widthSmall;
    unsigned int heightSmall;
    vector<unsigned char> dataRgb;
    vector<unsigned char> dataGray;
    vector<unsigned char> dataGraySmall;
};

vector<unsigned char> rgbaToGray(const vector<unsigned char>& rgbImg, int w, int h);
tuple<bool, Image> loadImage(string fpath);
tuple<bool, Image> loadImage(string dir, string fname);
tuple<Image, Image> loadImages(string dir, string fname_0, string fname_1);
tuple<Image, Image> loadImages(string dir);
tuple<Image, Image> loadImages();
tuple<Image, Image> loadImages(int argv, char **argc);

vector<unsigned char> downsample(const vector<unsigned char> &image, int width, int height);
vector<unsigned char> upsample(const vector<unsigned char> &img, int width, int height);

void saveImage(string fpath, const vector<unsigned char>& img, int w, int h);