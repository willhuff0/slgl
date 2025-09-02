#pragma once

#include <slgl/runtime/graphics/Texture.hpp>

#include <webgpu/webgpu.hpp>

#include <utility>

namespace SLGL::Graphics::WebGPU {
    wgpu::TextureViewDimension convertTextureViewDimension(Graphics::Texture::View::Dimension dimension);

    wgpu::TextureFormat convertTextureFormat(Graphics::Texture::Format format, Graphics::Texture::ColorSpace colorSpace);
    std::pair<Graphics::Texture::Format, Graphics::Texture::ColorSpace> convertFormat(wgpu::TextureFormat format);

    wgpu::OptionalBool convertOptionalBool(bool value);
}