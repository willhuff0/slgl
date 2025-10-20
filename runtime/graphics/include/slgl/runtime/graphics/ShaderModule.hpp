#pragma once

#include "Flags.hpp"

#include <string>
#include <memory>

namespace SLGL::Graphics {
    class ShaderModule {
    public:
        typedef std::shared_ptr<ShaderModule> Ref;

        class Source {
        public:
            typedef std::shared_ptr<Source> Ref;

            virtual ~Source() = default;
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
            Builder& SetSource(Source::Ref newSource);

            [[nodiscard]] const std::string &getLabel() const;
            [[nodiscard]] Source::Ref getSource() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Shader Module";
            Source::Ref source = nullptr;
        };

        virtual ~ShaderModule() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
    };
}