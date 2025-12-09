#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/common/Math.hpp>
#include <slgl/common/data/Image.hpp>

using namespace SLGL;
using namespace SLGL::Graphics;

struct CubeMapCaptureUniforms {
    glm::mat4 viewProjMatrix;
};

struct ConvolutionUniforms {
    float roughness;
};

class CubeMapConverter {
public:
    CubeMapConverter();

    Texture::Ref ConvertEquirectangularToCubeMap(const Texture::Ref& equirectangularMap, const std::string& label = "CubeMapConverter ConvertEquirectangularToCubeMap Result");
    Texture::Ref GenerateIrradianceMap(const Texture::Ref& cubeMap, const std::string& label = "CubeMapConverter Irradiance Map");
    Texture::Ref GenerateRadianceMap(const Texture::Ref& cubeMap, const std::string& label = "CubeMapConverter Radiance Map");

private:
    ShaderPipeline::Render::Ref equirectangularToCubeMapShaderPipeline;
    ShaderPipeline::Render::Ref irradianceShaderPipeline;
    ShaderPipeline::Render::Ref radianceShaderPipeline;
    BindSet::Layout::Ref perFaceBindSetLayout;
    BindSet::Layout::Ref equirectangularToCubeMapPerMapBindSetLayout;
    BindSet::Layout::Ref convolutionPerMapBindSetLayout;
    Buffer::Ref positionBuffer;
    Buffer::Ref convolutionUniformBuffer;
    Sampler::Ref sampler;
    std::vector<BindSet::Ref> faceBindSets;

    void capture(const BindSet::Ref& perMapBindSet, const Texture::Ref& target, ShaderPipeline::Render::Ref shaderPipeline, int mipMap = 0);
};

class EnvBrdfLutGenerator {
public:
    EnvBrdfLutGenerator();

    Texture::Ref GenerateBrdfLut(const std::string& label = "Environment BRDF LUT");

private:
    ShaderPipeline::Render::Ref shaderPipeline;
};

struct SkyboxUniforms {
    glm::mat4 projMatrix;
    glm::mat4 viewMatrix;
};

class Skybox {
public:
    static int GetRadianceMipMapCount();

    explicit Skybox(Texture::Ref cubeMap, Texture::Ref irradianceMap, Texture::Ref radianceMap, Texture::Ref brdfLut);

    static Skybox FromEquirectangularMap(const Texture::Ref& equirectangularMap);
    static Skybox FromEquirectangularMap(const Data::Image& image);

    void Draw(glm::mat4 projMatrix, glm::mat4 viewMatrix, RenderEncoder* renderEncoder);

    [[nodiscard]] Texture::Ref GetIrradianceMap() const;
    [[nodiscard]] Texture::Ref GetRadianceMap() const;
    [[nodiscard]] Texture::Ref GetBrdfLut() const;

private:
    ShaderPipeline::Render::Ref shaderPipeline;
    BindSet::Ref bindSet;
    Buffer::Ref uniformBuffer;
    Buffer::Ref positionBuffer;
    Texture::Ref cubeMap;
    Texture::Ref irradianceMap;
    Texture::Ref radianceMap;
    Texture::Ref brdfLut;
};
