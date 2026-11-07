#pragma once
#include "flow_fwd.h"
#include "flow_vector2.h"

namespace flow::input
{
    struct InputState
    {
        float mouseScroll;
        Vector2 mouseDelta;
        Vector2 mousePos;
        bool mouseDown[3];
    };
}