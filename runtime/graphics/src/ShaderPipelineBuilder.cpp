#include <slgl/runtime/graphics/ShaderPipeline.hpp>

#include <utility>

namespace SLGL::Graphics {
    // Layout
    ShaderPipeline::Layout::Builder::Builder(ShaderPipeline::Layout::Builder::Impl* impl) : impl(impl) { }
    
    ShaderPipeline::Layout::Builder& ShaderPipeline::Layout::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    ShaderPipeline::Layout::Builder& ShaderPipeline::Layout::Builder::AddBindSetLayout(const BindSet::Layout::Ref& bindSetLayout) {
        bindSetLayouts.push_back(bindSetLayout);
        return *this;
    }
    
    const std::string& ShaderPipeline::Layout::Builder::getLabel() const { return label; }
    const std::vector<BindSet::Layout::Ref>& ShaderPipeline::Layout::Builder::getBindSetLayouts() const { return bindSetLayouts; }
    
    ShaderPipeline::Layout::Ref ShaderPipeline::Layout::Builder::Build() { return impl->Build(*this); }
    
    // Render
    ShaderPipeline::Render::Builder::Builder(ShaderPipeline::Render::Builder::Impl* impl, ShaderPipeline::Layout::Ref layout): impl(impl), layout(std::move(layout)) { }
    
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetVertex(const ShaderPipeline::Render::Vertex &newVertex) { vertex = newVertex; return *this; }
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetPrimitive(const ShaderPipeline::Render::Primitive& newPrimitive) { primitive = newPrimitive; return *this; }
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetDepthStencil(const ShaderPipeline::Render::DepthStencil& newDepthStencil) { depthStencil = newDepthStencil; return *this; }
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetMultisample(const ShaderPipeline::Render::Multisample& newMultisample) { multisample = newMultisample; return *this; }
    ShaderPipeline::Render::Builder& ShaderPipeline::Render::Builder::SetFragment(const ShaderPipeline::Render::Fragment& newFragment) { fragment = newFragment; return *this; }
    
    const std::string& ShaderPipeline::Render::Builder::getLabel() const { return label; }
    const ShaderPipeline::Layout::Ref& ShaderPipeline::Render::Builder::getLayout() const { return layout; }
    const ShaderPipeline::Render::Vertex& ShaderPipeline::Render::Builder::getVertex() const { return vertex; }
    const ShaderPipeline::Render::Primitive& ShaderPipeline::Render::Builder::getPrimitive() const { return primitive; }
    const std::optional<ShaderPipeline::Render::DepthStencil>& ShaderPipeline::Render::Builder::getDepthStencil() const { return depthStencil; }
    const ShaderPipeline::Render::Multisample& ShaderPipeline::Render::Builder::getMultisample() const { return multisample; }
    const std::optional<ShaderPipeline::Render::Fragment>& ShaderPipeline::Render::Builder::getFragment() const { return fragment; }
    
    ShaderPipeline::Render::Ref ShaderPipeline::Render::Builder::Build() { return impl->Build(*this); }
}