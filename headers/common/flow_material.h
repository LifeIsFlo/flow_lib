#pragma once
#include "flow_color.h"
#include <span>
#include <vector>

namespace flow
{
    struct Material
    {
        std::string shaderLocation;
        std::string textureLocation;
        Color baseColor;
    };
}