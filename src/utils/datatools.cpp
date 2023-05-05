#include "datatools.hpp"

#include <vector>

vector<unsigned char> rgbaToGray(const vector<unsigned char> &rgbImg, int w, int h)
{
    vector<unsigned char> grayImg(w * h);

    for (int i = 0; i < w * h; ++i)
    {
        const int rgbIdx = i * 4;
        const double r = rgbImg[rgbIdx] * 0.2126;
        const double g = rgbImg[rgbIdx + 1] * 0.7152;
        const double b = rgbImg[rgbIdx + 2] * 0.0722;

        grayImg[i] = static_cast<unsigned char>(r + g + b);
    }

    return grayImg;
}

vector<unsigned char> grayToRgba(const vector<unsigned char> &grayImg, int w, int h)
{
    vector<unsigned char> rgbImg(w * h * 4);

    for (int i = 0; i < w * h; ++i)
    {
        const unsigned char gray = grayImg[i];
        const double r = 0.2126 * gray;
        const double g = 0.7152 * gray;
        const double b = 0.0722 * gray;

        const int rgbIdx = i * 4;
        rgbImg[rgbIdx] = static_cast<unsigned char>(r);
        rgbImg[rgbIdx + 1] = static_cast<unsigned char>(g);
        rgbImg[rgbIdx + 2] = static_cast<unsigned char>(b);
        rgbImg[rgbIdx + 3] = 255;
    }

    return rgbImg;
}

vector<unsigned char> downsample(const vector<unsigned char> &image, int width, int height, int factor)
{
    int new_width = width / factor;
    int new_height = height / factor;

    vector<unsigned char> resized_image(new_width * new_height);

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            // Compute the corresponding pixel in the original image
            int x_orig = x * factor;
            int y_orig = y * factor;

            // Compute the four nearest neighbors in the original image
            unsigned char tl = image[y_orig * width + x_orig];
            unsigned char tr = image[y_orig * width + x_orig + 1];
            unsigned char bl = image[(y_orig + 1) * width + x_orig];
            unsigned char br = image[(y_orig + 1) * width + x_orig + 1];

            // Compute the weighted average using bilinear interpolation
            float x_ratio = static_cast<float>(x_orig % factor) / (float)factor;
            float y_ratio = static_cast<float>(y_orig % factor) / (float)factor;
            float top = tl * (1.0f - x_ratio) + tr * x_ratio;
            float bottom = bl * (1.0f - x_ratio) + br * x_ratio;
            float value = top * (1.0f - y_ratio) + bottom * y_ratio;

            // Store the result in the resized image
            resized_image[y * new_width + x] = static_cast<unsigned char>(value);
        }
    }

    return resized_image;
}

vector<unsigned char> upsample(const vector<unsigned char> &img, int width, int height, int factor)
{
    int new_width = width * factor;
    int new_height = height * factor;
    vector<unsigned char> new_img(new_width * new_height);

    for (int j = 0; j < new_height; j++)
    {
        for (int i = 0; i < new_width; i++)
        {
            int x = i / factor;
            int y = j / factor;
            int idx = j * new_width + i;
            int new_idx = y * width + x;
            new_img[idx] = img[new_idx];
        }
    }

    return new_img;
}

tuple<bool, Image> loadImage(string fpath)
{
    unsigned error;
    unsigned char *buffer;
    Image img;

    error = lodepng_decode32_file(&buffer, &img.width, &img.height, fpath.c_str());
    if (error)
        printf("error %u for %s: %s\n", error, fpath.c_str(), lodepng_error_text(error));
    else
    {
        img.dataRgb = vector<unsigned char>(buffer, buffer + img.width * img.height * 4);
        img.dataGray = rgbaToGray(img.dataRgb, img.width, img.height);
        // img.dataGraySmall = downsample(img.dataGray, img.width, img.height);
        // img.widthSmall = img.width / factor;
        // img.heightSmall = img.height / factor;
        // saveImage("./data/im0_gray_res.png", img.dataGraySmall, img.widthSmall, img.heightSmall);
        free(buffer);
    }

    return make_tuple(error, img);
}

tuple<bool, Image> loadImage(string dir, string fname)
{
    string path = dir + fname;
    return loadImage(path);
}

tuple<Image, Image> loadImages(string dir, string fname_0, string fname_1)
{
    unsigned err;
    Image img_left;
    Image img_right;

    tie(err, img_left) = loadImage(dir, fname_0);
    if (!err)
        tie(err, img_right) = loadImage(dir, fname_1);

    return make_tuple(img_left, img_right);
}

tuple<Image, Image> loadImages(string dir)
{
    return loadImages(dir, "im0.png", "im1.png");
}

tuple<Image, Image> loadImages(int argc, char **argv)
{
    string dir = "./data/2021_chess2/";
    if (argc > 1)
        dir = argv[1];

    return loadImages(dir);
}

tuple<Image, Image> loadImages()
{
    string dir = "./data/2021_chess2/";
    return loadImages(dir);
}

void saveImage(string fpath, const vector<unsigned char> &img, int w, int h)
{
    unsigned error;

    if (img.size() == w * h)
        error = lodepng_encode_file(fpath.c_str(), img.data(), w, h, LCT_GREY, 8);
    else if (img.size() == w * h * 3)
        error = lodepng_encode_file(fpath.c_str(), img.data(), w, h, LCT_RGB, 8);
    else if (img.size() == w * h * 4)
        error = lodepng_encode_file(fpath.c_str(), img.data(), w, h, LCT_RGBA, 8);

    if (error)
        printf("error %u for %s: %s\n", error, fpath.c_str(), lodepng_error_text(error));
}