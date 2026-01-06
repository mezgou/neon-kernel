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

    size_t Width = 0;
    size_t Height = 0;

    uint16_t MaxValue = 255;
};

void P3Handler(std::ifstream& file, Image& image);
void P6Handler(std::ifstream& file, Image& image);

void SkipPPMComments(std::ifstream& file);

std::expected<Image, std::string> GetImage(std::filesystem::path path);
std::expected<std::string, std::string> SaveImage(const std::filesystem::path& path, const Image& image, bool isBinary);
