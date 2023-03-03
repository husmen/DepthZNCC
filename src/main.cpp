#include <filesystem>

#include "utils\clchecks.hpp"
#include "utils\datatools.hpp"

namespace fs = filesystem;

void print_help(int argc, char **argv)
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
    print_help(argc, argv);

    // Check OpenCL
    cl_hello_world();
    cl_vec_add();

    // Load images
    auto [img_left, img_right] = load_images();
    cout << "Left image size: " << img_left.width << "x" << img_left.height << "\n";
    cout << "Right image size: " << img_right.width << "x" << img_right.height << "\n";

    return 0;
}