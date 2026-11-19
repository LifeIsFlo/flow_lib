#pragma once
#include <flow_buttons_mouse.h>
#include <flow_keys.h>
#include <flow_vector2.h>

namespace flow::input
{
    struct KeyEvent
    {
        Keys key;
        bool down;
    };

    struct MouseEvent
    {
        Vector2 deltaPos;
        float scroll;
        uint8_t pressed;
        uint8_t released;
        Vector2 absolutePos;
        Vector2 clientPos;

        void nullify_keep_pos()
        {
            deltaPos = Vector2::ZERO;
            scroll = 0;
            pressed = 0;
            released = 0;
        }

        void nullify_no_keep_pos()
        {
            nullify_keep_pos();
            absolutePos = Vector2::ZERO;
            clientPos = Vector2::ZERO;
        }
    };
}