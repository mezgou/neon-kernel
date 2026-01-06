#pragma once

#include <vector>
#include <cstdint>
#include <expected>
#include <fstream>
#include <filesystem>

struct Image {
    std::vector<uint16_t> R;
    std::vector<uint16_t> G;
    std::vector<uint16_t> B;

    std::string PPMFormat;

    size_t Width = 0;
    size_t Height = 0;

    uint16_t MaxValue = 255;
};

std::expected<Image, std::string> GetImage(const std::filesystem::path& path);
std::expected<std::string, std::string> SaveImage(const std::filesystem::path& path, const Image& image);
