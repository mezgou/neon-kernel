#include "ImageProcessor.hpp"

#include <stdio.h>

#define CALL_CUDA(function) do { \
    cudaError_t error = function; \
    if (error != cudaSuccess) { \
        printf("[CUDA Error] %s", cudaGetErrorString(error)); \
    } \
} while(0)

DeviceImage::DeviceImage(size_t width, size_t height)
    : m_Width(width), m_Height(height) {
    size_t frameSize = width * height * sizeof(unsigned short);
    CALL_CUDA(cudaMalloc(&m_Data, frameSize * 3));

    m_R = m_Data;
    m_G = m_Data + (width * height);
    m_B = m_G + (width * height);
}

DeviceImage::~DeviceImage()
{
    if (m_Data) {
        CALL_CUDA(cudaFree(m_Data));
    }
}

void DeviceImage::Upload(const unsigned short* h_R, const unsigned short* h_G, const unsigned short* h_B)
{
    size_t frameSize = m_Width * m_Height * sizeof(unsigned short);
    CALL_CUDA(cudaMemcpy(m_R, h_R, frameSize, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(m_G, h_G, frameSize, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(m_B, h_B, frameSize, cudaMemcpyHostToDevice));
}

void DeviceImage::Download(unsigned short* h_R, unsigned short* h_G, unsigned short* h_B)
{
    size_t frameSize = m_Width * m_Height * sizeof(unsigned short);
    CALL_CUDA(cudaMemcpy(h_R, m_R, frameSize, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(h_G, m_G, frameSize, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(h_B, m_B, frameSize, cudaMemcpyDeviceToHost));
}

__global__ void GPUImageInversion(unsigned short* r, unsigned short* g, unsigned short* b, uint16_t maxValue, size_t imageResolution) {
    size_t index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < imageResolution) {
        r[index] = maxValue - r[index];
        g[index] = maxValue - g[index];
        b[index] = maxValue - b[index];
    }
}

__global__ void GPUImageGrayscale(unsigned short* r, unsigned short* g, unsigned short* b, size_t imageResolution) {
    size_t index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < imageResolution) {
        float y = 0.299f * r[index] + 0.587f * g[index] + 0.114f * b[index];
        r[index] = (unsigned short)y;
        g[index] = (unsigned short)y;
        b[index] = (unsigned short)y;
    }
}

__global__ void GPUImageBlur(unsigned short* r, unsigned short* g, unsigned short* b, size_t width, size_t height, int radius,
                            unsigned short* dstR, unsigned short* dstG, unsigned short* dstB) {
    size_t x = blockIdx.x * blockDim.x + threadIdx.x;
    size_t y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;

    unsigned long long sumR = 0;
    unsigned long long sumG = 0;
    unsigned long long sumB = 0;
    int count = 0;

    for (int dx = -radius; dx < radius + 1; dx++) {
        for (int dy = -radius; dy < radius + 1; dy++) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                size_t nIndex = ny * width + nx;
                sumR += r[nIndex];
                sumG += g[nIndex];
                sumB += b[nIndex];
                count++;
            }
        }
    }
    size_t index = y * width + x;
    dstR[index] = (unsigned short)(sumR / count);
    dstG[index] = (unsigned short)(sumG / count);
    dstB[index] = (unsigned short)(sumB / count);
}

__global__ void GPUImageSepia(unsigned short* r, unsigned short* g, unsigned short* b, size_t imageResolution, uint16_t maxValue) {
    size_t index = blockIdx.x * blockDim.x + threadIdx.x;
    if (index < imageResolution) {
        float inR = (float)r[index];
        float inG = (float)g[index];
        float inB = (float)b[index];

        float outR = (inR * 0.393f) + (inG * 0.769f) + (inB * 0.189f);
        float outG = (inR * 0.349f) + (inG * 0.686f) + (inB * 0.168f);
        float outB = (inR * 0.272f) + (inG * 0.534f) + (inB * 0.131f);

        r[index] = (unsigned short)fminf(maxValue, outR);
        g[index] = (unsigned short)fminf(maxValue, outG);
        b[index] = (unsigned short)fminf(maxValue, outB);
    }
}

void ApplyInversion(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue) {
    DeviceImage image(width, height);
    image.Upload(R, G, B);

    size_t resolution = width * height;
    GPUImageInversion<<<(resolution + 255) / 256, 256>>>(image.R(), image.G(), image.B(), maxValue, resolution);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());
    
    image.Download(R, G, B);
}

void ApplyGrayscale(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue) {
    DeviceImage image(width, height);
    image.Upload(R, G, B);

    size_t resolution = width * height;
    GPUImageGrayscale<<<(resolution + 255) / 256, 256>>>(image.R(), image.G(), image.B(), resolution);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    image.Download(R, G, B);
}

void ApplyBlur(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue, int radius) {
    DeviceImage src(width, height);
    DeviceImage dst(width, height);

    src.Upload(R, G, B);

    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);
    GPUImageBlur<<<gridSize, blockSize>>>(
        src.R(), src.G(), src.B(),
        width, height, radius,
        dst.R(), dst.G(), dst.B()
    );
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    dst.Download(R, G, B);
}

void ApplySepia(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue) {
    DeviceImage image(width, height);
    image.Upload(R, G, B);

    size_t resolution = width * height;
    GPUImageSepia<<<(resolution + 255) / 256, 256>>>(image.R(), image.G(), image.B(), resolution, maxValue);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    image.Download(R, G, B);
}
