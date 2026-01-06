#include "Config.hpp"

#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <cctype>

std::expected<std::string_view, std::string> GetNextArgument(size_t index, std::span<char*> arguments) {
    if (index < arguments.size() && !std::string_view(arguments[index]).starts_with("-")) {
        return std::string_view(arguments[index]);
    }
    return std::unexpected("[Error] Passed an argument with no value");
}

std::expected<AppConfig, std::string> GetAppConfig(int argc, char* argv[]) {
    AppConfig config;
    config.Output = "output.ppm";
    config.Effect = EffectType::None;

    const std::unordered_map<std::string_view, EffectType> effects = {
        {"grayscale", EffectType::Grayscale},
        {"blur", EffectType::Blur},
        {"inversion", EffectType::Inversion}
    };

    auto arguments = std::span(argv, argc);
    arguments = arguments.subspan(1);

    for (size_t i = 0; i < arguments.size(); i++) {
        std::string_view argument = arguments[i];
        if (argument == "-i" || argument == "--input") {
            auto result = GetNextArgument(i + 1, arguments);
            if (!result) return std::unexpected("[Error] Missing value for " + std::string(argument));

            config.Input = result.value();
            i++;
            continue;
        }
        if (argument == "-o" || argument == "--output") {
            auto result = GetNextArgument(i + 1, arguments);
            if (!result) return std::unexpected("[Error] Missing value for " + std::string(argument));

            config.Output = result.value();
            i++;
            continue;
        }
        if (argument == "-e" || argument == "--effect") {
            auto result = GetNextArgument(i + 1, arguments);
            if (!result) return std::unexpected("[Error] Missing value for " + std::string(argument));

            std::string_view value = result.value();
            std::string effect = std::string(value);
            std::ranges::transform(effect, effect.begin(), [](unsigned char c) {
                return std::tolower(c);
            });

            auto iterator = effects.find(std::string_view(effect));
            if (iterator == effects.end()) return std::unexpected("[Error] Effect '" + std::string(effect) + "' not found");
            config.Effect = iterator->second;
            i++;
            continue;
        }
        if (argument == "-v" || argument == "--value") {
            auto result = GetNextArgument(i + 1, arguments);
            if (!result) return std::unexpected("[Error] Missing value for " + std::string(argument));

            std::string_view value = result.value();
            float coef = 0.0f;
            auto [pointer, errorCode] = std::from_chars(value.data(), value.data() + value.size(), coef);
            if (errorCode != std::errc{} || pointer != value.data() + value.size()) return std::unexpected("[Error] Incorrect input for coefEffect. A number is needed");
            config.CoefEffect = coef;
            i++;
            continue;
        }
        return std::unexpected("[Error] An unknown argument has been entered " + std::string(argument));
    }

    if (config.Input.string() == "") return std::unexpected("[Error] The required '--input' argument is not entered");
    if (config.Effect == EffectType::None) return std::unexpected("[Error] The required '--effect' argument is not entered");

    return config;
}
