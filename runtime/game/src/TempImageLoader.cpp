#include <slgl/runtime/game/TempImageLoader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <slgl/runtime/game/deps/stb_image.h>

#include <slgl/runtime/game/deps/float16_t.hpp>
using numeric::float16_t;

#include <memory>
#include <cstdint>

Data::Image LoadImage(const std::string& path, int channels, Data::Image::ColorSpace colorSpace) {
    Data::Image image;

    stbi_uc* data;
    data = stbi_load(
        path.c_str(),
        &image.width,
        &image.height,
        &image.channels,
        channels
    );
    if (data == nullptr) throw std::runtime_error("stbi failed to decode image!");
    if (channels != 0) image.channels = channels;

    image.data = std::shared_ptr<uint8_t>(data, [](void* ptr) { stbi_image_free(ptr); });
    image.bytesPerSubpixel = sizeof(uint8_t);
    image.size = image.width * image.height * image.channels * image.bytesPerSubpixel;
    image.compressionMode = Data::Image::CompressionMode::Uncompressed;
    image.colorSpace = colorSpace;

    return image;
}

Data::Image LoadHDRI(const std::string& path) {
    Data::Image image;

    float* data;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_loadf(
        path.c_str(),
        &image.width,
        &image.height,
        &image.channels,
        4
    );
    stbi_set_flip_vertically_on_load(false);
    if (data == nullptr) throw std::runtime_error("stbi failed to decode image!");

    image.channels = 4;
    image.bytesPerSubpixel = sizeof(float16_t);
    image.size = image.width * image.height * image.channels * image.bytesPerSubpixel;
    image.compressionMode = Data::Image::CompressionMode::Uncompressed;
    image.colorSpace = Data::Image::ColorSpace::Linear;

    image.data = std::shared_ptr<uint8_t>((uint8_t*)(new float16_t[4 * image.width * image.height]), [](void* ptr) { delete[] (float16_t*)ptr; });
    auto* halfFloatData = (float16_t*)image.data.get();
    for (int i = 0; i < 4 * image.width * image.height; i++) {
        halfFloatData[i] = std::min<float>(data[i], (float)std::numeric_limits<float16_t>::max());
    }

    stbi_image_free(data);

    return image;
}
