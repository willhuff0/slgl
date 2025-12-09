#pragma once

#include "Flags.hpp"
#include "Queue.hpp"

#include <slgl/common/Math.hpp>

#include <string>
#include <memory>
#include <cstdint>

namespace SLGL::Graphics {
    class Texture {
    public:
        typedef std::shared_ptr<Texture> Ref;

        class View {
        public:
            typedef std::shared_ptr<View> Ref;

            enum class Dimension : uint8_t {
                SameAsTexture,
                _1D,
                _2D,
                _2DArray,
                Cube,
                CubeArray,
                _3D,
            };
            enum class Aspect : uint8_t {
                All,
                Stencil,
                Depth,
            };

            class Builder {
            public:
                class Impl {
                public:
                    virtual ~Impl() = default;
                    virtual Ref Build(Builder& builder) = 0;
                };

                explicit Builder(Impl* impl, Texture::Ref texture);

                Builder& SetLabel(const std::string& newLabel);
                Builder& SetDimension(Dimension newDimension);
                Builder& SetBaseMipLevel(uint32_t newBaseMipLevel);
                Builder& SetMipLevelCount(uint32_t newMipLevelCount);
                Builder& SetBaseArrayLayer(uint32_t newBaseArrayLayout);
                Builder& SetArrayLayerCount(uint32_t newArrayLayoutCount);
                Builder& SetAspect(Aspect newAspect);

                [[nodiscard]] const std::string& getLabel() const;
                [[nodiscard]] Texture::Ref getTexture() const;
                [[nodiscard]] Dimension getDimension() const;
                [[nodiscard]] uint32_t getBaseMipLevel() const;
                [[nodiscard]] uint32_t getMipLevelCount() const;
                [[nodiscard]] uint32_t getBaseArrayLayer() const;
                [[nodiscard]] uint32_t getArrayLayerCount() const;
                [[nodiscard]] Aspect getAspect() const;

                Ref Build();

            private:
                Impl* impl;
                std::string label; // Defaults to value from texture + " View"
                Texture::Ref texture;
                Dimension dimension = Dimension::SameAsTexture; // Defaults to value from texture
                uint32_t baseMipLevel = 0;
                uint32_t mipLevelCount = 0; // Defaults to value from texture
                uint32_t baseArrayLayer = 0;
                uint32_t arrayLayerCount = 0; // Defaults to value from texture
                Aspect aspect = Aspect::All;
            };

            virtual ~View() = default;

            virtual void SetLabel(const std::string& newLabel) = 0;

            virtual const std::string& GetLabel() = 0;
        };

        class Usage {
        public:
            typedef Flags<uint8_t> Flags;

            static constexpr uint8_t None         = 0b00000000;
            static constexpr uint8_t Write        = 0b00000001;
            static constexpr uint8_t CopySrc      = 0b00000010;
            static constexpr uint8_t Texture      = 0b00000100;
            static constexpr uint8_t Storage      = 0b00001000;
            static constexpr uint8_t RenderTarget = 0b00010000;
        };
        enum class Dimension : uint8_t {
            _1D,
            _2D,
            _3D,
        };
        enum class Format : uint8_t {
            R8,
            RG8,
            RGBA8,

            R16Float,
            RG16Float,
            RGBA16Float,
            R32Float,
            RG32Float,
            RGBA32Float,

            BC4R,
            BC5RG,
            BC7RGBA,

            Stencil8,
            Depth16Unorm,
            Depth24Plus,
            Depth24PlusStencil8,
            Depth32Float,
            Depth32FloatStencil8,
        };
        enum class ColorSpace : uint8_t {
            Linear,
            SRGB,
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
            Builder& SetUsage(Usage::Flags newUsage);
            Builder& SetSize(glm::ivec3 newSize);
            Builder& SetDimension(Dimension newDimension);
            Builder& SetFormat(Format newFormat);
            Builder& SetColorSpace(ColorSpace newColorSpace);
            Builder& SetMipLevelCount(uint32_t newMipLevelCount);
            Builder& SetSampleCount(uint32_t newSampleCount);

            [[nodiscard]] const std::string& getLabel() const;
            [[nodiscard]] const Usage::Flags& getUsage() const;
            [[nodiscard]] const glm::ivec3& getSize() const;
            [[nodiscard]] Dimension getDimension() const;
            [[nodiscard]] Format getFormat() const;
            [[nodiscard]] ColorSpace getColorSpace() const;
            [[nodiscard]] uint32_t getMipLevelCount() const;
            [[nodiscard]] uint32_t getSampleCount() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Texture";
            Usage::Flags usage = Usage::Write | Usage::Texture;
            glm::ivec3 size = { 0, 0, 0 };
            Dimension dimension = Dimension::_2D;
            Format format = Format::RGBA8;
            ColorSpace colorSpace = ColorSpace::Linear;
            uint32_t mipLevelCount = 1;
            uint32_t sampleCount = 1;
        };

        virtual ~Texture() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
        virtual glm::ivec3 GetSize() = 0;
        virtual Dimension GetDimension() = 0;
        virtual Format GetFormat() = 0;
        virtual ColorSpace GetColorSpace() = 0;
        virtual uint32_t GetMipLevelCount() = 0;
        virtual uint32_t GetSampleCount() = 0;

        virtual uint32_t GetSurfaceFormat() = 0;

        virtual void Write(Queue* queue, void *src, size_t size, int mipLevel, glm::ivec3 offset, glm::ivec3 extent) = 0;
        virtual void Write(Queue* queue, void *src, size_t size, int mipLevel) = 0;
        virtual void Write(Queue* queue, void *src, size_t size) = 0;

        virtual View::Builder CreateView() = 0;
    };
}