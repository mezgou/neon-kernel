#include "Image.hpp"

void P3Handler(std::ifstream& file, Image& image) {
    for (size_t i = 0; i < image.Width * image.Height; i++) {
        file >> image.R[i] >> image.G[i] >> image.B[i];
    }
}

void P6Handler(std::ifstream& file, Image& image) {
    file.get();
    size_t totalPixels = image.Width * image.Height;

    if (image.MaxValue < 256) {
        std::vector<uint8_t> buffer(totalPixels * 3);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        for (size_t i = 0; i < totalPixels; i++) {
            image.R[i] = buffer[i * 3 + 0];
            image.G[i] = buffer[i * 3 + 1];
            image.B[i] = buffer[i * 3 + 2];
        }
    } else {
        std::vector<uint8_t> buffer(totalPixels * 3 * 2);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

        auto Get16 = [&](size_t offset) -> uint16_t {
            uint16_t msb = buffer[offset];
            uint16_t lsb = buffer[offset + 1];
            return (msb << 8) | lsb;
        };

        for (size_t i = 0; i < totalPixels; i++) {
            image.R[i] = Get16(i * 6 + 0);
            image.G[i] = Get16(i * 6 + 2);
            image.B[i] = Get16(i * 6 + 4);
        }
    }
}

void SkipPPMComments(std::ifstream& file) {
    file >> std::ws;
    while (file.peek() == '#') {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        file >> std::ws;
    }
}

std::expected<Image, std::string> GetImage(std::filesystem::path path) {
    Image image;

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return std::unexpected("[Error] The file could not be opened. The wrong path may have been specified");
    }

    std::string magickNumber;
    file >> magickNumber;
    if (magickNumber != "P3" && magickNumber != "P6") {
        return std::unexpected("[Error] Incorrect format for PPM image");
    }

    SkipPPMComments(file);
    size_t width = 0, height = 0;
    file >> width >> height;

    SkipPPMComments(file);
    uint16_t maxValue = 255;
    file >> maxValue;

    if (!file) {
        return std::unexpected("[Error] File reading error");
    }

    if (width == 0 || height == 0 || height >= 10000 || width >= 10000) {
        return std::unexpected("[Error] Incorrect resolution of the PPM Image");
    }

    image.R.resize(width * height);
    image.G.resize(width * height);
    image.B.resize(width * height);

    image.Width = width;
    image.Height = height;
    image.MaxValue = maxValue;

    if (magickNumber == "P3") P3Handler(file, image);
    if (magickNumber == "P6") P6Handler(file, image);

    file.close();

    return image;
}