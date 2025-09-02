#include "WebGPUConverters.hpp"

namespace SLGL::Graphics {
    wgpu::TextureViewDimension WebGPU::convertTextureViewDimension(Graphics::Texture::View::Dimension dimension) {
        switch (dimension) {
            case Graphics::Texture::View::Dimension::_1D: return wgpu::TextureViewDimension::_1D;
            case Graphics::Texture::View::Dimension::_2D: return wgpu::TextureViewDimension::_2D;
            case Graphics::Texture::View::Dimension::_2DArray: return wgpu::TextureViewDimension::_2DArray;
            case Graphics::Texture::View::Dimension::Cube: return wgpu::TextureViewDimension::Cube;
            case Graphics::Texture::View::Dimension::CubeArray: return wgpu::TextureViewDimension::CubeArray;
            case Graphics::Texture::View::Dimension::_3D: return wgpu::TextureViewDimension::_3D;
            default: throw std::runtime_error("Unexpected texture view dimension");
        }
    }

    wgpu::TextureFormat WebGPU::convertTextureFormat(Graphics::Texture::Format format, Graphics::Texture::ColorSpace colorSpace) {
        switch (format) {
            case Graphics::Texture::Format::R8: assert(colorSpace == Graphics::Texture::ColorSpace::Linear); return wgpu::TextureFormat::R8Unorm;
            case Graphics::Texture::Format::RG8: assert(colorSpace == Graphics::Texture::ColorSpace::Linear); return wgpu::TextureFormat::RG8Unorm;
            case Graphics::Texture::Format::RGBA8: return colorSpace == Graphics::Texture::ColorSpace::Linear ? wgpu::TextureFormat::RGBA8Unorm : wgpu::TextureFormat::RGBA8UnormSrgb;
            case Graphics::Texture::Format::BC4R: assert(colorSpace == Graphics::Texture::ColorSpace::Linear); return wgpu::TextureFormat::BC4RUnorm;
            case Graphics::Texture::Format::BC5RG: assert(colorSpace == Graphics::Texture::ColorSpace::Linear); return wgpu::TextureFormat::BC5RGUnorm;
            case Graphics::Texture::Format::BC7RGBA: return colorSpace == Graphics::Texture::ColorSpace::Linear ? wgpu::TextureFormat::BC7RGBAUnorm : wgpu::TextureFormat::BC7RGBAUnormSrgb;
            default: throw std::runtime_error("Unexpected texture format");
        }
    }
    std::pair<Graphics::Texture::Format, Graphics::Texture::ColorSpace> WebGPU::convertFormat(wgpu::TextureFormat format) {
        switch (format) {
            case wgpu::TextureFormat::R8Unorm: return std::make_pair(Graphics::Texture::Format::R8, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RG8Unorm: return std::make_pair(Graphics::Texture::Format::RG8, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA8Unorm: return std::make_pair(Graphics::Texture::Format::RGBA8, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA8UnormSrgb: return std::make_pair(Graphics::Texture::Format::RGBA8, Graphics::Texture::ColorSpace::SRGB);
            case wgpu::TextureFormat::BC4RUnorm: return std::make_pair(Graphics::Texture::Format::BC4R, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC5RGUnorm: return std::make_pair(Graphics::Texture::Format::BC5RG, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC7RGBAUnorm: return std::make_pair(Graphics::Texture::Format::BC7RGBA, Graphics::Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC7RGBAUnormSrgb: return std::make_pair(Graphics::Texture::Format::BC7RGBA, Graphics::Texture::ColorSpace::SRGB);
            default: throw std::runtime_error("Unexpected texture format");
        }
    }

    wgpu::OptionalBool WebGPU::convertOptionalBool(bool value) {
        return value ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
    }
}