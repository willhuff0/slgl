#pragma once

#include <slgl/common/utils/ObserverPattern.hpp>

#include <memory>
#include <string>

namespace SLGL::Platform {
    OBSERVER_PATTERN(WindowSize, int, int)

    class Window : public WindowSizeSubject {
    public:
        typedef std::shared_ptr<Window> Ref;

        Window() = default;
        virtual ~Window() = default;

        [[nodiscard]] virtual bool ShouldClose() = 0;

        [[nodiscard]] virtual int GetWidth() const = 0;
        [[nodiscard]] virtual int GetHeight() const = 0;

        virtual void SetTitle(const std::string& title) = 0;
    };
}