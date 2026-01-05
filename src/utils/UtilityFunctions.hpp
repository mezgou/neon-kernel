#pragma once

#include <string_view>

#include "Config.hpp"

inline std::string_view EffectTypeToString(EffectType effect) {
    switch (effect) {
        case EffectType::Grayscale: return "Grayscale";
        case EffectType::Blur: return "Blur";
        case EffectType::Inversion: return "Inversion";
        default: return "None";
    }
}
