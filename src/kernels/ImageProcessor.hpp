#pragma once

#include <cstdint>

class DeviceImage {
public:
    DeviceImage(size_t width, size_t height);
    ~DeviceImage();

    DeviceImage(const DeviceImage&) = delete;
    DeviceImage(DeviceImage&&) = delete;

    DeviceImage& operator=(const DeviceImage&) = delete;
    DeviceImage& operator=(DeviceImage&&) = delete;

    void Upload(const unsigned short* h_R, const unsigned short* h_G, const unsigned short* h_B);
    void Download(unsigned short* h_R, unsigned short* h_G, unsigned short* h_B);

    unsigned short* R() { return m_R; }
    unsigned short* G() { return m_G; }
    unsigned short* B() { return m_B; }

private:
    size_t m_Width = 0;
    size_t m_Height = 0;

    unsigned short* m_Data = nullptr;
    unsigned short* m_R = nullptr;
    unsigned short* m_G = nullptr;
    unsigned short* m_B = nullptr;
};

void ApplyInversion(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue);
void ApplyGrayscale(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue);
void ApplyBlur(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue, int radius);
void ApplySepia(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue);
