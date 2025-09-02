#pragma once

namespace SLGL::Graphics::WebGPU {
    SLGL::Data::Shader mipmapGeneratorShader {
      .language = SLGL::Data::Shader::Language::WGSL,
      .source = R""""(
struct VSOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoord: vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VSOutput {
    var pos = array(
        // 1st triangle
        vec2f( 0.0,  0.0),  // center
        vec2f( 1.0,  0.0),  // right, center
        vec2f( 0.0,  1.0),  // center, top

        // 2nd triangle
        vec2f( 0.0,  1.0),  // center, top
        vec2f( 1.0,  0.0),  // right, center
        vec2f( 1.0,  1.0),  // right, top
    );

    var vsOutput: VSOutput;
    let xy = pos[vertexIndex];
    vsOutput.position = vec4f(xy * 2.0 - 1.0, 0.0, 1.0);
    vsOutput.texCoord = vec2f(xy.x, 1.0 - xy.y);
    return vsOutput;
}

@group(0) @binding(0) var inputSampler: sampler;
@group(0) @binding(1) var inputTexture: texture_2d<f32>;

@fragment
fn fs_main(input: VSOutput) -> @location(0) vec4f {
    return textureSample(inputTexture, inputSampler, input.texCoord);
}
)"""",
    };
}
