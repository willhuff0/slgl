#include "backends/WebGPU/WebGPUTexture.hpp"

#include "backends/WebGPU/WebGPUConverters.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::TextureUsage convertUsage(Graphics::Texture::Usage::Flags flags) {
        const auto& flagBits = flags.GetBitset();
        WGPUTextureUsage usage = wgpu::TextureUsage::None;
        if (flagBits.test(0)) usage |= wgpu::TextureUsage::CopyDst;
        if (flagBits.test(1)) usage |= wgpu::TextureUsage::CopySrc;
        if (flagBits.test(2)) usage |= wgpu::TextureUsage::TextureBinding;
        if (flagBits.test(3)) usage |= wgpu::TextureUsage::StorageBinding;
        if (flagBits.test(4)) usage |= wgpu::TextureUsage::RenderAttachment;
        return usage;
    }

    static int getChannelCount(wgpu::TextureFormat format) {
        switch (format) {
            case wgpu::TextureFormat::R8Unorm: return 1;
            case wgpu::TextureFormat::RG8Unorm: return 2;
            case wgpu::TextureFormat::RGBA8Unorm: return 4;
            case wgpu::TextureFormat::RGBA8UnormSrgb: return 4;
            case wgpu::TextureFormat::BC4RUnorm: return 1;
            case wgpu::TextureFormat::BC5RGUnorm: return 2;
            case wgpu::TextureFormat::BC7RGBAUnorm: return 4;
            case wgpu::TextureFormat::BC7RGBAUnormSrgb: return 4;
            default: throw std::runtime_error("Unexpected texture format");
        }
    }

    static wgpu::TextureDimension convertDimension(Graphics::Texture::Dimension dimension) {
        switch (dimension) {
            case Graphics::Texture::Dimension::_1D: return wgpu::TextureDimension::_1D;
            case Graphics::Texture::Dimension::_2D: return wgpu::TextureDimension::_2D;
            case Graphics::Texture::Dimension::_3D: return wgpu::TextureDimension::_3D;
            default: throw std::runtime_error("Unexpected texture dimension");
        }
    }
    static Graphics::Texture::Dimension convertDimension(wgpu::TextureDimension dimension) {
        switch (dimension) {
            case wgpu::TextureDimension::_1D: return Graphics::Texture::Dimension::_1D;
            case wgpu::TextureDimension::_2D: return Graphics::Texture::Dimension::_2D;
            case wgpu::TextureDimension::_3D: return Graphics::Texture::Dimension::_3D;
            default: throw std::runtime_error("Unexpected texture dimension");
        }
    }

    WebGPU::Texture::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::Texture::Ref WebGPU::Texture::Builder::Build(Graphics::Texture::Builder &builder) {
        wgpu::TextureDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.usage = convertUsage(builder.getUsage());
        desc.size = { static_cast<uint32_t>(builder.getSize().x), static_cast<uint32_t>(builder.getSize().y), 1 };
        desc.dimension = convertDimension(builder.getDimension());
        desc.format = convertTextureFormat(builder.getFormat(), builder.getColorSpace());
        desc.mipLevelCount = builder.getMipLevels();
        desc.sampleCount = builder.getSamples();
        return std::make_shared<Texture>(ctx.device.createTexture(desc), builder.getLabel());
    }

    WebGPU::Texture::Texture(wgpu::Texture texture, std::string label) : texture(texture), label(std::move(label)) { }
    WebGPU::Texture::~Texture() {
        texture.release();
    }

    void WebGPU::Texture::SetLabel(const std::string &newLabel) {
        label = newLabel;
        texture.setLabel(wgpu::StringView(label));
    }

    wgpu::Texture WebGPU::Texture::GetHandle() { return texture; }
    const std::string &WebGPU::Texture::GetLabel() { return label; }
    glm::ivec2 WebGPU::Texture::GetSize() { return { texture.getWidth(), texture.getHeight() }; }
    Graphics::Texture::Dimension WebGPU::Texture::GetDimension() { return convertDimension(texture.getDimension()); }
    Graphics::Texture::Format WebGPU::Texture::GetFormat() { return convertFormat(texture.getFormat()).first; }
    Graphics::Texture::ColorSpace WebGPU::Texture::GetColorSpace() { return convertFormat(texture.getFormat()).second; }
    uint32_t WebGPU::Texture::GetMipLevels() { return texture.getMipLevelCount(); }
    uint32_t WebGPU::Texture::GetSamples() { return texture.getSampleCount(); }

    uint32_t WebGPU::Texture::GetSurfaceFormat() {
        return texture.getFormat();
    }

    void WebGPU::Texture::Write(Graphics::Queue *queue, void *src, size_t size, int mipLevel, glm::ivec2 offset, glm::ivec2 extent) {
        wgpu::TexelCopyTextureInfo dest;
        dest.texture = texture;
        dest.mipLevel = mipLevel;
        dest.origin = { static_cast<uint32_t>(offset.x), static_cast<uint32_t>(offset.y), 0 };
        dest.aspect = wgpu::TextureAspect::All;

        wgpu::TexelCopyBufferLayout srcLayout;
        srcLayout.offset = 0;
        srcLayout.bytesPerRow = texture.getWidth() * getChannelCount(texture.getFormat());
        srcLayout.rowsPerImage = texture.getHeight();

        ((wgpu::Queue)(WGPUQueue)queue).writeTexture(dest, src, size, srcLayout,
                                                     { static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y), 1 });
    }
    void WebGPU::Texture::Write(Graphics::Queue *queue, void *src, size_t size, int mipLevel) {
        Write(queue, src, size, mipLevel, glm::ivec2(0.0f), GetSize());
    }
    void WebGPU::Texture::Write(Graphics::Queue *queue, void *src, size_t size) {
        Write(queue, src, size, 0);
    }

    Graphics::Texture::View::Builder WebGPU::Texture::CreateView() {
        static auto* textureViewBuilderImpl = new WebGPU::Texture::View::Builder();
        return Graphics::Texture::View::Builder(textureViewBuilderImpl, shared_from_this());
    }

    static Graphics::Texture::Format convertResFormat(int channels, SLGL::Data::Image::CompressionMode compressionMode) {
        switch(channels) {
            case 1:
                switch(compressionMode) {
                    case SLGL::Data::Image::CompressionMode::Uncompressed: return Graphics::Texture::Format::R8;
                    case SLGL::Data::Image::CompressionMode::BC4: return Graphics::Texture::Format::BC4R;
                    default: throw std::runtime_error("Unexpected channel/compression combo");
                }
            case 2:
                switch(compressionMode) {
                    case SLGL::Data::Image::CompressionMode::Uncompressed: return Graphics::Texture::Format::RG8;
                    case SLGL::Data::Image::CompressionMode::BC5: return Graphics::Texture::Format::BC5RG;
                    default: throw std::runtime_error("Unexpected channel/compression combo");
                }
            case 3:
                switch(compressionMode) {
                    case SLGL::Data::Image::CompressionMode::Uncompressed: return Graphics::Texture::Format::RGBA8;
                    default: throw std::runtime_error("Unexpected channel/compression combo");
                }
            case 4:
                switch(compressionMode) {
                    case SLGL::Data::Image::CompressionMode::Uncompressed: return Graphics::Texture::Format::RGBA8;
                    case SLGL::Data::Image::CompressionMode::BC7: return Graphics::Texture::Format::BC7RGBA;
                    default: throw std::runtime_error("Unexpected channel/compression combo");
                }
            default: throw std::runtime_error("Unexpected channel count");
        }
    }
    static Graphics::Texture::ColorSpace convertResColorSpace(SLGL::Data::Image::ColorSpace colorSpace) {
        switch(colorSpace) {
            case SLGL::Data::Image::ColorSpace::Linear: return Graphics::Texture::ColorSpace::Linear;
            case SLGL::Data::Image::ColorSpace::SRGB: return Graphics::Texture::ColorSpace::SRGB;
            default: throw std::runtime_error("Unexpected ColorSpace");
        }
    }
    Graphics::Texture::Ref WebGPU::Texture::FromData(WebGPU::Backend* gfx, Graphics::Queue* queue, const std::string& label, const Data::Image& image, int mipLevels) {
        assert(image.compressionMode != SLGL::Data::Image::CompressionMode::Uncompressed || image.bytesPerSubpixel == 1);

        auto result = gfx->CreateTexture()
                .SetLabel(label)
                .SetSize({ image.width, image.height })
                .SetFormat(convertResFormat(image.channels, image.compressionMode))
                .SetColorSpace(convertResColorSpace(image.colorSpace))
                .SetUsage(Graphics::Texture::Usage::Write | Graphics::Texture::Usage::Texture |
                    (mipLevels > 1 ? Graphics::Texture::Usage::RenderTarget : Graphics::Texture::Usage::None))
                .SetMipLevels(mipLevels)
                .Build();

        if (image.channels == 3) {
            assert(image.compressionMode == Data::Image::CompressionMode::Uncompressed);
            // Convert RGB to RGBA
            unsigned char* rgb = image.data.get();
            unsigned char* rgba = new unsigned char[image.width * image.height * 4];
            for (unsigned long i = 0; i < image.width * image.height; i++) {
                rgba[i * 4 + 0] = rgb[i * 3 + 0];
                rgba[i * 4 + 1] = rgb[i * 3 + 1];
                rgba[i * 4 + 2] = rgb[i * 3 + 2];
                rgba[i * 4 + 3] = 255;
            }
            result->Write(queue, rgba, image.width * image.height * 4);
            delete[] rgba;
        } else {
            result->Write(queue, image.data.get(), image.size);
        }

        if (mipLevels > 1) {
            gfx->GetMipmapGenerator()->GenerateMipmaps(gfx, queue, result, mipLevels);
        }

        return result;
    }
}