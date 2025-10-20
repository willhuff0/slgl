#pragma once

#include <cstdint>
#include <memory>
#include <fstream>
#include <vector>
#include <cassert>

namespace SLGL::Data {
    struct Image {
        enum class CompressionMode : uint8_t {
            Uncompressed = 0, // 8-bit per channel
            BC7 = 1,
            BC5 = 2,
            BC4 = 3,
        };
        enum class ColorSpace : uint8_t {
            Linear = 0,
            SRGB = 1,
        };

        std::shared_ptr<uint8_t> data = nullptr;

        int width = 0, height = 0;

        int bytesPerSubpixel = 0;
        int channels = 0;

        unsigned long size = 0;

        CompressionMode compressionMode = CompressionMode::Uncompressed;
        ColorSpace colorSpace = ColorSpace::Linear;

        void WriteAsTGA(const std::string& path) const;
    };
}