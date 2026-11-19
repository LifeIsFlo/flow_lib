#pragma once
#include <flow_fwd.h>
#include <flow_vector2.h>

namespace flow::input
{
    enum MouseButton : uint8_t
    {
        NONE = 0,
        MOUSE_BUTTON_0 = 1 << 0, // Left mouse button: bit 0
        MOUSE_BUTTON_1 = 1 << 1, // Middle mouse button: bit 1
        MOUSE_BUTTON_2 = 1 << 2, // Right mouse button: bit 2
        MOUSE_BUTTON_3 = 1 << 3, // Device-specific: bit 3
        MOUSE_BUTTON_4 = 1 << 4, // Device-specific: bit 4
        MOUSE_BUTTON_5 = 1 << 5, // Device-specific: bit 5
        MOUSE_BUTTON_6 = 1 << 6, // Device-specific: bit 6
        MOUSE_BUTTON_7 = 1 << 7,  // Device-specific: bit 7
        MOUSE_BUTTON_ALL = 0xFF // used for checks and debugging the input protocol
    };

    struct MouseState {
        uint8_t buttonsReleased = 0;
        uint8_t buttonsDown = 0;
        uint8_t buttonsPressed = 0;
        float scroll = 0;
        Vector2 deltaPos = Vector2::ZERO;
        Vector2 absolutePos = Vector2::ZERO;
        Vector2 clientPos = Vector2::ZERO;
    };
}