#include "backends/WebGPU/WebGPUShaderRenderPipeline.hpp"

#include "backends/WebGPU/WebGPUShaderPipelineLayout.hpp"
#include "backends/WebGPU/WebGPUShaderModule.hpp"
#include "backends/WebGPU/WebGPUConverters.hpp"

#include <webgpu/webgpu.hpp>

#include <variant>

namespace SLGL::Graphics {
    static wgpu::VertexStepMode convertStepMode(Graphics::ShaderPipeline::Render::Vertex::Buffer::StepMode stepMode) {
        switch (stepMode) {
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::StepMode::Vertex: return wgpu::VertexStepMode::Vertex;
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::StepMode::Instance: return wgpu::VertexStepMode::Instance;
            default: throw std::runtime_error("Unexpected vertex step mode");
        }
    }

    static wgpu::VertexFormat convertAttributeFormat(Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format format) {
        switch (format) {
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x1: return wgpu::VertexFormat::Float32;
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x2: return wgpu::VertexFormat::Float32x2;
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3: return wgpu::VertexFormat::Float32x3;
            case Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x4: return wgpu::VertexFormat::Float32x4;
            default: throw std::runtime_error("Unexpected vertex attribute format");
        }
    }

    static wgpu::VertexState convertVertex(const Graphics::ShaderPipeline::Render::Vertex& vertex) {
        wgpu::VertexState state = wgpu::Default;
        state.module = dynamic_cast<WebGPU::ShaderModule*>(vertex.module.get())->GetHandle();
        state.entryPoint = wgpu::StringView(vertex.entry);

        state.constantCount = vertex.constants.size();
        wgpu::ConstantEntry* constants = new wgpu::ConstantEntry[state.constantCount];
        for (size_t i = 0; const auto& [key, value] : vertex.constants) {
            constants[i] = wgpu::Default;
            constants[i].key = wgpu::StringView(key);
            constants[i].value = value;
            i++;
        }
        state.constants = constants;

        state.bufferCount = vertex.buffers.size();
        wgpu::VertexBufferLayout* buffers = new wgpu::VertexBufferLayout[state.bufferCount];
        for (size_t i = 0; const auto& buffer : vertex.buffers) {
            buffers[i].stepMode = convertStepMode(buffer.stepMode);
            buffers[i].arrayStride = buffer.stride;
            buffers[i].attributeCount = buffer.attributes.size();
            wgpu::VertexAttribute* attributes = new wgpu::VertexAttribute[buffers[i].attributeCount];
            for (size_t j = 0; const auto& attribute : buffer.attributes) {
                attributes[j].format = convertAttributeFormat(attribute.format);
                attributes[j].offset = attribute.offset;
                attributes[j].shaderLocation = attribute.location;
                j++;
            }
            buffers[i].attributes = attributes;
            i++;
        }
        state.buffers = buffers;

        return state;
    }

    static wgpu::PrimitiveTopology convertTopology(Graphics::ShaderPipeline::Render::Primitive::Topology topology) {
        switch (topology) {
            case Graphics::ShaderPipeline::Render::Primitive::Topology::PointList: return wgpu::PrimitiveTopology::PointList;
            case Graphics::ShaderPipeline::Render::Primitive::Topology::LineList: return wgpu::PrimitiveTopology::LineList;
            case Graphics::ShaderPipeline::Render::Primitive::Topology::LineStrip: return wgpu::PrimitiveTopology::LineStrip;
            case Graphics::ShaderPipeline::Render::Primitive::Topology::TriangleList: return wgpu::PrimitiveTopology::TriangleList;
            case Graphics::ShaderPipeline::Render::Primitive::Topology::TriangleStrip: return wgpu::PrimitiveTopology::TriangleStrip;
            default: throw std::runtime_error("Unexpected primitive topology");
        }
    }

    static wgpu::IndexFormat convertIndexFormat(Graphics::ShaderPipeline::Render::Primitive::IndexFormat indexFormat) {
        switch (indexFormat) {
            case Graphics::ShaderPipeline::Render::Primitive::IndexFormat::Undefined: return wgpu::IndexFormat::Undefined;
            case Graphics::ShaderPipeline::Render::Primitive::IndexFormat::Uint16: return wgpu::IndexFormat::Uint16;
            case Graphics::ShaderPipeline::Render::Primitive::IndexFormat::Uint32: return wgpu::IndexFormat::Uint32;
            default: throw std::runtime_error("Unexpected index format");
        }
    }

    static wgpu::FrontFace convertFrontFace(Graphics::ShaderPipeline::Render::Primitive::FrontFace frontFace) {
        switch (frontFace) {
            case Graphics::ShaderPipeline::Render::Primitive::FrontFace::CCW: return wgpu::FrontFace::CCW;
            case Graphics::ShaderPipeline::Render::Primitive::FrontFace::CW: return wgpu::FrontFace::CW;
            default: throw std::runtime_error("Unexpected front face");
        }
    }

    static wgpu::CullMode convertCullMode(Graphics::ShaderPipeline::Render::Primitive::CullMode cullMode) {
        switch (cullMode) {
            case Graphics::ShaderPipeline::Render::Primitive::CullMode::None: return wgpu::CullMode::None;
            case Graphics::ShaderPipeline::Render::Primitive::CullMode::Front: return wgpu::CullMode::Front;
            case Graphics::ShaderPipeline::Render::Primitive::CullMode::Back: return wgpu::CullMode::Back;
            default: throw std::runtime_error("Unexpected cull mode");
        }
    }

    static wgpu::PrimitiveState convertPrimitive(const Graphics::ShaderPipeline::Render::Primitive& primitive) {
        wgpu::PrimitiveState state = wgpu::Default;
        state.topology = convertTopology(primitive.topology);
        state.stripIndexFormat = convertIndexFormat(primitive.indexFormat);
        state.frontFace = convertFrontFace(primitive.frontFace);
        state.cullMode = convertCullMode(primitive.cullMode);
        state.unclippedDepth = !primitive.clipDepth;
        return state;
    }

    static wgpu::TextureFormat convertDepthStencilFormat(Graphics::ShaderPipeline::Render::DepthStencil::Format format) {
        switch (format) {
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Stencil8: return wgpu::TextureFormat::Stencil8;
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Depth16Unorm: return wgpu::TextureFormat::Depth16Unorm;
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Depth24Plus: return wgpu::TextureFormat::Depth24Plus;
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Depth24PlusStencil8: return wgpu::TextureFormat::Depth24PlusStencil8;
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Depth32Float: return wgpu::TextureFormat::Depth32Float;
            case Graphics::ShaderPipeline::Render::DepthStencil::Format::Depth32FloatStencil8: return wgpu::TextureFormat::Depth32FloatStencil8;
            default: throw std::runtime_error("Unexpected depth stencil format");
        }
    }

    static wgpu::CompareFunction convertCompare(Graphics::ShaderPipeline::Render::DepthStencil::CompareMode compareMode) {
        switch (compareMode) {
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::Never: return wgpu::CompareFunction::Never;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::Less: return wgpu::CompareFunction::Less;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::LessEqual: return wgpu::CompareFunction::LessEqual;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::Greater: return wgpu::CompareFunction::Greater;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::GreaterEqual: return wgpu::CompareFunction::GreaterEqual;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::Equal: return wgpu::CompareFunction::Equal;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::NotEqual: return wgpu::CompareFunction::NotEqual;
            case Graphics::ShaderPipeline::Render::DepthStencil::CompareMode::Always: return wgpu::CompareFunction::Always;
            default: throw std::runtime_error("Unexpected depth stencil compare mode");
        }
    }

    static wgpu::StencilOperation convertStencilOp(Graphics::ShaderPipeline::Render::DepthStencil::Face::Op op) {
        switch (op) {
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::Keep: return wgpu::StencilOperation::Keep;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::Zero: return wgpu::StencilOperation::Zero;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::Replace: return wgpu::StencilOperation::Replace;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::Invert: return wgpu::StencilOperation::Invert;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::IncrementClamp: return wgpu::StencilOperation::IncrementClamp;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::DecrementClamp: return wgpu::StencilOperation::DecrementClamp;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::IncrementWrap: return wgpu::StencilOperation::IncrementWrap;
            case Graphics::ShaderPipeline::Render::DepthStencil::Face::Op::DecrementWrap: return wgpu::StencilOperation::DecrementWrap;
            default: throw std::runtime_error("Unexpected stencil op");
        }
    }

    static wgpu::StencilFaceState convertStencilFace(const Graphics::ShaderPipeline::Render::DepthStencil::Face& stencilFace) {
        wgpu::StencilFaceState state = wgpu::Default;
        state.compare = convertCompare(stencilFace.compare);
        state.failOp = convertStencilOp(stencilFace.failOp);
        state.depthFailOp = convertStencilOp(stencilFace.depthFailOp);
        state.passOp = convertStencilOp(stencilFace.passOp);
        return state;
    }

    static wgpu::DepthStencilState convertDepthStencil(const Graphics::ShaderPipeline::Render::DepthStencil& depthStencil) {
        wgpu::DepthStencilState state = wgpu::Default;
        state.format = convertDepthStencilFormat(depthStencil.format);
        state.depthWriteEnabled = WebGPU::convertOptionalBool(depthStencil.depthWrite);
        state.depthCompare = convertCompare(depthStencil.depthCompare);
        state.stencilFront = convertStencilFace(depthStencil.stencilFront);
        state.stencilBack = convertStencilFace(depthStencil.stencilBack);
        state.stencilReadMask = depthStencil.stencilReadMask;
        state.stencilWriteMask = depthStencil.stencilWriteMask;
        state.depthBias = depthStencil.depthBias;
        state.depthBiasSlopeScale = depthStencil.depthBiasSlope;
        state.depthBiasClamp = depthStencil.depthBiasClamp;
        return state;
    }

    static wgpu::MultisampleState convertMultisample(const Graphics::ShaderPipeline::Render::Multisample& multisample) {
        wgpu::MultisampleState state = wgpu::Default;
        state.count = multisample.count;
        state.mask = multisample.mask;
        state.alphaToCoverageEnabled = multisample.alphaToCoverage;
        return state;
    }

    static wgpu::BlendOperation convertBlendOp(Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op op) {
        switch (op) {
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op::Add: return wgpu::BlendOperation::Add;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op::Subtract: return wgpu::BlendOperation::Subtract;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op::ReverseSubtract: return wgpu::BlendOperation::ReverseSubtract;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op::Min: return wgpu::BlendOperation::Min;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Op::Max: return wgpu::BlendOperation::Max;
            default: throw std::runtime_error("Unexpected blend op");
        }
    }

    static wgpu::BlendFactor convertBlendFactor(Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor factor) {
        switch (factor) {
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::Zero: return wgpu::BlendFactor::One;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::One: return wgpu::BlendFactor::One;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::Src: return wgpu::BlendFactor::Src;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::OneMinusSrc: return wgpu::BlendFactor::OneMinusSrc;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::SrcAlpha: return wgpu::BlendFactor::SrcAlpha;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::OneMinusSrcAlpha: return wgpu::BlendFactor::OneMinusSrcAlpha;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::Dst: return wgpu::BlendFactor::Dst;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::OneMinusDst: return wgpu::BlendFactor::OneMinusDst;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::DstAlpha: return wgpu::BlendFactor::DstAlpha;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::OneMinusDstAlpha: return wgpu::BlendFactor::OneMinusDstAlpha;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::Constant: return wgpu::BlendFactor::Constant;
            case Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component::Factor::OneMinusConstant: return wgpu::BlendFactor::OneMinusConstant;
            default: throw std::runtime_error("Unexpected blend factor");
        }
    }

    static wgpu::BlendComponent convertBlendComponent(const Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend::Component& blend) {
        wgpu::BlendComponent component = wgpu::Default;
        component.operation = convertBlendOp(blend.op);
        component.srcFactor = convertBlendFactor(blend.srcFactor);
        component.dstFactor = convertBlendFactor(blend.dstFactor);
        return component;
    }

    static wgpu::BlendState convertBlend(const Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Blend& blend) {
        wgpu::BlendState state = wgpu::Default;
        state.color = convertBlendComponent(blend.color);
        state.alpha = convertBlendComponent(blend.alpha);
        return state;
    }

    static wgpu::ColorWriteMask convertWriteMask(Graphics::ShaderPipeline::Render::Fragment::ColorTarget::Mask::Flags flags) {
        const auto& flagBits = flags.GetBitset();
        WGPUColorWriteMask stage = wgpu::ColorWriteMask::None;
        if (flagBits.test(0)) stage |= wgpu::ColorWriteMask::Red;
        if (flagBits.test(1)) stage |= wgpu::ColorWriteMask::Green;
        if (flagBits.test(2)) stage |= wgpu::ColorWriteMask::Blue;
        if (flagBits.test(3)) stage |= wgpu::ColorWriteMask::Alpha;
        if (flagBits.test(4)) stage |= wgpu::ColorWriteMask::All;
        return stage;
    }

    static wgpu::FragmentState convertFragment(const Graphics::ShaderPipeline::Render::Fragment& fragment) {
        wgpu::FragmentState state = wgpu::Default;
        state.module = dynamic_cast<WebGPU::ShaderModule*>(fragment.module.get())->GetHandle();
        state.entryPoint = wgpu::StringView(fragment.entry);

        state.constantCount = fragment.constants.size();
        wgpu::ConstantEntry* constants = new wgpu::ConstantEntry[state.constantCount];
        for (size_t i = 0; const auto& [key, value] : fragment.constants) {
            constants[i] = wgpu::Default;
            constants[i].key = wgpu::StringView(key);
            constants[i].value = value;
            i++;
        }
        state.constants = constants;

        state.targetCount = fragment.targets.size();
        wgpu::ColorTargetState* targets = new wgpu::ColorTargetState[state.targetCount];
        for (size_t i = 0; const auto& target : fragment.targets) {
            targets[i] = wgpu::Default;
            targets[i].format = static_cast<WGPUTextureFormat>(target.surfaceFormat);
            targets[i].blend = target.blend.has_value() ? new WGPUBlendState(convertBlend(target.blend.value())) : nullptr;
            targets[i].writeMask = convertWriteMask(target.writeMask);
            i++;
        }
        state.targets = targets;

        return state;
    }

    WebGPU::ShaderPipeline::Render::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::ShaderPipeline::Render::Ref WebGPU::ShaderPipeline::Render::Builder::Build(Graphics::ShaderPipeline::Render::Builder &builder) {
        wgpu::RenderPipelineDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.layout = dynamic_cast<Layout*>(builder.getLayout().get())->GetHandle();
        desc.vertex = convertVertex(builder.getVertex());
        desc.primitive = convertPrimitive(builder.getPrimitive());
        desc.depthStencil = builder.getDepthStencil().has_value() ? new WGPUDepthStencilState(convertDepthStencil(builder.getDepthStencil().value())) : nullptr;
        desc.multisample = convertMultisample(builder.getMultisample());
        desc.fragment = builder.getFragment().has_value() ? new WGPUFragmentState(convertFragment(builder.getFragment().value())) : nullptr;
        auto result = std::make_shared<Render>(ctx.device.createRenderPipeline(desc), builder.getLabel());

        // Cleanup
        delete[] desc.vertex.constants;
        delete desc.depthStencil;
        if (desc.fragment != nullptr) {
            delete[] desc.fragment->constants;
            for (size_t i = 0; i < desc.fragment->targetCount; i++) {
                delete desc.fragment->targets[i].blend;
            }
            delete[] desc.fragment->targets;
            delete desc.fragment;
        }

        return result;
    }

    WebGPU::ShaderPipeline::Render::Render(wgpu::RenderPipeline renderPipeline, std::string label)
        : renderPipeline(renderPipeline), label(std::move(label)) { }
    WebGPU::ShaderPipeline::Render::~Render() {
        renderPipeline.release();
    }

    void WebGPU::ShaderPipeline::Render::SetLabel(const std::string &newLabel) {
        label = newLabel;
        renderPipeline.setLabel(wgpu::StringView(label));
    }

    wgpu::RenderPipeline WebGPU::ShaderPipeline::Render::GetHandle() { return renderPipeline; }
    const std::string &WebGPU::ShaderPipeline::Render::GetLabel() { return label; }
}