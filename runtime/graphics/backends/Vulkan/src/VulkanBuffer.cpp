#include "backends/Vulkan/VulkanBuffer.hpp"

#include <vulkan/vulkan.hpp>

#include <memory>

namespace SLGL::Graphics {
    static vk::BufferUsageFlags convertUsage(Graphics::Buffer::Usage::Flags flags) {
        const auto& flagBits = flags.GetBitset();
        vk::BufferUsageFlags usage;
        if (flagBits.test(0)) usage |= vk::BufferUsageFlagBits::eTransferDst;
        if (flagBits.test(1)) usage |= vk::BufferUsageFlagBits::eTransferSrc;
        if (flagBits.test(2)) usage |= vk::BufferUsageFlagBits::eIndexBuffer;
        if (flagBits.test(3)) usage |= vk::BufferUsageFlagBits::eVertexBuffer;
        if (flagBits.test(4)) usage |= vk::BufferUsageFlagBits::eUniformBuffer;
        if (flagBits.test(5)) usage |= vk::BufferUsageFlagBits::eStorageBuffer;
        if (flagBits.test(6)) usage |= vk::BufferUsageFlagBits::eIndirectBuffer;
        return usage;
    }

    Vulkan::Buffer::Builder::Builder(Vulkan::Context& ctx) : ctx(ctx) { }
    Graphics::Buffer::Ref Vulkan::Buffer::Builder::Build(Graphics::Buffer::Builder& builder) {
        auto r = ctx.device.createBuffer(vk::BufferCreateInfo {
            .size = builder.getSize(),
            .usage = convertUsage(builder.getUsage()),
        });

        return std::make_shared<Buffer>(), builder.getLabel());
    }


}