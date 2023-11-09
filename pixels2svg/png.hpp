#include <stdint.h>
#include <expected>
#include <string>
#include <format>

#include <png.h>

struct Color8
{
    std::string hex() const
    {
        return std::format("#{:02x}{:02x}{:02x}", r, g, b);
    }

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class PNGFile
{
public:
    PNGFile(const PNGFile&) = delete;
    PNGFile(PNGFile&& rhs);
    PNGFile& operator=(const PNGFile&) = delete;
    PNGFile& operator=(PNGFile&&) = delete;

    static std::expected<PNGFile, std::string>
    readFromFile(const std::string& filename);

    ~PNGFile();

    Color8 at(uint32_t x, uint32_t y) const;
    uint32_t width() const { return img_width; }
    uint32_t height() const { return img_height; }

private:
    PNGFile() = default;

    static constexpr size_t HEADER_SIZE = 8;

    FILE* file = nullptr;
    png_structp png = nullptr;
    png_infop info = nullptr;

    uint32_t img_width;
    uint32_t img_height;
    uint8_t bit_depth;
    size_t row_size;
    png_byte color_type;
    uint8_t pixel_size;

    png_bytep* rows = nullptr;

};
