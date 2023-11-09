#include "png.hpp"

#include <iostream>
#include <array>
#include <string>
#include <cstdio>
#include <expected>
#include <cassert>
#include <format>
#include <utility>

#include <png.h>

PNGFile::PNGFile(PNGFile&& rhs)
{
    std::swap(file, rhs.file);
    std::swap(png, rhs.png);
    std::swap(info, rhs.info);
    img_width = rhs.img_width;
    img_height = rhs.img_height;
    bit_depth = rhs.bit_depth;
    row_size = rhs.row_size;
    color_type = rhs.color_type;
    pixel_size = rhs.pixel_size;
    std::swap(rows, rhs.rows);
}

std::expected<PNGFile, std::string>
PNGFile::readFromFile(const std::string& filename)
{
    PNGFile result;
    result.file = std::fopen(filename.c_str(), "rb");
    if(!result.file)
    {
        return std::unexpected("Failed to open file.");
    }

    // Check header
    std::array<unsigned char, HEADER_SIZE> header;
    if(std::fread(header.data(), 1, header.size(), result.file) !=
       header.size())
    {
        return std::unexpected("Failed to read file header.");
    }

    if(png_sig_cmp(header.data(), 0, header.size()))
    {
        return std::unexpected("File is not a PNG.");
    }

    // Read actual data
    result.png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!result.png)
    {
        return std::unexpected("Failed to read PNG struct.");
    }

    result.info = png_create_info_struct(result.png);
    if (!result.info)
    {
        if(result.png != nullptr)
        {
            png_destroy_read_struct(&result.png, nullptr, nullptr);
        }
        return std::unexpected("Failed to read PNG info.");
    }

    png_init_io(result.png, result.file);
    png_set_sig_bytes(result.png, HEADER_SIZE);

    png_read_png(result.png, result.info, PNG_TRANSFORM_PACKING |
                 PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND |
                 PNG_TRANSFORM_GRAY_TO_RGB, nullptr);

    // This stores “height” number of pointers to rows.
    result.rows = png_get_rows(result.png, result.info);
    result.img_width = png_get_image_width(result.png, result.info);
    result.img_height = png_get_image_height(result.png, result.info);
    result.bit_depth = png_get_bit_depth(result.png, result.info);
    result.row_size = png_get_rowbytes(result.png, result.info);
    result.color_type = png_get_color_type(result.png, result.info);
    result.pixel_size = result.row_size / result.img_width;

    std::cerr << std::format(
        "Image is {}x{} @{} bits. Each row is {} bytes. "
        "Each pixel is {} bytes.\n",
        result.img_width, result.img_height, result.bit_depth,
        result.row_size, result.pixel_size);
    return std::expected<PNGFile, std::string>{
        std::in_place, std::move(result)};
}

PNGFile::~PNGFile()
{
    if(png != nullptr && info != nullptr)
    {
        png_destroy_read_struct(&png, &info, nullptr);
    }
    if(file != nullptr)
    {
        std::fclose(file);
    }
}

Color8 PNGFile::at(uint32_t x, uint32_t y) const
{
    Color8 color;
    color.r = rows[y][x * pixel_size];
    color.g = rows[y][x * pixel_size + 1];
    color.b = rows[y][x * pixel_size + 2];
    switch(color_type)
    {
    case PNG_COLOR_TYPE_RGB:
        color.a = 255;
        break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
        assert(pixel_size == 4);
        color.a = rows[y][x * pixel_size + 3];
        break;
    default:
        assert(false);
    }
    return color;
}
