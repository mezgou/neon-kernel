#pragma once

#include <string_view>

#include "Config.hpp"
#include "Image.hpp"
#include "Timer.hpp"
#include "ImageProcessor.hpp"

inline std::string_view EffectTypeToString(EffectType effect) {
    switch (effect) {
        case EffectType::Grayscale: return "Grayscale";
        case EffectType::Blur: return "Blur";
        case EffectType::Inversion: return "Inversion";
        default: return "None";
    }
}

inline void PrintDebugInfo(const AppConfig& config, const Image& image) {
    std::println("[Debug] config.Input = {}", config.Input.string());
    std::println("[Debug] config.Output = {}", config.Output.string());
    std::println("[Debug] config.CoefRadiusConvolution = {}", config.CoefRadiusConvolution.value_or(0.0f));
    std::println("[Debug] config.Effect = {}", EffectTypeToString(config.Effect));
    std::println("[Debug] image.Width = {}", image.Width);
    std::println("[Debug] image.Height = {}", image.Height);
    std::println("[Debug] image.MaxValue = {}", image.MaxValue);
}

inline void ApplyEffect(const AppConfig& config, Image& image) {
    Timer timer("Apply effect");
    if (config.Effect == EffectType::Inversion) {
        ApplyInversion(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue);
    } else if (config.Effect == EffectType::Grayscale) {
        ApplyGrayscale(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue);
    } else if (config.Effect == EffectType::Blur) {
        ApplyBlur(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue, config.CoefRadiusConvolution.value_or(1));
    }
}
