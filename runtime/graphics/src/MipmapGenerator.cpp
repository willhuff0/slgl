#include <slgl/runtime/graphics/MipmapGenerator.hpp>

#include <string>

namespace SLGL::Graphics {
    MipmapGenerator::MipmapGenerator(GFX* gfx, const Data::Shader& shader) {
        ShaderModule::Source::Ref shaderSource = gfx->CreateShaderSource(shader);

        shaderModule = gfx->CreateShaderModule()
                .SetLabel("SLGL Mipmap Generator Shader Module")
                .SetSource(shaderSource)
                .Build();

        bindSetLayout = gfx->CreateBindSetLayout()
                .SetLabel("SLGL Mipmap Generate Bind Set Layout")
                .AddEntry(0, { .sampler = BindSet::Layout::Entry::Sampler { } })
                .AddEntry(1, { .texture = BindSet::Layout::Entry::Texture { } })
                .Build();

        shaderPipelineLayout = gfx->CreateShaderPipelineLayout()
                .SetLabel("SLGL Mipmap Generate Shader Pipeline Layout")
                .AddBindSetLayout(bindSetLayout)
                .Build();

        sampler = gfx->CreateSampler()
                .SetLabel("SLGL Mipmap Generator Sampler")
                .SetAddressModeU(Sampler::AddressMode::Clamp)
                .SetAddressModeV(Sampler::AddressMode::Clamp)
                .SetAddressModeW(Sampler::AddressMode::Clamp)
                .Build();
    }

    void MipmapGenerator::GenerateMipmaps(GFX* gfx, Queue* queue, const Texture::Ref& texture, int mipLevels) {
        ShaderPipeline::Render::Ref pipeline = nullptr;

        auto iter = shaderRenderPipelines.find(texture->GetFormat());
        if (iter != shaderRenderPipelines.end()) {
          pipeline = iter->second;
        } else {
            pipeline = gfx->CreateShaderRenderPipeline(shaderPipelineLayout)
                .SetLabel("SLGL Mipmap Generator Shader Render Pipeline (Format: " + std::to_string((uint8_t)texture->GetFormat()) + ")")
                .SetVertex({
                    .module = shaderModule,
                    .entry = "vs_main",
                })
                .SetFragment({
                    .module = shaderModule,
                    .entry = "fs_main",
                    .targets = {
                        ShaderPipeline::Render::Fragment::ColorTarget {
                            .surfaceFormat = texture->GetSurfaceFormat(),
                        }
                    }
                })
                .Build();
        }

        auto commandEncoder = gfx->CreateCommandEncoder("SLGL Mipmap Generator Command Encoder");

        for (uint32_t mipLevel = 0; mipLevel < mipLevels - 1; mipLevel++) {
            auto inputView = texture->CreateView()
                .SetLabel("SLGL Mipmap Generator Input Texture View")
                .SetBaseMipLevel(mipLevel)
                .Build();
            auto outputView = texture->CreateView()
                .SetLabel("SLGL Mipmap Generator Output Texture View")
                .SetBaseMipLevel(mipLevel + 1)
                .Build();

            auto bindSet = bindSetLayout->Instantiate()
                .SetLabel("SLGL Mipmap Generator Bind Set (Format: " + std::to_string((uint8_t)texture->GetFormat()) + ", Texture: " + texture->GetLabel() + ")")
                .SetEntry(0, { BindSet::Entry::Sampler {
                    .sampler = sampler,
                }})
                .SetEntry(1, { BindSet::Entry::TextureView {
                    .textureView = inputView,
                }})
                .Build();

            commandEncoder->EncodeRenderPass({
                .label = "SLGL Mipmap Generator Render Pass (Level: " + std::to_string(mipLevel) + ")",
                .colorAttachments = {
                    CommandEncoder::RenderPass::ColorAttachment {
                        .textureView = outputView,
                    }
                }
            }, [&](RenderEncoder* renderEncoder) {
                renderEncoder->SetPipeline(pipeline);
                renderEncoder->SetBindSet(0, bindSet);
                renderEncoder->Draw(6);
            });
        }

        gfx->SubmitCommands(queue, commandEncoder->Finish("SLGL Mipmap Generator Command Buffer"));
}
}
