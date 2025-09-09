#pragma once

#include "Flags.hpp"
#include "Queue.hpp"

#include <string>
#include <memory>
#include <cstdint>

namespace SLGL::Graphics {
    class Buffer {
    public:
        typedef std::shared_ptr<Buffer> Ref;

        class Usage {
        public:
            typedef Flags<uint8_t> Flags;

            static constexpr uint8_t None    = 0b00000000;
            static constexpr uint8_t Write   = 0b00000001;
            static constexpr uint8_t Read    = 0b00000010;
            static constexpr uint8_t Index   = 0b00000100;
            static constexpr uint8_t Vertex  = 0b00001000;
            static constexpr uint8_t Uniform = 0b00010000;
            static constexpr uint8_t Storage = 0b00100000;
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
            Builder& SetSize(uint64_t newSize);

            [[nodiscard]] const std::string& getLabel() const;
            [[nodiscard]] const Usage::Flags& getUsage() const;
            [[nodiscard]] uint64_t getSize() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Buffer";
            Usage::Flags usage = Usage::None;
            uint64_t size = 0;
        };

        virtual ~Buffer() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
        virtual uint64_t GetSize() = 0;

        virtual void Write(Queue* queue, void *src, uint64_t offset, uint64_t size) = 0;
        virtual void Write(Queue* queue, void *src) = 0;
    };
}