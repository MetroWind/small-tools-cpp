#include <iostream>
#include <vector>
#include <string>

#include <Magick++.h>
#include <cxxopts.hpp>

struct Options
{
    std::string input_file;
    std::string output_file;
    double corner_radius;
    double shadow_blur;
    double shadow_alpha;
};

void fancify(const Options& options)
{
    using namespace Magick;

    // Construct the image object. Seperating image construction from the
    // the read operation ensures that a failure to read the image file
    // doesn't render the image object useless.
    Image shot;
    // Read a file into image object
    shot.read(options.input_file);
    double width = shot.size().width();
    double height = shot.size().height();

    Image alpha = shot;
    alpha.channel(AlphaChannel);
    alpha.strokeWidth(0);

    alpha.backgroundColor("transparent");
    alpha.erase();
    alpha.fillColor("white");
    alpha.draw(DrawableRoundRectangle(
                   0, 0, width - 1, height - 1,
                   options.corner_radius, options.corner_radius));

    shot.alpha(false);
    shot.composite(std::move(alpha), 0, 0, CopyAlphaCompositeOp);

    Image shadow = shot;
    shadow.backgroundColor("black");
    shadow.shadow(options.shadow_alpha, options.shadow_blur, 0, 0);
    shadow.composite(std::move(shot), 2.0 * options.shadow_blur,
                     ssize_t(1.5 * options.shadow_blur), OverCompositeOp);
    Image bg = shadow;
    bg.backgroundColor("white");
    bg.erase();
    bg.composite(std::move(shadow), 0, 0, OverCompositeOp);

    bg.write(options.output_file);
}

int main(int argc,char **argv)
{
    Magick::InitializeMagick(*argv);

    cxxopts::Options cmd_options("shot_process", "Make fancy screenshot");
    cmd_options.add_options()
        ("filename", "Input file", cxxopts::value<std::string>())
        ("o,output", "Output file. If this is not issued, write to the input file.",
         cxxopts::value<std::string>())
        ("r,corner-radius", "The radius of the rounded corners",
         cxxopts::value<double>()->default_value("10"))
        ("b,shadow-blur", "The radius of the shadow blur",
         cxxopts::value<double>()->default_value("10"))
        ("a,shadow-alpha", "The opacity of the shadow",
         cxxopts::value<double>()->default_value("40"))
        ("h,help", "Print this message.");

    cmd_options.parse_positional({"filename",});

    if(argc <= 1)
    {
        std::cout << cmd_options.help() << std::endl;
        return 1;
    }

    auto opts = cmd_options.parse(argc, argv);

    if (opts.count("help"))
    {
        std::cout << cmd_options.help() << std::endl;
        return 0;
    }

    std::string out_file;
    if(opts.count("output"))
    {
        out_file = opts["output"].as<std::string>();
    }
    else
    {
        out_file = opts["filename"].as<std::string>();
    }

    Options options {
        opts["filename"].as<std::string>(),
        out_file,
        opts["corner-radius"].as<double>(),
        opts["shadow-blur"].as<double>(),
        opts["shadow-alpha"].as<double>(),
    };

    fancify(std::move(options));
    return 0;
}
