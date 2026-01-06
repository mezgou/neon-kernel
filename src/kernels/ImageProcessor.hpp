#pragma once

#include <cstdint>

void ApplyInversion(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue);
void ApplyGrayscale(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue);
void ApplyBlur(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue, int value);
