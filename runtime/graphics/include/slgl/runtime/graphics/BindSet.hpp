#pragma once

#include "Buffer.hpp"
#include "Sampler.hpp"
#include "Texture.hpp"

#include "Flags.hpp"

#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <cstdint>

namespace SLGL::Graphics {
    class GFX;

    class BindSet {
    public:
        typedef std::shared_ptr<BindSet> Ref;

        class Builder;
        class Layout {
        public:
            typedef std::shared_ptr<Layout> Ref;

            struct Entry {
                class Visibility {
                public:
                    typedef Flags<uint8_t> Flags;

                    static constexpr uint8_t None     = 0b00000000;
                    static constexpr uint8_t Vertex   = 0b00000001;
                    static constexpr uint8_t Fragment = 0b00000010;
                    static constexpr uint8_t Compute  = 0b00000100;
                };
                enum class Type : uint8_t {
                    Undefined,
                    Buffer,
                    Sampler,
                    Texture,
                    StorageTexture,
                };

                struct Buffer {
                    enum class Type : uint8_t {
                        Undefined,
                        Uniform,
                        Storage,
                        ReadOnlyStorage,
                    };

                    Type type = Type::Undefined;
                    bool isDynamic = false;
                    uint64_t minSize = 0;
                };
                struct Sampler {
                    enum class Type : uint8_t {
                        Filtering,
                        NonFiltering,
                        Comparison,
                    };

                    Type type = Type::Filtering;
                };
                struct Texture {
                    enum class SampleType : uint8_t {
                        Float,
                        UnfilterableFloat,
                        Depth,
                        Sint,
                        Uint,
                    };

                    SampleType sampleType = SampleType::Float;
                    Graphics::Texture::View::Dimension viewDimension = Graphics::Texture::View::Dimension::_2D;
                    bool multisample = false;
                };
                struct StorageTexture {
                    enum class Access : uint8_t {
                        WriteOnly,
                        ReadOnly,
                        ReadWrite,
                    };
                    enum class ViewDimension : uint8_t {
                        _1D,
                        _2D,
                        _2DArray,
                        Cube,
                        CubeArray,
                        _3D,
                    };

                    Access access = Access::ReadWrite;
                    Graphics::Texture::Format format = Graphics::Texture::Format::RGBA8;
                    Graphics::Texture::ColorSpace colorSpace = Graphics::Texture::ColorSpace::Linear;
                    ViewDimension viewDimension = ViewDimension::_2D;
                };

                Visibility::Flags visibility = Visibility::Vertex | Visibility::Fragment;
                Type type = Type::Undefined;
                union {
                    Buffer buffer;
                    Sampler sampler;
                    Texture texture;
                    StorageTexture storageTexture;
                };
            };

            class Builder {
            public:
                class Impl {
                public:
                    virtual ~Impl() = default;
                    virtual Ref Build(Builder& builder) = 0;
                };

                explicit Builder(Impl* impl);

                Builder& SetLabel(const std::string& newLabel);
                Builder& AddEntry(uint32_t binding, const Entry& entry);

                [[nodiscard]] const std::string& getLabel() const;
                [[nodiscard]] const std::unordered_map<uint32_t, Entry>& getEntries() const;

                Ref Build();

            private:
                Impl* impl;
                std::string label = "SLGL Bind Set Layout";
                std::unordered_map<uint32_t, Entry> entries;
            };

            virtual ~Layout() = default;

            virtual void SetLabel(const std::string& newLabel) = 0;

            virtual const std::string& GetLabel() = 0;
            virtual const std::unordered_map<uint32_t, Entry>& GetEntries() = 0;

            virtual BindSet::Builder Instantiate() = 0;
        };

        struct Entry {
            struct Buffer {
                SLGL::Graphics::Buffer::Ref buffer = nullptr;
                uint64_t offset = 0;
                uint64_t size = 0;
            };
            struct Sampler {
                Graphics::Sampler::Ref sampler = nullptr;
            };
            struct TextureView {
                Graphics::Texture::View::Ref textureView = nullptr;
            };

            Entry(const Buffer& buffer) : value(buffer) { } // NOLINT(*-explicit-constructor)
            Entry(const Sampler& sampler) : value(sampler) { } // NOLINT(*-explicit-constructor)
            Entry(const TextureView& textureView) : value(textureView) { } // NOLINT(*-explicit-constructor)

            std::variant<Buffer, Sampler, TextureView> value;
        };

        class Builder {
        public:
            class Impl {
            public:
                virtual ~Impl() = default;
                virtual BindSet::Ref Build(Builder& builder) = 0;
            };

            explicit Builder(Impl* impl, Layout::Ref layout);

            Builder& SetLabel(const std::string& newLabel);
            Builder& SetEntry(uint32_t binding, const Entry& entry);

            [[nodiscard]] const std::string &getLabel() const;
            [[nodiscard]] Layout::Ref getLayout() const;
            [[nodiscard]] const std::unordered_map<uint32_t, Entry> &getEntries() const;

            BindSet::Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Bind Set";
            Layout::Ref layout;
            std::unordered_map<uint32_t, Entry> entries;
        };

        virtual ~BindSet() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
        virtual Layout::Ref GetLayout() = 0;
    };
}
