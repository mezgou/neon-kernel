#include <print>
#include <expected>
#include <string>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

#include "Config.hpp"
#include "Image.hpp"
#include "UtilityFunctions.hpp"

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    auto resultConfig = GetAppConfig(argc, argv);
    if (!resultConfig) {
        std::println("{}", resultConfig.error());
        return 1;
    }
    AppConfig config = resultConfig.value();

    std::println("[Debug] config.Input = {}", config.Input.string());
    std::println("[Debug] config.Output = {}", config.Output.string());
    std::println("[Debug] config.CoefEffect = {}", config.CoefEffect.value_or(0.0f));
    std::println("[Debug] config.Effect = {}", EffectTypeToString(config.Effect));
    
    auto resultImage = GetImage(config.Input);
    if (!resultImage) {
        std::println("{}", resultImage.error());
        return 1;
    }
    Image image = resultImage.value();

    std::println("[Debug] image.Width = {}", image.Width);
    std::println("[Debug] image.Height = {}", image.Height);
    std::println("[Debug] image.MaxValue = {}", image.MaxValue);

    auto result = SaveImage(config.Output, image, true);
    if (!result) {
        std::println("{}", resultImage.error());
        return 1;
    }
    std::println("{}", result.value());

    return 0;
}
