#include "ImageProcessor.hpp"

#include <stdio.h>

#define CALL_CUDA(function) do { \
    cudaError_t error = function; \
    if (error != cudaSuccess) { \
        printf("[CUDA Error] %s", cudaGetErrorString(error)); \
    } \
} while(0)

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

__global__ void GPUImageBlur(unsigned short* r, unsigned short* g, unsigned short* b, size_t width, size_t height, int value,
                            unsigned short* dstR, unsigned short* dstG, unsigned short* dstB) {
    size_t x = blockIdx.x * blockDim.x + threadIdx.x;
    size_t y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;

    unsigned long long sumR = 0;
    unsigned long long sumG = 0;
    unsigned long long sumB = 0;
    int count = 0;

    int radius = 1 * value;
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

void ApplyInversion(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue) {
    size_t imageResolution = width * height;
    size_t threadsPerBlock = 256;
    size_t blocksPerGrid = (imageResolution + threadsPerBlock - 1) / threadsPerBlock;

    unsigned short* r;
    unsigned short* g;
    unsigned short* b;

    CALL_CUDA(cudaMalloc(&r, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&g, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&b, sizeof(unsigned short) * imageResolution));

    CALL_CUDA(cudaMemcpy(r, R, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(g, G, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(b, B, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));

    GPUImageInversion<<<blocksPerGrid, threadsPerBlock>>>(r, g, b, maxValue, imageResolution);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    CALL_CUDA(cudaMemcpy(R, r, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(G, g, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(B, b, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));

    CALL_CUDA(cudaFree(r));
    CALL_CUDA(cudaFree(g));
    CALL_CUDA(cudaFree(b));
}

void ApplyGrayscale(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue) {
    size_t imageResolution = width * height;
    size_t threadsPerBlock = 256;
    size_t blocksPerGrid = (imageResolution + threadsPerBlock - 1) / threadsPerBlock;

    unsigned short* r;
    unsigned short* g;
    unsigned short* b;

    CALL_CUDA(cudaMalloc(&r, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&g, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&b, sizeof(unsigned short) * imageResolution));

    CALL_CUDA(cudaMemcpy(r, R, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(g, G, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(b, B, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));

    GPUImageGrayscale<<<blocksPerGrid, threadsPerBlock>>>(r, g, b, imageResolution);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    CALL_CUDA(cudaMemcpy(R, r, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(G, g, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(B, b, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));

    CALL_CUDA(cudaFree(r));
    CALL_CUDA(cudaFree(g));
    CALL_CUDA(cudaFree(b));
}

void ApplyBlur(unsigned short* R, unsigned short* G, unsigned short* B, size_t width, size_t height, uint16_t maxValue, int value) {
    size_t imageResolution = width * height;
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

    unsigned short* r;
    unsigned short* g;
    unsigned short* b;

    unsigned short* dstR;
    unsigned short* dstG;
    unsigned short* dstB;

    CALL_CUDA(cudaMalloc(&r, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&g, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&b, sizeof(unsigned short) * imageResolution));

    CALL_CUDA(cudaMalloc(&dstR, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&dstG, sizeof(unsigned short) * imageResolution));
    CALL_CUDA(cudaMalloc(&dstB, sizeof(unsigned short) * imageResolution));

    CALL_CUDA(cudaMemcpy(r, R, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(g, G, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));
    CALL_CUDA(cudaMemcpy(b, B, sizeof(unsigned short) * imageResolution, cudaMemcpyHostToDevice));

    GPUImageBlur<<<gridSize, blockSize>>>(r, g, b, width, height, value, dstR, dstG, dstB);
    CALL_CUDA(cudaGetLastError());
    CALL_CUDA(cudaDeviceSynchronize());

    CALL_CUDA(cudaMemcpy(R, dstR, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(G, dstG, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));
    CALL_CUDA(cudaMemcpy(B, dstB, sizeof(unsigned short) * imageResolution, cudaMemcpyDeviceToHost));

    CALL_CUDA(cudaFree(r));
    CALL_CUDA(cudaFree(g));
    CALL_CUDA(cudaFree(b));

    CALL_CUDA(cudaFree(dstR));
    CALL_CUDA(cudaFree(dstG));
    CALL_CUDA(cudaFree(dstB));
}
