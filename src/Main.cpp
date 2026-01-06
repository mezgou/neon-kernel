#include <print>

#include "Config.hpp"
#include "Image.hpp"
#include "UtilityFunctions.hpp"
#include "ImageProcessor.hpp"
#include "Timer.hpp"

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    Timer timer("Run program");

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

    ApplyEffect(config, image);

    auto result = SaveImage(config.Output, image);
    if (!result) {
        std::println("{}", result.error());
        return 1;
    }
    std::println("{}", result.value());

    return 0;
}
