#include <slgl/runtime/graphics/BindSet.hpp>

#include <utility>

namespace SLGL::Graphics {
    BindSet::Builder::Builder(Impl* impl, BindSet::Layout::Ref layout) : impl(impl), layout(std::move(layout)) { }

    BindSet::Builder& BindSet::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    BindSet::Builder& BindSet::Builder::SetEntry(uint32_t binding, const BindSet::Entry& entry) {
        assert(layout->GetEntries().find(binding) != layout->GetEntries().end() && "Binding must be in layout!");
        entries.emplace(binding, entry);
        return *this;
    }

    const std::string& BindSet::Builder::getLabel() const { return label; }
    BindSet::Layout::Ref BindSet::Builder::getLayout() const { return layout; }
    const std::unordered_map<uint32_t, BindSet::Entry>& BindSet::Builder::getEntries() const { return entries; }

    BindSet::Ref BindSet::Builder::Build() { return impl->Build(*this); }

    // Layout
    BindSet::Layout::Builder::Builder(BindSet::Layout::Builder::Impl* impl) : impl(impl) { }

    BindSet::Layout::Builder& BindSet::Layout::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    BindSet::Layout::Builder& BindSet::Layout::Builder::AddEntry(uint32_t binding, const BindSet::Layout::Entry &entry) { entries.emplace(binding, entry); return *this; }

    const std::string& BindSet::Layout::Builder::getLabel() const { return label; }
    const std::unordered_map<uint32_t, BindSet::Layout::Entry>& BindSet::Layout::Builder::getEntries() const { return entries; }

    BindSet::Layout::Ref BindSet::Layout::Builder::Build() { return impl->Build(*this); }
}
