#pragma once

#include <string>

class IWindow {
public:
    virtual ~IWindow() = default;

    virtual bool Initialize(int width, int height, const std::string& title) = 0;

    virtual bool ShouldClose() = 0;

    [[nodiscard]] virtual int GetWidth() const = 0;
    [[nodiscard]] virtual int GetHeight() const = 0;
};