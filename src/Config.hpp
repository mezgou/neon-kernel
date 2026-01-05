#pragma once

#include <filesystem>
#include <optional>
#include <expected>

enum class EffectType {
    None,
    Grayscale,
    Blur,
    Inversion
};

struct AppConfig {
    std::filesystem::path Input;
    std::filesystem::path Output;
    std::optional<float> CoefEffect;
    EffectType Effect;
};

std::expected<std::string_view, std::string> GetNextArgument(size_t index, std::span<char*> arguments);

std::expected<AppConfig, std::string> GetAppConfig(int argc, char* argv[]);
