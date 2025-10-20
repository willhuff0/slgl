#include <slgl/common/data/Image.hpp>

void SLGL::Data::Image::WriteAsTGA(const std::string& path) const {
    assert(data && size != 0);
    assert(compressionMode == CompressionMode::Uncompressed && bytesPerSubpixel == 1);

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create tga file!");
    }

    unsigned char header[18] = {0};

    // ID Length (no ID)
    header[0] = 0;

    // Color map type (no color map)
    header[1] = 0;

    // Image type (3 = grayscale, 2 = RGB/RGBA)
    header[2] = channels == 1 ? 3 : 2;

    // header[3-7] = 0

    // X-origin (2 bytes)
    header[8] = 0;
    header[9] = 0;

    // Y-origin (2 bytes)
    header[10] = 0;
    header[11] = 0;

    // Width (2 bytes)
    header[12] = width & 0xFF;
    header[13] = (width >> 8) & 0xFF;

    // Height (2 bytes)
    header[14] = height & 0xFF;
    header[15] = (height >> 8) & 0xFF;

    // Pixel depth (bits per pixel)
    header[16] = channels * 8;

    // Image descriptor
    // Bit 5 is set (top-left origin)
    header[17] = channels == 4 ? 0x08 : 0x00;

    file.write(reinterpret_cast<char*>(header), sizeof(header));

    if (channels >= 3) {
        // swap R and B channel
        std::vector<unsigned char> tgaData(size);
        for (int i = 0; i < width * height; i++) {
            int offset = i * channels;
            tgaData[offset] = data.get()[offset + 2]; // B
            tgaData[offset + 1] = data.get()[offset + 1]; // G
            tgaData[offset + 2] = data.get()[offset]; // R
            if (channels == 4) {
                tgaData[offset + 3] = data.get()[offset + 3]; // A
            }
        }
        file.write(reinterpret_cast<char*>(tgaData.data()), size);
    } else {
        file.write(reinterpret_cast<char*>(data.get()), size);
    }

    file.close();
}
