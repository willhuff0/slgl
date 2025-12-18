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
        return usage;
    }

    Vulkan::Buffer::Builder::Builder(Vulkan::Context& ctx) : ctx(ctx) { }
    Graphics::Buffer::Ref Vulkan::Buffer::Builder::Build(Graphics::Buffer::Builder& builder) {
        vk::BufferCreateInfo createInfo;
        return std::make_shared<Buffer>(ctx.device.createBuffer(createInfo), builder.getLabel());
    }
}