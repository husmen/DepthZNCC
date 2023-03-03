#include <lodepng.h>

using namespace std;

// typedef vector<unsigned char> pixels;

struct Image
{
    unsigned int width;
    unsigned int height;
    vector<unsigned char> data;
};

tuple<bool, Image> load_image(string fpath);
tuple<bool, Image> load_image(string dir, string fname);
tuple<Image, Image> load_images(string dir, string fname_0, string fname_1);
tuple<Image, Image> load_images(string dir);
tuple<Image, Image> load_images();