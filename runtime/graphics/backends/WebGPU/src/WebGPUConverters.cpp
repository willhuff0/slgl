#include "backends/WebGPU/WebGPUConverters.hpp"

namespace SLGL::Graphics {
    wgpu::TextureViewDimension WebGPU::convertTextureViewDimension(Texture::View::Dimension dimension) {
        switch (dimension) {
            case Texture::View::Dimension::_1D: return wgpu::TextureViewDimension::_1D;
            case Texture::View::Dimension::_2D: return wgpu::TextureViewDimension::_2D;
            case Texture::View::Dimension::_2DArray: return wgpu::TextureViewDimension::_2DArray;
            case Texture::View::Dimension::Cube: return wgpu::TextureViewDimension::Cube;
            case Texture::View::Dimension::CubeArray: return wgpu::TextureViewDimension::CubeArray;
            case Texture::View::Dimension::_3D: return wgpu::TextureViewDimension::_3D;
            default: throw std::runtime_error("Unexpected texture view dimension");
        }
    }

    wgpu::TextureFormat WebGPU::convertTextureFormat(Texture::Format format, Texture::ColorSpace colorSpace) {
        switch (format) {
            case Texture::Format::R8: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::R8Unorm;
            case Texture::Format::RG8: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::RG8Unorm;
            case Texture::Format::RGBA8: return colorSpace == Texture::ColorSpace::Linear ? wgpu::TextureFormat::RGBA8Unorm : wgpu::TextureFormat::RGBA8UnormSrgb;

            case Texture::Format::R16Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::R16Float;
            case Texture::Format::RG16Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::RG16Float;
            case Texture::Format::RGBA16Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::RGBA16Float;
            case Texture::Format::R32Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::R32Float;
            case Texture::Format::RG32Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::RG32Float;
            case Texture::Format::RGBA32Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::RGBA32Float;

            case Texture::Format::BC4R: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::BC4RUnorm;
            case Texture::Format::BC5RG: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::BC5RGUnorm;
            case Texture::Format::BC7RGBA: return colorSpace == Texture::ColorSpace::Linear ? wgpu::TextureFormat::BC7RGBAUnorm : wgpu::TextureFormat::BC7RGBAUnormSrgb;

            case Texture::Format::Stencil8: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Stencil8;
            case Texture::Format::Depth16Unorm: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Depth16Unorm;
            case Texture::Format::Depth24Plus: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Depth24Plus;
            case Texture::Format::Depth24PlusStencil8: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Depth24PlusStencil8;
            case Texture::Format::Depth32Float: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Depth32Float;
            case Texture::Format::Depth32FloatStencil8: assert(colorSpace == Texture::ColorSpace::Linear); return wgpu::TextureFormat::Depth32FloatStencil8;

            default: throw std::runtime_error("Unexpected texture format");
        }
    }
    std::pair<Texture::Format, Texture::ColorSpace> WebGPU::convertFormat(wgpu::TextureFormat format) {
        switch (format) {
            case wgpu::TextureFormat::R8Unorm: return std::make_pair(Texture::Format::R8, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RG8Unorm: return std::make_pair(Texture::Format::RG8, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA8Unorm: return std::make_pair(Texture::Format::RGBA8, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA8UnormSrgb: return std::make_pair(Texture::Format::RGBA8, Texture::ColorSpace::SRGB);

            case wgpu::TextureFormat::R16Float: return std::make_pair(Texture::Format::R16Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RG16Float: return std::make_pair(Texture::Format::RG16Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA16Float: return std::make_pair(Texture::Format::RGBA16Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::R32Float: return std::make_pair(Texture::Format::R32Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RG32Float: return std::make_pair(Texture::Format::RG32Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::RGBA32Float: return std::make_pair(Texture::Format::RGBA32Float, Texture::ColorSpace::Linear);

            case wgpu::TextureFormat::BC4RUnorm: return std::make_pair(Texture::Format::BC4R, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC5RGUnorm: return std::make_pair(Texture::Format::BC5RG, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC7RGBAUnorm: return std::make_pair(Texture::Format::BC7RGBA, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::BC7RGBAUnormSrgb: return std::make_pair(Texture::Format::BC7RGBA, Texture::ColorSpace::SRGB);

            case wgpu::TextureFormat::Stencil8: return std::make_pair(Texture::Format::Stencil8, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::Depth16Unorm: return std::make_pair(Texture::Format::Depth16Unorm, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::Depth24Plus: return std::make_pair(Texture::Format::Depth24Plus, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::Depth24PlusStencil8: return std::make_pair(Texture::Format::Depth24PlusStencil8, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::Depth32Float: return std::make_pair(Texture::Format::Depth32Float, Texture::ColorSpace::Linear);
            case wgpu::TextureFormat::Depth32FloatStencil8: return std::make_pair(Texture::Format::Depth32FloatStencil8, Texture::ColorSpace::Linear);

            default: throw std::runtime_error("Unexpected texture format");
        }
    }

    wgpu::TextureViewDimension WebGPU::convertTextureToViewDimension(wgpu::TextureDimension dimension) {
        switch (dimension) {
            case wgpu::TextureDimension::_1D: return wgpu::TextureViewDimension::_1D;
            case wgpu::TextureDimension::_2D: return wgpu::TextureViewDimension::_2D;
            case wgpu::TextureDimension::_3D: return wgpu::TextureViewDimension::_3D;
            default: throw std::runtime_error("Unexpected texture dimension");
        }
    }

    wgpu::OptionalBool WebGPU::convertOptionalBool(bool value) {
        return value ? wgpu::OptionalBool::True : wgpu::OptionalBool::False;
    }
}