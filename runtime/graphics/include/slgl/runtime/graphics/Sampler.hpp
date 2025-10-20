#pragma once

#include "Flags.hpp"

#include <string>
#include <memory>
#include <cstdint>

namespace SLGL::Graphics {
    class Sampler {
    public:
        typedef std::shared_ptr<Sampler> Ref;

        enum class AddressMode : uint8_t  {
            Clamp,
            Repeat,
            Mirror,
        };
        enum class FilterMode : uint8_t  {
            Nearest,
            Linear,
        };
        enum class CompareMode : uint8_t  {
            Undefined,
            Never,
            Less,
            LessEqual,
            Greater,
            GreaterEqual,
            Equal,
            NotEqual,
            Always,
        };

        class Builder {
        public:
            class Impl{
            public:
                virtual ~Impl() = default;
                virtual Ref Build(Builder& builder) = 0;
            };

            explicit Builder(Impl* impl);

            Builder& SetLabel(const std::string& newLabel);
            Builder& SetAddressModeU(AddressMode newAddressModeU);
            Builder& SetAddressModeV(AddressMode newAddressModeV);
            Builder& SetAddressModeW(AddressMode newAddressModeW);
            Builder& SetMagFilter(FilterMode newMagFilter);
            Builder& SetMinFilter(FilterMode newMinFilter);
            Builder& SetMipmapFilter(FilterMode newMipmapFilter);
            Builder& SetCompareMode(CompareMode newCompareMode);
            Builder& SetMinLod(float newMinLod);
            Builder& SetMaxLod(float newMaxLod);
            Builder& SetMaxAnisotropy(uint16_t newMaxAnisotropy);

            [[nodiscard]] const std::string &getLabel() const;
            [[nodiscard]] AddressMode getAddressModeU() const;
            [[nodiscard]] AddressMode getAddressModeV() const;
            [[nodiscard]] AddressMode getAddressModeW() const;
            [[nodiscard]] FilterMode getMagFilter() const;
            [[nodiscard]] FilterMode getMinFilter() const;
            [[nodiscard]] FilterMode getMipmapFilter() const;
            [[nodiscard]] CompareMode getCompareMode() const;
            [[nodiscard]] float getMinLod() const;
            [[nodiscard]] float getMaxLod() const;
            [[nodiscard]] uint16_t getMaxAnisotropy() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Texture";
            AddressMode addressModeU = AddressMode::Repeat;
            AddressMode addressModeV = AddressMode::Repeat;
            AddressMode addressModeW = AddressMode::Repeat;
            FilterMode magFilter = FilterMode::Linear;
            FilterMode minFilter = FilterMode::Linear;
            FilterMode mipmapFilter = FilterMode::Linear;
            CompareMode compareMode = CompareMode::Undefined;
            float minLod = 0.0f;
            float maxLod = 32.0f;
            uint16_t maxAnisotropy = 1;
        };

        virtual ~Sampler() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
    };
}