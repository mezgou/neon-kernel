#include <print>

#include "Config.hpp"
#include "Image.hpp"
#include "UtilityFunctions.hpp"
#include "ImageProcessor.hpp"

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    auto resultConfig = GetAppConfig(argc, argv);
    if (!resultConfig) {
        std::println("{}", resultConfig.error());
        return 1;
    }
    AppConfig config = resultConfig.value();
    
    auto resultImage = GetImage(config.Input);
    if (!resultImage) {
        std::println("{}", resultImage.error());
        return 1;
    }
    Image image = resultImage.value();

#ifdef _DEBUG
    PrintDebugInfo(config, image);
#endif

    if (config.Effect == EffectType::Inversion) {
        ApplyInversion(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue);
    } else if (config.Effect == EffectType::Grayscale) {
        ApplyGrayscale(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue);
    } else if (config.Effect == EffectType::Blur) {
        ApplyBlur(image.R.data(), image.G.data(), image.B.data(), image.Width, image.Height, image.MaxValue, config.CoefRadiusConvolution.value());
    }

    auto result = SaveImage(config.Output, image, true);
    if (!result) {
        std::println("{}", resultImage.error());
        return 1;
    }
    std::println("{}", result.value());

    return 0;
}
