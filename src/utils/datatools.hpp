#pragma once

#include <lodepng.h>
#include <tuple>
#include <vector>

using namespace std;

struct Image
{
    unsigned int width;
    unsigned int height;
    vector<unsigned char> dataRgb;
    vector<unsigned char> dataGray;
};

vector<unsigned char> rgbaToGray(const vector<unsigned char>& rgbImg, int w, int h);
tuple<bool, Image> loadImage(string fpath);
tuple<bool, Image> loadImage(string dir, string fname);
tuple<Image, Image> loadImages(string dir, string fname_0, string fname_1);
tuple<Image, Image> loadImages(string dir);
tuple<Image, Image> loadImages();
tuple<Image, Image> loadImages(int argv, char **argc);

vector<unsigned char> downsample(const vector<unsigned char> &image, int width, int height, int factor);
vector<unsigned char> upsample(const vector<unsigned char> &img, int width, int height, int factor);

void saveImage(string fpath, const vector<unsigned char>& img, int w, int h);