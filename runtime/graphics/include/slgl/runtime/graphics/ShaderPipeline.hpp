#pragma once

#include "ShaderModule.hpp"
#include "Texture.hpp"
#include "BindSet.hpp"
#include "Queue.hpp"

#include "Flags.hpp"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <cstdint>

namespace SLGL::Graphics::ShaderPipeline {
    class Layout {
    public:
        typedef std::shared_ptr<Layout> Ref;

        class Builder {
        public:
            class Impl {
            public:
                virtual ~Impl() = default;
                virtual Ref Build(Builder& builder) = 0;
            };

            explicit Builder(Impl* impl);

            Builder& SetLabel(const std::string& newLabel);
            Builder& AddBindSetLayout(const BindSet::Layout::Ref& bindSetLayout);

            [[nodiscard]] const std::string &getLabel() const;
            [[nodiscard]] const std::vector<BindSet::Layout::Ref> &getBindSetLayouts() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Shader Pipeline Layout";
            std::vector<BindSet::Layout::Ref> bindSetLayouts;
        };

        virtual ~Layout() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
    };

    class Render {
    public:
        typedef std::shared_ptr<Render> Ref;

        struct Vertex {
            struct Buffer {
                enum class StepMode : uint8_t {
                    Vertex,
                    Instance,
                };
                struct Attribute {
                    enum class Format : uint8_t {
                        Float32x1,
                        Float32x2,
                        Float32x3,
                        Float32x4,
                    };

                    Format format = Format::Float32x1;
                    uint64_t offset = 0;
                    uint32_t location = 0;
                };

                StepMode stepMode = StepMode::Vertex;
                uint64_t stride = 0;
                std::vector<Attribute> attributes;
            };

            ShaderModule::Ref module = nullptr;
            std::string entry;
            std::unordered_map<std::string, double> constants;
            std::vector<Buffer> buffers;
        };
        struct Primitive {
            enum class Topology : uint8_t {
                PointList,
                LineList,
                LineStrip,
                TriangleList,
                TriangleStrip,
            };
            enum class IndexFormat : uint8_t {
                Undefined,
                Uint16,
                Uint32,
            };
            enum class FrontFace : uint8_t {
                CCW,
                CW,
            };
            enum class CullMode : uint8_t {
                None,
                Front,
                Back,
            };

            Topology topology = Topology::TriangleList;
            IndexFormat indexFormat = IndexFormat::Undefined;
            FrontFace frontFace = FrontFace::CCW;
            CullMode cullMode = CullMode::Back;
            bool clipDepth = true;
        };
        struct DepthStencil {
            enum class Format : uint8_t {
                Stencil8,
                Depth16Unorm,
                Depth24Plus,
                Depth24PlusStencil8,
                Depth32Float,
                Depth32FloatStencil8,
            };
            enum class CompareMode : uint8_t {
                Never,
                Less,
                LessEqual,
                Greater,
                GreaterEqual,
                Equal,
                NotEqual,
                Always,
            };
            struct Face {
                enum class Op : uint8_t {
                    Keep,
                    Zero,
                    Replace,
                    Invert,
                    IncrementClamp,
                    DecrementClamp,
                    IncrementWrap,
                    DecrementWrap,
                };

                CompareMode compare = CompareMode::Always;
                Op failOp = Op::Keep;
                Op depthFailOp = Op::Keep;
                Op passOp = Op::Keep;
            };

            Format format = Format::Depth24Plus;
            bool depthWrite = true;
            CompareMode depthCompare = CompareMode::Less;
            Face stencilFront;
            Face stencilBack;
            uint32_t stencilReadMask = 0x00000000;
            uint32_t stencilWriteMask = 0x00000000;
            int32_t depthBias = 0;
            float depthBiasSlope = 0.0f;
            float depthBiasClamp = 0.0f;
        };
        struct Multisample {
            uint32_t count = 1;
            uint32_t mask = 0xFFFFFFFF;
            bool alphaToCoverage = false;
        };
        struct Fragment {
            struct ColorTarget {
                struct Blend {
                    struct Component {
                        enum class Op : uint8_t {
                            Add,
                            Subtract,
                            ReverseSubtract,
                            Min,
                            Max,
                        };
                        enum class Factor : uint8_t {
                            Zero,
                            One,
                            Src,
                            OneMinusSrc,
                            SrcAlpha,
                            OneMinusSrcAlpha,
                            Dst,
                            OneMinusDst,
                            DstAlpha,
                            OneMinusDstAlpha,
                            Constant,
                            OneMinusConstant,
                        };

                        Op op = Op::Add;
                        Factor srcFactor = Factor::One;
                        Factor dstFactor = Factor::Zero;
                    };

                    Component color;
                    Component alpha;
                };

                class Mask {
                public:
                    typedef Flags<uint8_t> Flags;

                    static constexpr uint8_t None = 0b00000000;
                    static constexpr uint8_t R = 0b00000001;
                    static constexpr uint8_t G = 0b00000010;
                    static constexpr uint8_t B = 0b00000100;
                    static constexpr uint8_t A = 0b00001000;
                    static constexpr uint8_t All = 0b00001111;
                };

                uint32_t surfaceFormat = 0;
                std::optional<Blend> blend;
                Mask::Flags writeMask = Mask::All;
            };

            ShaderModule::Ref module;
            std::string entry;
            std::unordered_map<std::string, double> constants;
            std::vector<ColorTarget> targets;
        };

        class Builder {
        public:
            class Impl {
            public:
                virtual ~Impl() = default;
                virtual Ref Build(Builder& builder) = 0;
            };

            Builder(Impl* impl, Layout::Ref layout);

            Builder& SetLabel(const std::string& newLabel);
            Builder& SetVertex(const Vertex& newVertex);
            Builder& SetPrimitive(const Primitive& newPrimitive);
            Builder& SetDepthStencil(const DepthStencil& newDepthStencil);
            Builder& SetMultisample(const Multisample& newMultisample);
            Builder& SetFragment(const Fragment& newFragment);

            [[nodiscard]] const std::string &getLabel() const;
            [[nodiscard]] const Layout::Ref &getLayout() const;
            [[nodiscard]] const Vertex &getVertex() const;
            [[nodiscard]] const Primitive &getPrimitive() const;
            [[nodiscard]] const std::optional<DepthStencil> &getDepthStencil() const;
            [[nodiscard]] const Multisample &getMultisample() const;
            [[nodiscard]] const std::optional<Fragment> &getFragment() const;

            Ref Build();

        private:
            Impl* impl;
            std::string label = "SLGL Shader BeforeRender Pipeline";
            Layout::Ref layout;
            Vertex vertex;
            Primitive primitive;
            std::optional<DepthStencil> depthStencil;
            Multisample multisample;
            std::optional<Fragment> fragment;
        };

        virtual ~Render() = default;

        virtual void SetLabel(const std::string& newLabel) = 0;

        virtual const std::string& GetLabel() = 0;
    };

//    struct Compute {
//        ShaderModule::Ref module;
//        std::string entry;
//        std::unordered_map<std::string, double> constants;
//    };
}