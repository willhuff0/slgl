#include <slgl/runtime/graphics/ShaderModule.hpp>

#include <utility>

namespace SLGL::Graphics {
    ShaderModule::Builder::Builder(ShaderModule::Builder::Impl* impl) : impl(impl) { }

    ShaderModule::Builder& ShaderModule::Builder::SetLabel(const std::string& newLabel) { label = newLabel; return *this; }
    ShaderModule::Builder& ShaderModule::Builder::SetSource(ShaderModule::Source::Ref newSource) { source = std::move(newSource); return *this; }

    const std::string& ShaderModule::Builder::getLabel() const { return label; }
    ShaderModule::Source::Ref ShaderModule::Builder::getSource() const { return source; }

    ShaderModule::Ref ShaderModule::Builder::Build() { return impl->Build(*this); }
}