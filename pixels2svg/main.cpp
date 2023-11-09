#include <stdint.h>

#include <algorithm>
#include <expected>
#include <format>
#include <sstream>
#include <iostream>

#include <cxxopts.hpp>

#include "png.hpp"

std::string png2Svg(const PNGFile& png, uint32_t size)
{
    double pixel_size = 1.0;
    if(size > 0)
    {
        pixel_size = double(size) / double(std::max(png.width(), png.height()));
    }

    std::stringstream ss;

    ss << std::format(R"(<svg viewBox="0 0 {} {}" xmlns="http://www.w3.org/2000/svg">)",
                      png.width() * pixel_size, png.height() * pixel_size)
       << "\n";

    for(uint32_t y = 0; y < png.height(); y++)
    for(uint32_t x = 0; x < png.width(); x++)
    {
        auto color = png.at(x, y);
        if(color.a == 0)
        {
            continue;
        }
        std::string opacity;
        if(color.a != 255)
        {
            opacity = std::format(
                R"(opacity="{}")", double(color.a) / 255.0);
        }

        ss << std::format(
            R"(<rect x="{}" y="{}" width="{}" height="{}" fill="{}" stroke="{}" stroke-width="{}" {} />)",
            x * pixel_size, y * pixel_size, pixel_size, pixel_size,
            color.hex(), color.hex(), pixel_size * 0.02, opacity)
           << "\n";
    }
    ss << "</svg>\n";
    return ss.str();
}

int main(int argc, char** argv)
{
    cxxopts::Options cmd_options("pixels2svg", "Convert a image to svg by pixels");
    cmd_options.add_options()
        ("filename", "Input file", cxxopts::value<std::string>())
        ("s,size", "The length of the longest dimension of the resulting SVG. "
         "Zero means using the original image size.",
         cxxopts::value<uint32_t>()->default_value("0"))
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

    std::expected<PNGFile, std::string> png =
        std::move(PNGFile::readFromFile(opts["filename"].as<std::string>()));
    if(!png.has_value())
    {
        std::cerr << png.error() << std::endl;
        return 1;
    }

    std::cout << png2Svg(*std::move(png), opts["size"].as<uint32_t>());
    return 0;
}
