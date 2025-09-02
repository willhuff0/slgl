#include <slgl/runtime/graphics/Buffer.hpp>

namespace SLGL::Graphics {
    Buffer::Builder::Builder(Buffer::Builder::Impl* impl) : impl(impl) { }

    Buffer::Builder& Buffer::Builder::SetLabel(const std::string& newLabel) { label = newLabel; return *this; }
    Buffer::Builder& Buffer::Builder::SetUsage(Buffer::Usage::Flags newUsage) { usage = newUsage; return *this;}
    Buffer::Builder& Buffer::Builder::SetSize(uint64_t newSize) { size = newSize; return *this; }

    const std::string &Buffer::Builder::getLabel() const { return label; }
    const Buffer::Usage::Flags& Buffer::Builder::getUsage() const { return usage; }
    uint64_t Buffer::Builder::getSize() const { return size; }

    Buffer::Ref Buffer::Builder::Build() { return impl->Build(*this); }
}