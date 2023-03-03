#include "datatools.hpp"

tuple<bool, Image> load_image(string fpath)
{
    unsigned error;
    unsigned char *buffer;
    Image img;

    error = lodepng_decode32_file(&buffer, &img.width, &img.height, fpath.c_str());
    if (error)
        printf("error %u for %s: %s\n", error, fpath.c_str(), lodepng_error_text(error));
    else
    {
        img.data = vector<unsigned char>(buffer, buffer + img.width * img.height * 4);
        free(buffer);
    }

    return make_tuple(error, img);
}

tuple<bool, Image> load_image(string dir, string fname)
{
    string path = dir + fname;
    return load_image(path);
}

tuple<Image, Image> load_images(string dir, string fname_0, string fname_1)
{
    unsigned err;
    Image img_left;
    Image img_right;

    tie(err, img_left) = load_image(dir, fname_0);
    if (!err)
        tie(err, img_right) = load_image(dir, fname_1);

    return make_tuple(img_left, img_right);
}

tuple<Image, Image> load_images(string dir)
{
    return load_images(dir, "im0.png", "im1.png");
}

tuple<Image, Image> load_images()
{
    string dir = "D:\\ws_ocl\\mpp_project\\data\\";
    return load_images(dir);
}