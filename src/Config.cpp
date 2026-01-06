#include "Config.hpp"

#include <unordered_map>
#include <ranges>
#include <algorithm>

static std::expected<std::string_view, std::string> GetNextArgument(size_t index, std::span<char*> arguments) {
    if (index < arguments.size() && !std::string_view(arguments[index]).starts_with("-")) {
        return std::string_view(arguments[index]);
    }
    return std::unexpected("[Error] Passed an argument with no value");
}

static const std::unordered_map<std::string_view, EffectType> s_Effects = {
    {"grayscale", EffectType::Grayscale},
    {"blur", EffectType::Blur},
    {"inversion", EffectType::Inversion},
    {"sepia", EffectType::Sepia}
};

std::expected<AppConfig, std::string> GetAppConfig(int argc, char* argv[]) {
    AppConfig config;
    config.Output = "output.ppm";
    config.Effect = EffectType::None;

    auto arguments = std::span(argv, argc);
    arguments = arguments.subspan(1);

    for (size_t i = 0; i < arguments.size(); i++) {
        std::string_view argument = arguments[i];

        auto ConsumeValue = [&](std::string_view flag) -> std::expected<std::string_view, std::string> {
            auto result = GetNextArgument(++i, arguments);
            if (!result) return std::unexpected("[Error] Missing value for " + std::string(flag));
            return result;
        };

        if (argument == "-i" || argument == "--input") {
            auto value = ConsumeValue(argument);
            if (!value) return std::unexpected(value.error());
            config.Input = *value;
            continue;
        }
        if (argument == "-o" || argument == "--output") {
            auto value = ConsumeValue(argument);
            if (!value) return std::unexpected(value.error());
            config.Output = *value;
            continue;
        }
        if (argument == "-e" || argument == "--effect") {
            auto value = ConsumeValue(argument);
            if (!value) return std::unexpected(value.error());

            std::string effect = std::string(*value);
            std::ranges::transform(effect, effect.begin(), [](unsigned char c) {
                return std::tolower(c);
            });

            auto iterator = s_Effects.find(std::string_view(effect));
            if (iterator == s_Effects.end()) return std::unexpected("[Error] Effect '" + std::string(effect) + "' not found");
            config.Effect = iterator->second;
            continue;
        }
        if (argument == "-v" || argument == "--value") {
            auto value = ConsumeValue(argument);
            if (!value) return std::unexpected(value.error());

            int coef = 0;
            auto [pointer, errorCode] = std::from_chars((*value).data(), (*value).data() + (*value).size(), coef);
            if (errorCode != std::errc{} || pointer != (*value).data() + (*value).size()) return std::unexpected("[Error] Incorrect input for coefEffect. A number is needed");
            config.CoefRadiusConvolution = coef;
            continue;
        }
        return std::unexpected("[Error] An unknown argument has been entered " + std::string(argument));
    }

    if (config.Input.string().empty()) return std::unexpected("[Error] The required '--input' argument is not entered");
    if (config.Effect == EffectType::None) return std::unexpected("[Error] The required '--effect' argument is not entered");

    return config;
}
