#include <print>

#include "Config.hpp"
#include "UtilityFunctions.hpp"

int main(int argc, char* argv[]) {
    auto result = GetAppConfig(argc, argv);
    if (!result) {
        std::println("{}", result.error());
        return 1;
    }
    AppConfig config = result.value();

    std::println("[Debug] config.Input = {}", config.Input.string());
    std::println("[Debug] config.Output = {}", config.Output.string());
    std::println("[Debug] config.CoefEffect = {}", config.CoefEffect.value_or(0.0f));
    std::println("[Debug] config.Effect = {}", EffectTypeToString(config.Effect));
    
    return 0;
}
