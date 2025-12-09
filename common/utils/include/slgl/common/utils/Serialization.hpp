#pragma once

#include "InputStream.hpp"

#include <slgl/common/Math.hpp>

#include <concepts>
#include <bit>
#include <type_traits>
#include <utility>
#include <string>
#include <ostream>
#include <ios>
#include <vector>
#include <uuid.h>

namespace Serialization {


#if __cplusplus >= 202302L
#define byteswap std::byteswap
#else
    template<std::integral T>
    T byteswap(T value) {
        size_t size = sizeof(T);
        auto* p = reinterpret_cast<std::uint8_t*>(&value);
        for (size_t i = 0; i < size / 2; ++i) {
            std::swap(p[i], p[size - 1 - i]);
        }
        return value;
    }
#endif

    template<std::integral T>
    [[nodiscard]] constexpr T host_to_network(T value) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return value;
        } else {
            return byteswap(value);
        }
    }

    template<std::integral T>
    [[nodiscard]] constexpr T network_to_host(T value) noexcept {
        return host_to_network(value);
    }

    template<std::integral T>
    void Write(std::ostream& stream, T value) {
        value = host_to_network<T>(value);
        stream.write((char*)&value, sizeof(T));
    }

    inline void Write(std::ostream& stream, float value) {
        Write<uint32_t>(stream, std::bit_cast<uint32_t>(value));
    }
    inline void Write(std::ostream& stream, double value) {
        Write<uint64_t>(stream, std::bit_cast<uint64_t>(value));
    }

    inline void Read(std::istream& stream, void* dest, std::streamsize size) {
        if (!stream.read((char*)dest, size)) {
            if (stream.eof()) {
                throw Serialization::ReadError(ReadError::ErrorType::EndOfFile, "raw data", size, stream.gcount());
            } else {
                throw Serialization::ReadError(ReadError::ErrorType::StreamFailure, "raw data", size, stream.gcount());
            }
        } else if (stream.gcount() != size) {
            throw Serialization::ReadError(ReadError::ErrorType::InsufficientData, "raw data", size, stream.gcount());
        }
    }

    template<typename T>
    T Read(std::istream& stream);

    template<std::integral T>
    T Read(InputStream& stream) {
        T value;
        if (!stream.read((char*)&value, sizeof(T))) {
            if (stream.eof()) {
                throw Serialization::ReadError(ReadError::ErrorType::EndOfFile, "integral type", sizeof(T), stream.gcount());
            } else {
                throw Serialization::ReadError(ReadError::ErrorType::StreamFailure, "integral type", sizeof(T), stream.gcount());
            }
        } else if (stream.gcount() != sizeof(T)) {
            throw Serialization::ReadError(ReadError::ErrorType::InsufficientData, "integral type", sizeof(T), stream.gcount());
        }
        return network_to_host<T>(value);
    }

    template<>
    inline float Read(std::istream& stream) {
        return std::bit_cast<float>(Read<uint32_t>(stream));
    }
    template<>
    inline double Read(std::istream& stream) {
        return std::bit_cast<double>(Read<uint64_t>(stream));
    }

    template<>
    inline glm::vec2 Read(std::istream& stream) {
        return { Read<float>(stream), Read<float>(stream) };
    }
    template<typename T, std::integral CompT>
    requires std::is_base_of_v<glm::ivec2, T>
    glm::ivec2 Read(std::istream& stream) {
        return { Read<CompT>(stream), Read<CompT>(stream) };
    }
    template<>
    inline glm::ivec2 Read(std::istream& stream) {
        return Read<glm::ivec2, uint32_t>(stream);
    }

    template<>
    inline glm::vec3 Read(std::istream& stream) {
        return { Read<float>(stream), Read<float>(stream), Read<float>(stream) };
    }
    template<typename T, std::integral CompT>
    requires std::is_base_of_v<glm::ivec3, T>
    glm::ivec3 Read(std::istream& stream) {
        return { Read<CompT>(stream), Read<CompT>(stream), Read<CompT>(stream) };
    }
    template<>
    inline glm::ivec3 Read(std::istream& stream) {
        return Read<glm::ivec3, uint32_t>(stream);
    }

    template<>
    inline glm::quat Read(std::istream& stream) {
        return { Read<float>(stream), Read<float>(stream), Read<float>(stream), Read<float>(stream) };
    }

    inline void Write(std::ostream& stream, const glm::vec2& value) {
        Write(stream, (float)value.x);
        Write(stream, (float)value.y);
    }
    template<std::integral CompT>
    void Write(std::ostream& stream, const glm::ivec2& value) {
        Write(stream, (CompT)value.x);
        Write(stream, (CompT)value.y);
    }
    template<>
    inline void Write<uint32_t>(std::ostream& stream, const glm::ivec2& value) {
        Write(stream, (uint32_t)value.x);
        Write(stream, (uint32_t)value.y);
    }

    inline void Write(std::ostream& stream, const glm::vec3& value) {
        Write(stream, (float)value.x);
        Write(stream, (float)value.y);
        Write(stream, (float)value.z);
    }
    template<std::integral CompT>
    void Write(std::ostream& stream, const glm::ivec3& value) {
        Write(stream, (CompT)value.x);
        Write(stream, (CompT)value.y);
        Write(stream, (CompT)value.z);
    }
    template<>
    inline void Write<uint32_t>(std::ostream& stream, const glm::ivec3& value) {
        Write(stream, (uint32_t)value.x);
        Write(stream, (uint32_t)value.y);
        Write(stream, (uint32_t)value.z);
    }

    inline void Write(std::ostream& stream, const glm::quat& value) {
        Write(stream, (float)value.x);
        Write(stream, (float)value.y);
        Write(stream, (float)value.z);
        Write(stream, (float)value.w);
    }

    template<std::integral SizeT>
    void Write(std::ostream& stream, const std::string& value) {
        SizeT length = static_cast<SizeT>(value.size());
        Write<SizeT>(stream, length);
        if (length == 0) return;
        stream.write(value.data(), length);
    }
    inline void Write(std::ostream& stream, const std::string& value) { return Write<uint32_t>(stream, value); }

    template<typename T, std::integral SizeT>
    requires std::is_base_of_v<std::string, T>
    std::string Read(std::istream& stream, SizeT maxLength = 0) {
        SizeT length = Read<SizeT>(stream);
        if (length == 0) return {};
        if (maxLength > 0 && length > maxLength) throw std::runtime_error("String is longer than the max length!");
        std::string value;
        value.resize(length);
        if (!stream.read(value.data(), length)) {
            if (stream.eof()) {
                throw Serialization::ReadError(ReadError::ErrorType::EndOfFile, "string", length, stream.gcount());
            } else {
                throw Serialization::ReadError(ReadError::ErrorType::StreamFailure, "string", length, stream.gcount());
            }
        } else if (stream.gcount() != length) {
            throw Serialization::ReadError(ReadError::ErrorType::InsufficientData, "string", length, stream.gcount());
        }
        return value;
    }
    template<>
    inline std::string Read(std::istream& stream) { return Read<std::string, uint32_t>(stream); }

    template<typename ItemT, std::integral SizeT>
    void Write(std::ostream& stream, const std::vector<ItemT>& value) {
        SizeT length = static_cast<SizeT>(value.size());
        Write<SizeT>(stream, length);
        if (length == 0) return;
        stream.write((char*)value.data(), length * sizeof(ItemT));
    }
    template<typename ItemT>
    void Write(std::ostream& stream, const std::vector<ItemT>& value) { return Write<ItemT, uint32_t>(stream, value); }

    template<typename T, typename ItemT, std::integral SizeT>
    requires std::is_base_of_v<std::vector<ItemT>, T>
    std::vector<ItemT> Read(std::istream& stream, SizeT maxLength = 0) {
        SizeT length = Read<SizeT>(stream);
        if (length == 0) return {};
        if (maxLength > 0 && length > maxLength) throw std::runtime_error("Vector is longer than the max length!");
        std::vector<ItemT> value;
        value.resize(length);
        auto size = length * sizeof(ItemT);
        if (!stream.read((char*)value.data(), size)) {
            if (stream.eof()) {
                throw Serialization::ReadError(ReadError::ErrorType::EndOfFile, "vector", size, stream.gcount());
            } else {
                throw Serialization::ReadError(ReadError::ErrorType::StreamFailure, "vector", size, stream.gcount());
            }
        } else if (stream.gcount() != size) {
            throw Serialization::ReadError(ReadError::ErrorType::InsufficientData, "vector", size, stream.gcount());
        }
        return value;
    }
    template<typename T, typename ItemT>
    requires std::is_base_of_v<std::vector<ItemT>, T>
    std::vector<ItemT> Read(std::istream& stream) { return Read<std::vector<ItemT>, ItemT, uint32_t>(stream); }

    inline void Write(std::ostream& stream, const uuids::uuid& uuid) {
        stream.write((char*)uuid.as_bytes().data(), 16);
    }

    template<>
    inline uuids::uuid Read(std::istream& stream) {
        std::array<uuids::uuid::value_type, 16> data {};
        if (!stream.read((char*)&data, 16)) {
            if (stream.eof()) {
                throw Serialization::ReadError(ReadError::ErrorType::EndOfFile, "uuid", 16, stream.gcount());
            } else {
                throw Serialization::ReadError(ReadError::ErrorType::StreamFailure, "uuid", 16, stream.gcount());
            }
        } else if (stream.gcount() != 16) {
            throw Serialization::ReadError(ReadError::ErrorType::InsufficientData, "uuid", 16, stream.gcount());
        }
        return { data };
    }
}
