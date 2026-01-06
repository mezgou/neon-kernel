#pragma once

#include <filesystem>
#include <optional>
#include <expected>

enum class EffectType {
    None,
    Grayscale,
    Blur,
    Inversion,
    Sepia
};

struct AppConfig {
    std::filesystem::path Input;
    std::filesystem::path Output;
    std::optional<int> CoefRadiusConvolution;
    EffectType Effect;
};

std::expected<AppConfig, std::string> GetAppConfig(int argc, char* argv[]);
